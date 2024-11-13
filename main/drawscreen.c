#include <lvgl.h>
#include "drawscreen.h"

static LV_STYLE_CONST_INIT(indic_style,
	((static lv_style_const_prop_t []){
		LV_STYLE_CONST_PAD_TOP(3),
		LV_STYLE_CONST_PAD_BOTTOM(3),
		LV_STYLE_CONST_PAD_LEFT(3),
		LV_STYLE_CONST_PAD_RIGHT(3),
		LV_STYLE_CONST_RADIUS(5),
		LV_STYLE_CONST_BORDER_WIDTH(2),
		LV_STYLE_CONST_BORDER_COLOR(LV_COLOR_MAKE(0, 0, 255)),
		LV_STYLE_CONST_BORDER_OPA(LV_OPA_100),
		LV_STYLE_CONST_TEXT_ALIGN(LV_ALIGN_CENTER),
		LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(192, 192, 192)),
		LV_STYLE_CONST_BG_OPA(LV_OPA_0),
		LV_STYLE_CONST_PROPS_END
	}));

static lv_obj_t *mkindic(lv_obj_t *parent, void (*event_cb)(lv_event_t *e))
{
	lv_obj_t *indic = lv_label_create(parent);
	lv_obj_add_style(indic, &indic_style, LV_PART_MAIN);
	lv_obj_set_size(indic, 100, 50);
	if (event_cb) {
		lv_obj_add_event_cb(indic, event_cb, LV_EVENT_DRAW_TASK_ADDED,
				NULL);
		lv_obj_add_flag(indic, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
	}
	return indic;
}

void drawscreen(lv_display_t* disp)
{
	lv_obj_t *scr = lv_display_get_screen_active(disp);
	lv_obj_clean(scr);
	lv_obj_set_style_bg_color(scr, lv_color_make(0, 0, 64), LV_PART_MAIN);
	lv_obj_t *welcome_label = lv_label_create(scr);
	lv_obj_set_width(welcome_label, 220);
	lv_obj_set_height(welcome_label, 16);
	lv_obj_align(welcome_label, LV_ALIGN_CENTER, 0, 0);
	lv_label_set_long_mode(welcome_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_style_text_color(welcome_label, lv_color_white(),
			LV_PART_MAIN);
	lv_label_set_text_static(welcome_label,
			"Welcome text welcome text welcome text welcome text");
	lv_obj_t *tl = mkindic(scr, NULL);
	lv_obj_align(tl, LV_ALIGN_CENTER, -55, -35);
	lv_obj_t *tr = mkindic(scr, NULL);
	lv_obj_align(tr, LV_ALIGN_CENTER, 55, -35);
	lv_obj_t *bl = mkindic(scr, NULL);
	lv_obj_align(bl, LV_ALIGN_CENTER, -55, 35);
	lv_obj_t *br = mkindic(scr, NULL);
	lv_obj_align(br, LV_ALIGN_CENTER, 55, 35);

	lv_label_set_text_static(tl, "TL");
	lv_label_set_text_static(tr, "TR");
	lv_label_set_text_static(bl, "BL");
	lv_label_set_text_static(br, "BR");
}
