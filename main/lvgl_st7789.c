#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_types.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <esp_heap_caps.h>
#include <esp_sleep.h>
#include <esp_timer.h>
#include <esp_err.h>
#include <esp_log.h>
#include <lvgl.h>
#include <esp_lcd_panel_st7789.h>
#include "pins.h"
#include "drawscreen.h"

#define TAG "lvgl_st7789"

#define LV_TICK_PERIOD_MS 1
#define SEND_BUF_SIZE ((TFT_WIDTH * TFT_HEIGHT * \
		LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) / 10)

static bool IRAM_ATTR color_trans_done(esp_lcd_panel_io_handle_t panel_io,
		esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	lv_display_t *disp = (lv_display_t*)user_ctx;
	lv_display_flush_ready(disp);
	return false;
}

void lvgl_display_push(lv_display_t *disp_drv, const lv_area_t *area,
		uint8_t *px_map)
{
	esp_lcd_panel_handle_t panel_handle =
		(esp_lcd_panel_handle_t)lv_display_get_user_data(disp_drv);
	lv_draw_sw_rgb565_swap(px_map, lv_area_get_size(area));
	ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle,
			area->x1, area->y1,
			area->x2 + 1, area->y2 + 1,
			(uint16_t *)px_map));
}

static void lv_tick_task(void *arg) {
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

void app_main(void)
{
	ESP_LOGI(TAG, "Starting");

	ESP_ERROR_CHECK(gpio_set_direction(TFT_BL, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_level(TFT_BL, !TFT_BACKLIGHT_ON));
	vTaskDelay(pdMS_TO_TICKS(500));
	ESP_LOGI(TAG, "Initialize SPI bus");
	ESP_ERROR_CHECK(spi_bus_initialize(TFT_SPI_HOST,
		& (spi_bus_config_t) {
			.mosi_io_num = TFT_MOSI,
			.sclk_io_num = TFT_SCLK,
			.flags = SPICOMMON_BUSFLAG_MASTER
				| SPICOMMON_BUSFLAG_GPIO_PINS,
		},
		SPI_DMA_CH_AUTO
	));
	ESP_LOGI(TAG, "Attach panel IO handle to SPI");
	esp_lcd_panel_io_handle_t io_handle = NULL;
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
		(esp_lcd_spi_bus_handle_t)TFT_SPI_HOST,
		& (esp_lcd_panel_io_spi_config_t) {
			.dc_gpio_num = TFT_DC,
			.cs_gpio_num = TFT_CS,
			.pclk_hz = TFT_SPI_FREQUENCY,
			.lcd_cmd_bits = 8,
			.lcd_param_bits = 8,
			.spi_mode = 0,
			.trans_queue_depth = 17,
		},
		&io_handle
	));
	ESP_LOGI(TAG, "Attach vendor specific module");
	esp_lcd_panel_handle_t panel_handle = NULL;
	ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(
		io_handle,
		& (esp_lcd_panel_dev_config_t) {
			.reset_gpio_num = TFT_RST,
			.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
			.bits_per_pixel = 16,
		},
		&panel_handle
	));
	ESP_LOGI(TAG, "Reset panel");
	ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
	ESP_LOGI(TAG, "Init panel");
	ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
	ESP_LOGI(TAG, "Turn on the screen");
	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
	ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
	ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
	ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
	ESP_LOGI(TAG, "Turn on backlight");
	ESP_ERROR_CHECK(gpio_set_level(TFT_BL, TFT_BACKLIGHT_ON));

	lv_init();
	lv_display_t *disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
	ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(
		io_handle,
		&(esp_lcd_panel_io_callbacks_t) {
			color_trans_done
		},
		disp));
	lv_display_set_user_data(disp, panel_handle);
	lv_display_set_flush_cb(disp, lvgl_display_push);
	static lv_color_t *buf[2];
	for (int i = 0; i < 2; i++) {
		buf[i] = heap_caps_malloc(SEND_BUF_SIZE, MALLOC_CAP_DMA);
		assert(buf[i] != NULL);
	}
	lv_display_set_buffers(disp, buf[0], buf[1], SEND_BUF_SIZE,
		LV_DISPLAY_RENDER_MODE_PARTIAL);
	lv_disp_set_rotation(disp, LV_DISPLAY_ROTATION_90);
	lv_obj_t *scr = lv_display_get_screen_active(disp);

	ESP_LOGI(TAG, "Display size %lu x %lu", lv_obj_get_width(scr),
						lv_obj_get_height(scr));
	ESP_LOGI(TAG, "Draw on screen");
	drawscreen(scr);

	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(
		&(esp_timer_create_args_t) {
			.callback = &lv_tick_task,
			.name = "periodic_display",
		},
		&periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer,
				LV_TICK_PERIOD_MS * 1000));
	const TickType_t xFrequency = configTICK_RATE_HZ / 25;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1) {
		lv_task_handler();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
