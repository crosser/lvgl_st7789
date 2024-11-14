#include <lvgl.h>
#include "drawscreen.h"

static void rect_draw_cb(lv_event_t * e)
{
	lv_obj_t * obj = lv_event_get_target(e);
	lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
	lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);
	if (base_dsc->part != LV_PART_MAIN) return;
	lv_area_t obj_coords;
	lv_obj_get_coords(obj, &obj_coords);
	lv_area_t a = {
		.x1 = 0,
		.x2 = 25,
		.y1 = 0,
		.y2 = 25,
	};
	lv_area_align(&obj_coords, &a, LV_ALIGN_CENTER, 0, 0);
	lv_draw_rect_dsc_t box;
	lv_draw_rect_dsc_init(&box);
	box.border_width = 3;
	box.border_color = lv_color_make(128, 128, 0);
	box.bg_opa = LV_OPA_100;
	box.bg_color = lv_color_make(128, 0, 0);
	lv_draw_rect(base_dsc->layer, &box, &a);
}

static void circle_draw_cb(lv_event_t * e)
{
	lv_obj_t * obj = lv_event_get_target(e);
	lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
	lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);
	if (base_dsc->part != LV_PART_MAIN) return;
	lv_area_t obj_coords;
	lv_obj_get_coords(obj, &obj_coords);
	int c_x = obj_coords.x1 + (obj_coords.x2 - obj_coords.x1) / 2;
	int c_y = obj_coords.y1 + (obj_coords.y2 - obj_coords.y1) / 2;
	lv_draw_arc_dsc_t arc;
	lv_draw_arc_dsc_init(&arc);
	arc.color = lv_color_make(0, 128, 0);
	arc.start_angle = 0;
	arc.end_angle = 360;
	arc.width = 12;
	arc.radius = 12;
	arc.center.x = c_x;
	arc.center.y = c_y;
	lv_draw_arc(base_dsc->layer, &arc);
}

static void triang_draw_cb(lv_event_t * e)
{
	lv_obj_t * obj = lv_event_get_target(e);
	lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
	lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);
	if (base_dsc->part != LV_PART_MAIN) return;
	lv_area_t obj_coords;
	lv_obj_get_coords(obj, &obj_coords);
	int c_x = obj_coords.x1 + (obj_coords.x2 - obj_coords.x1) / 2;
	int c_y = obj_coords.y1 + (obj_coords.y2 - obj_coords.y1) / 2;
	lv_draw_triangle_dsc_t arrow;
	lv_draw_triangle_dsc_init(&arrow);
	arrow.bg_color = lv_color_make(0, 128, 0);
	arrow.p[0].x = c_x;
	arrow.p[0].y = c_y - 12;
	arrow.p[1].x = c_x - 12;
	arrow.p[1].y = c_y + 12;
	arrow.p[2].x = c_x + 12;
	arrow.p[2].y = c_y + 12;
	lv_draw_triangle(base_dsc->layer, &arrow);
}

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

static lv_obj_t *mkbare(lv_obj_t *parent, void (*event_cb)(lv_event_t *e))
{
	lv_obj_t *bare = lv_obj_create(parent);
	lv_obj_remove_style_all(bare);
	lv_obj_set_style_border_width(bare, 0, LV_PART_MAIN);
	lv_obj_set_size(bare, 100, 50);
	if (event_cb) {
		lv_obj_add_event_cb(bare, event_cb, LV_EVENT_DRAW_TASK_ADDED,
				NULL);
		lv_obj_add_flag(bare, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
	}
	return bare;
}

void drawscreen(lv_obj_t *scr)
{
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
	lv_obj_t *tl = mkindic(scr, rect_draw_cb);
	lv_obj_align(tl, LV_ALIGN_CENTER, -60, -35);
	lv_obj_t *tr = mkbare(scr, rect_draw_cb);  // <<<<<-----
	lv_obj_align(tr, LV_ALIGN_CENTER, 60, -35);
	lv_obj_t *bl = mkindic(scr, circle_draw_cb);
	lv_obj_align(bl, LV_ALIGN_CENTER, -60, 35);
	lv_obj_t *br = mkindic(scr, triang_draw_cb);
	lv_obj_align(br, LV_ALIGN_CENTER, 60, 35);

	lv_label_set_text_static(tl, "TL");
	//lv_label_set_text_static(tr, "TR");
	//lv_obj_invalidate(tr);
	lv_label_set_text_static(bl, "BL");
	lv_label_set_text_static(br, "BR");
}
