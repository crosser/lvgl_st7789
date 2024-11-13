#include <lvgl.h>
#include "drawscreen.h"

void drawscreen(lv_display_t* disp)
{
	lv_obj_t *scr = lv_display_get_screen_active(disp);
	lv_obj_clean(scr);
	lv_obj_set_style_bg_color(scr, lv_color_make(0, 0, 64), LV_PART_MAIN);
	lv_obj_t *welcome_label = lv_label_create(scr);
	lv_obj_set_width(welcome_label, lv_pct(80));
	lv_obj_set_height(welcome_label, lv_pct(15));
	lv_obj_align(welcome_label, LV_ALIGN_CENTER, 0, 0);
	lv_label_set_long_mode(welcome_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_style_text_color(welcome_label, lv_color_white(),
			LV_PART_MAIN);
	lv_label_set_text_static(welcome_label,
			"Welcome text welcome text welcome text welcome text");
}
