idf_component_register(
	SRCS "lvgl_st7789.c" "drawscreen.c"
	INCLUDE_DIRS "."
	REQUIRES "driver" "esp_driver_gpio" "esp_driver_spi" "esp_lcd"
)
