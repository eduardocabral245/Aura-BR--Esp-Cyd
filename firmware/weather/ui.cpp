// Interface grafica do Aura (LVGL 9). Tela de 240x320.
// Nao usa nenhuma API do Arduino: so LVGL + app_api.h.
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "app_api.h"

#define AURA_FW_VERSION "2.0 pt-BR"

// ------------------------------------------------------------------ fontes
LV_FONT_DECLARE(lv_font_montserrat_latin_12);
LV_FONT_DECLARE(lv_font_montserrat_latin_14);
LV_FONT_DECLARE(lv_font_montserrat_latin_16);
LV_FONT_DECLARE(lv_font_montserrat_latin_20);
LV_FONT_DECLARE(lv_font_montserrat_temp_56);

#define F12 (&lv_font_montserrat_latin_12)
#define F14 (&lv_font_montserrat_latin_14)
#define F16 (&lv_font_montserrat_latin_16)
#define F20 (&lv_font_montserrat_latin_20)
#define FTEMP (&lv_font_montserrat_temp_56)
#define FSYMBOL_BIG (&lv_font_montserrat_40)  // embutida no LVGL (contem os LV_SYMBOL_*)

// ------------------------------------------------------------------ imagens
LV_IMAGE_DECLARE(icon_blizzard);
LV_IMAGE_DECLARE(icon_blowing_snow);
LV_IMAGE_DECLARE(icon_clear_night);
LV_IMAGE_DECLARE(icon_cloudy);
LV_IMAGE_DECLARE(icon_drizzle);
LV_IMAGE_DECLARE(icon_flurries);
LV_IMAGE_DECLARE(icon_haze_fog_dust_smoke);
LV_IMAGE_DECLARE(icon_heavy_rain);
LV_IMAGE_DECLARE(icon_heavy_snow);
LV_IMAGE_DECLARE(icon_isolated_scattered_tstorms_day);
LV_IMAGE_DECLARE(icon_isolated_scattered_tstorms_night);
LV_IMAGE_DECLARE(icon_mostly_clear_night);
LV_IMAGE_DECLARE(icon_mostly_cloudy_day);
LV_IMAGE_DECLARE(icon_mostly_cloudy_night);
LV_IMAGE_DECLARE(icon_mostly_sunny);
LV_IMAGE_DECLARE(icon_partly_cloudy);
LV_IMAGE_DECLARE(icon_partly_cloudy_night);
LV_IMAGE_DECLARE(icon_scattered_showers_day);
LV_IMAGE_DECLARE(icon_scattered_showers_night);
LV_IMAGE_DECLARE(icon_showers_rain);
LV_IMAGE_DECLARE(icon_sleet_hail);
LV_IMAGE_DECLARE(icon_snow_showers_snow);
LV_IMAGE_DECLARE(icon_strong_tstorms);
LV_IMAGE_DECLARE(icon_sunny);
LV_IMAGE_DECLARE(icon_tornado);
LV_IMAGE_DECLARE(icon_wintry_mix_rain_snow);

LV_IMAGE_DECLARE(image_blizzard);
LV_IMAGE_DECLARE(image_blowing_snow);
LV_IMAGE_DECLARE(image_clear_night);
LV_IMAGE_DECLARE(image_cloudy);
LV_IMAGE_DECLARE(image_drizzle);
LV_IMAGE_DECLARE(image_flurries);
LV_IMAGE_DECLARE(image_haze_fog_dust_smoke);
LV_IMAGE_DECLARE(image_heavy_rain);
LV_IMAGE_DECLARE(image_heavy_snow);
LV_IMAGE_DECLARE(image_isolated_scattered_tstorms_day);
LV_IMAGE_DECLARE(image_isolated_scattered_tstorms_night);
LV_IMAGE_DECLARE(image_mostly_clear_night);
LV_IMAGE_DECLARE(image_mostly_cloudy_day);
LV_IMAGE_DECLARE(image_mostly_cloudy_night);
LV_IMAGE_DECLARE(image_mostly_sunny);
LV_IMAGE_DECLARE(image_partly_cloudy);
LV_IMAGE_DECLARE(image_partly_cloudy_night);
LV_IMAGE_DECLARE(image_scattered_showers_day);
LV_IMAGE_DECLARE(image_scattered_showers_night);
LV_IMAGE_DECLARE(image_showers_rain);
LV_IMAGE_DECLARE(image_sleet_hail);
LV_IMAGE_DECLARE(image_snow_showers_snow);
LV_IMAGE_DECLARE(image_strong_tstorms);
LV_IMAGE_DECLARE(image_sunny);
LV_IMAGE_DECLARE(image_tornado);
LV_IMAGE_DECLARE(image_wintry_mix_rain_snow);

// ------------------------------------------------------------------ cores
#define COL_WHITE 0xFFFFFF
#define COL_SOFT 0xDCEAFA     // texto secundario
#define COL_DIM 0xB8D4F2      // texto terciario
#define COL_RAIN 0xA6E1FF
#define COL_ACCENT 0x4DB8FF
#define COL_GOOD 0x34C98E
#define COL_DANGER 0xFF6B70
#define COL_WARN 0xFFC857
#define COL_INK 0x14304F      // texto escuro sobre botao claro

// ------------------------------------------------------------------ estado
static WeatherData s_data;
static bool s_has_data = false;
static int s_tab = 1;  // 0 = por hora, 1 = 7 dias
static bool s_night_theme = false;

static lv_obj_t* s_lbl_city;
static lv_obj_t* s_lbl_status;
static lv_obj_t* s_lbl_clock;
static lv_obj_t* s_btn_gear;
static lv_obj_t* s_img_hero;
static lv_obj_t* s_lbl_temp;
static lv_obj_t* s_lbl_unit;
static lv_obj_t* s_lbl_cond;
static lv_obj_t* s_lbl_feels;
static lv_obj_t* s_lbl_hilo;
static lv_obj_t* s_tab_btn[2];
static lv_obj_t* s_tab_lbl[2];
static lv_obj_t* s_card[2];  // 0 = por hora, 1 = 7 dias

static lv_obj_t* s_d_day[FORECAST_ROWS];
static lv_obj_t* s_d_icon[FORECAST_ROWS];
static lv_obj_t* s_d_min[FORECAST_ROWS];
static lv_obj_t* s_d_track[FORECAST_ROWS];
static lv_obj_t* s_d_fill[FORECAST_ROWS];
static lv_obj_t* s_d_max[FORECAST_ROWS];

static lv_obj_t* s_h_hour[FORECAST_ROWS];
static lv_obj_t* s_h_icon[FORECAST_ROWS];
static lv_obj_t* s_h_rain[FORECAST_ROWS];
static lv_obj_t* s_h_temp[FORECAST_ROWS];

static lv_obj_t* s_settings = nullptr;
static lv_obj_t* s_location = nullptr;
static lv_obj_t* s_modal = nullptr;

static const LocalizedStrings* S() { return get_strings(g_settings.lang); }

// "Campinas, Sao Paulo" -> "Campinas": o cabecalho tem pouco espaco
static void header_city_text(char* out, size_t n) {
  char name[LABEL_LEN];
  snprintf(name, sizeof(name), "%s", g_settings.location);
  char* comma = strchr(name, ',');
  if (comma) *comma = '\0';
  snprintf(out, n, LV_SYMBOL_GPS "  %s", name);
}

// ------------------------------------------------------------------ escolha de imagens
static const lv_image_dsc_t* choose_image(int code, int is_day) {
  switch (code) {
    case 0: return is_day ? &image_sunny : &image_clear_night;
    case 1: return is_day ? &image_mostly_sunny : &image_mostly_clear_night;
    case 2: return is_day ? &image_partly_cloudy : &image_partly_cloudy_night;
    case 3: return &image_cloudy;
    case 45: case 48: return &image_haze_fog_dust_smoke;
    case 51: case 53: case 55: return &image_drizzle;
    case 56: case 57: return &image_sleet_hail;
    case 61: return is_day ? &image_scattered_showers_day : &image_scattered_showers_night;
    case 63: return &image_showers_rain;
    case 65: return &image_heavy_rain;
    case 66: case 67: return &image_wintry_mix_rain_snow;
    case 71: case 73: case 75: case 85: return &image_snow_showers_snow;
    case 77: return &image_flurries;
    case 80: case 81: return is_day ? &image_scattered_showers_day : &image_scattered_showers_night;
    case 82: return &image_heavy_rain;
    case 86: return &image_heavy_snow;
    case 95: return is_day ? &image_isolated_scattered_tstorms_day : &image_isolated_scattered_tstorms_night;
    case 96: case 99: return &image_strong_tstorms;
    default: return is_day ? &image_mostly_cloudy_day : &image_mostly_cloudy_night;
  }
}

static const lv_image_dsc_t* choose_icon(int code, int is_day) {
  switch (code) {
    case 0: return is_day ? &icon_sunny : &icon_clear_night;
    case 1: return is_day ? &icon_mostly_sunny : &icon_mostly_clear_night;
    case 2: return is_day ? &icon_partly_cloudy : &icon_partly_cloudy_night;
    case 3: return &icon_cloudy;
    case 45: case 48: return &icon_haze_fog_dust_smoke;
    case 51: case 53: case 55: return &icon_drizzle;
    case 56: case 57: return &icon_sleet_hail;
    case 61: return is_day ? &icon_scattered_showers_day : &icon_scattered_showers_night;
    case 63: return &icon_showers_rain;
    case 65: return &icon_heavy_rain;
    case 66: case 67: return &icon_wintry_mix_rain_snow;
    case 71: case 73: case 75: case 85: return &icon_snow_showers_snow;
    case 77: return &icon_flurries;
    case 80: case 81: return is_day ? &icon_scattered_showers_day : &icon_scattered_showers_night;
    case 82: return &icon_heavy_rain;
    case 86: return &icon_heavy_snow;
    case 95: return is_day ? &icon_isolated_scattered_tstorms_day : &icon_isolated_scattered_tstorms_night;
    case 96: case 99: return &icon_strong_tstorms;
    default: return is_day ? &icon_mostly_cloudy_day : &icon_mostly_cloudy_night;
  }
}

// ------------------------------------------------------------------ tema (fundo por clima)
struct Sky {
  uint32_t top;
  uint32_t bottom;
};

static Sky sky_for(int code, int is_day) {
  int c = weather_condition_index(code);
  // grupos: 0 limpo (0,1), 1 parcial (2), 2 nublado (3), 3 neblina (4),
  //         4 chuva (5..9,12), 5 neve (10), 6 tempestade (11)
  int g;
  if (c <= 1) g = 0;
  else if (c == 2) g = 1;
  else if (c == 3) g = 2;
  else if (c == 4) g = 3;
  else if (c == 10) g = 5;
  else if (c == 11) g = 6;
  else g = 4;

  static const Sky day[] = {
    {0x2A6FD0, 0x5FAEEB},  // limpo
    {0x3472C2, 0x74B0E2},  // parcialmente nublado
    {0x56738F, 0x8DA5BC},  // nublado
    {0x71838F, 0xA6B4BF},  // neblina
    {0x38546F, 0x6A88A2},  // chuva
    {0x6487AB, 0xA2BDD6},  // neve
    {0x2B2F4D, 0x585C84},  // tempestade
  };
  static const Sky night[] = {
    {0x08112B, 0x25407C},
    {0x101B33, 0x34496C},
    {0x151E2E, 0x38485E},
    {0x1F2835, 0x435064},
    {0x0D1829, 0x2F4560},
    {0x1B2842, 0x516889},
    {0x12132A, 0x37375F},
  };
  return is_day ? day[g] : night[g];
}

static void style_bg(lv_obj_t* obj, uint32_t top, uint32_t bottom) {
  lv_obj_set_style_bg_color(obj, lv_color_hex(top), 0);
  lv_obj_set_style_bg_grad_color(obj, lv_color_hex(bottom), 0);
  lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
}

static void apply_card_style(lv_obj_t* card) {
  if (s_night_theme) {
    lv_obj_set_style_bg_color(card, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(card, 26, 0);
  } else {
    lv_obj_set_style_bg_color(card, lv_color_hex(0x0B2545), 0);
    lv_obj_set_style_bg_opa(card, 56, 0);
  }
}

static void apply_theme(int code, int is_day) {
  Sky sky = sky_for(code, is_day);
  s_night_theme = !is_day;
  style_bg(lv_screen_active(), sky.top, sky.bottom);
  for (int i = 0; i < 2; i++)
    if (s_card[i]) apply_card_style(s_card[i]);
}

// ------------------------------------------------------------------ utilitarios de objetos
static lv_obj_t* plain_obj(lv_obj_t* parent) {
  lv_obj_t* o = lv_obj_create(parent);
  lv_obj_remove_style_all(o);
  lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  return o;
}

static lv_obj_t* make_label(lv_obj_t* parent, const lv_font_t* font, uint32_t color) {
  lv_obj_t* l = lv_label_create(parent);
  lv_obj_set_style_text_font(l, font, 0);
  lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
  lv_label_set_text(l, "");
  return l;
}

static lv_obj_t* make_fixed_label(lv_obj_t* parent, const lv_font_t* font, uint32_t color, int x, int y, int w,
                                  lv_text_align_t align) {
  lv_obj_t* l = make_label(parent, font, color);
  lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
  lv_obj_set_size(l, w, lv_font_get_line_height(font));
  lv_obj_set_style_text_align(l, align, 0);
  lv_obj_set_pos(l, x, y);
  return l;
}

static lv_obj_t* make_card(lv_obj_t* parent, int x, int y, int w, int h) {
  lv_obj_t* c = plain_obj(parent);
  lv_obj_set_size(c, w, h);
  lv_obj_set_pos(c, x, y);
  lv_obj_set_style_radius(c, 14, 0);
  lv_obj_set_style_border_width(c, 1, 0);
  lv_obj_set_style_border_color(c, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(c, 36, 0);
  apply_card_style(c);
  return c;
}

// Botao simples (retangulo arredondado + texto).
static lv_obj_t* make_button(lv_obj_t* parent, const char* text, int w, int h, uint32_t bg, lv_opa_t bg_opa,
                             uint32_t fg, uint32_t border, lv_opa_t border_opa, const lv_font_t* font) {
  lv_obj_t* b = plain_obj(parent);
  lv_obj_set_size(b, w, h);
  lv_obj_set_style_radius(b, h / 2 > 14 ? 14 : h / 2, 0);
  lv_obj_set_style_bg_color(b, lv_color_hex(bg), 0);
  lv_obj_set_style_bg_opa(b, bg_opa, 0);
  lv_obj_set_style_border_width(b, border_opa ? 1 : 0, 0);
  lv_obj_set_style_border_color(b, lv_color_hex(border), 0);
  lv_obj_set_style_border_opa(b, border_opa, 0);
  lv_obj_set_style_bg_opa(b, bg_opa > 60 ? bg_opa - 60 : bg_opa + 40, LV_STATE_PRESSED);
  lv_obj_set_style_opa(b, 90, LV_STATE_DISABLED);
  lv_obj_add_flag(b, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_t* l = make_label(b, font, fg);
  lv_label_set_text(l, text);
  lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
  lv_obj_set_size(l, w - 12, lv_font_get_line_height(font));
  lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(l);
  return b;
}

static lv_obj_t* button_label(lv_obj_t* btn) { return lv_obj_get_child(btn, 0); }

// ------------------------------------------------------------------ formatacao
static int display_temp(float celsius) {
  float v = g_settings.fahrenheit ? celsius * 9.0f / 5.0f + 32.0f : celsius;
  int r = (int)lroundf(v);
  return r;
}

static void fmt_deg(char* buf, size_t n, float celsius) { snprintf(buf, n, "%d°", display_temp(celsius)); }

static void fmt_hour(char* buf, size_t n, int hour) {
  const LocalizedStrings* s = S();
  if (hour < 0 || hour > 23) {
    snprintf(buf, n, "%s", s->invalid_hour);
    return;
  }
  if (g_settings.h24) {
    snprintf(buf, n, "%02d%s", hour, s->h24_suffix);
  } else if (hour == 12) {
    snprintf(buf, n, "%s", s->noon);
  } else {
    int h12 = hour % 12;
    if (h12 == 0) h12 = 12;
    snprintf(buf, n, "%d%s", h12, hour < 12 ? s->am : s->pm);
  }
}

static lv_color_t temp_color(float c) {
  static const struct {
    float t;
    uint32_t col;
  } stops[] = {{-10, 0x6EA8FF}, {0, 0x5CC8FF}, {10, 0x6FE3C1}, {20, 0xFFE066}, {30, 0xFF9F43}, {40, 0xFF5A4F}};
  const int n = sizeof(stops) / sizeof(stops[0]);
  if (c <= stops[0].t) return lv_color_hex(stops[0].col);
  if (c >= stops[n - 1].t) return lv_color_hex(stops[n - 1].col);
  for (int i = 0; i < n - 1; i++) {
    if (c <= stops[i + 1].t) {
      float f = (c - stops[i].t) / (stops[i + 1].t - stops[i].t);
      return lv_color_mix(lv_color_hex(stops[i + 1].col), lv_color_hex(stops[i].col), (uint8_t)(f * 255));
    }
  }
  return lv_color_hex(stops[n - 1].col);
}

// ------------------------------------------------------------------ atualizacao da tela principal
static bool main_built() { return s_lbl_temp != nullptr && lv_obj_is_valid(s_lbl_temp); }

static void layout_hero_unit() {
  lv_obj_update_layout(s_lbl_temp);
  lv_obj_align_to(s_lbl_unit, s_lbl_temp, LV_ALIGN_OUT_RIGHT_TOP, 2, 3);
}

static void update_hero() {
  if (!main_built()) return;
  const LocalizedStrings* s = S();
  lv_label_set_text(s_lbl_unit, g_settings.fahrenheit ? "F" : "C");
  if (!s_has_data) {
    lv_label_set_text(s_lbl_temp, "--°");
    lv_label_set_text(s_lbl_cond, "");
    lv_label_set_text(s_lbl_feels, "");
    lv_label_set_text(s_lbl_hilo, "");
    layout_hero_unit();
    return;
  }
  char buf[48];
  snprintf(buf, sizeof(buf), "%d°", display_temp(s_data.cur.temp_c));
  lv_label_set_text(s_lbl_temp, buf);
  layout_hero_unit();
  lv_label_set_text(s_lbl_cond, s->cond[weather_condition_index(s_data.cur.code)]);
  snprintf(buf, sizeof(buf), "%s %d°", s->feels_like, display_temp(s_data.cur.feels_c));
  lv_label_set_text(s_lbl_feels, buf);
  lv_image_set_src(s_img_hero, choose_image(s_data.cur.code, s_data.cur.is_day));
  snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %d°   " LV_SYMBOL_DOWN " %d°", display_temp(s_data.daily[0].max_c), display_temp(s_data.daily[0].min_c));
  lv_label_set_text(s_lbl_hilo, buf);
}

static void update_daily() {
  if (!main_built() || !s_has_data) return;
  const LocalizedStrings* s = S();
  float wk_min = s_data.daily[0].min_c, wk_max = s_data.daily[0].max_c;
  for (int i = 1; i < FORECAST_ROWS; i++) {
    if (s_data.daily[i].min_c < wk_min) wk_min = s_data.daily[i].min_c;
    if (s_data.daily[i].max_c > wk_max) wk_max = s_data.daily[i].max_c;
  }
  float range = wk_max - wk_min;
  if (range < 1.0f) range = 1.0f;
  const int track_w = 52;
  for (int i = 0; i < FORECAST_ROWS; i++) {
    const WeatherDay& d = s_data.daily[i];
    bool use_name_for_today = (i == 0 && g_settings.lang != LANG_FR);
    lv_label_set_text(s_d_day[i], use_name_for_today ? s->today : s->weekdays[d.dow % 7]);
    char b[16];
    fmt_deg(b, sizeof(b), d.min_c);
    lv_label_set_text(s_d_min[i], b);
    fmt_deg(b, sizeof(b), d.max_c);
    lv_label_set_text(s_d_max[i], b);
    lv_image_set_src(s_d_icon[i], choose_icon(d.code, i == 0 ? s_data.cur.is_day : 1));

    int fx = (int)(((d.min_c - wk_min) / range) * track_w);
    int fw = (int)(((d.max_c - d.min_c) / range) * track_w);
    if (fw < 6) fw = 6;
    if (fx + fw > track_w) fx = track_w - fw;
    if (fx < 0) fx = 0;
    lv_obj_set_pos(s_d_fill[i], fx, 0);
    lv_obj_set_size(s_d_fill[i], fw, 6);
    lv_obj_set_style_bg_color(s_d_fill[i], temp_color(d.min_c), 0);
    lv_obj_set_style_bg_grad_color(s_d_fill[i], temp_color(d.max_c), 0);
  }
}

static void update_hourly() {
  if (!main_built() || !s_has_data) return;
  const LocalizedStrings* s = S();
  for (int i = 0; i < FORECAST_ROWS; i++) {
    const WeatherHour& h = s_data.hourly[i];
    char b[24];
    if (i == 0 && g_settings.lang != LANG_FR) {
      lv_label_set_text(s_h_hour[i], s->now);
    } else {
      fmt_hour(b, sizeof(b), h.hour);
      lv_label_set_text(s_h_hour[i], b);
    }
    lv_image_set_src(s_h_icon[i], choose_icon(h.code, h.is_day));
    snprintf(b, sizeof(b), LV_SYMBOL_TINT " %d%%", h.precip);
    lv_label_set_text(s_h_rain[i], b);
    lv_obj_set_style_text_opa(s_h_rain[i], h.precip == 0 ? 110 : 255, 0);
    fmt_deg(b, sizeof(b), h.temp_c);
    lv_label_set_text(s_h_temp[i], b);
  }
}

static void refresh_all() {
  if (!main_built()) return;
  if (s_has_data) apply_theme(s_data.cur.code, s_data.cur.is_day);
  update_hero();
  update_daily();
  update_hourly();
}

// ------------------------------------------------------------------ abas
static void style_tabs() {
  for (int i = 0; i < 2; i++) {
    bool active = (i == s_tab);
    lv_obj_set_style_bg_color(s_tab_btn[i], lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(s_tab_btn[i], active ? 235 : 48, 0);
    lv_obj_set_style_text_color(s_tab_lbl[i], lv_color_hex(active ? COL_INK : COL_WHITE), 0);
  }
}

void ui_show_tab(int tab) {
  if (!main_built()) return;
  s_tab = tab ? 1 : 0;
  if (s_tab == 0) {
    lv_obj_remove_flag(s_card[0], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_card[1], LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_remove_flag(s_card[1], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_card[0], LV_OBJ_FLAG_HIDDEN);
  }
  style_tabs();
}

static void tab_clicked_cb(lv_event_t* e) { ui_show_tab((int)(intptr_t)lv_event_get_user_data(e)); }
static void card_clicked_cb(lv_event_t* e) { ui_show_tab(s_tab ? 0 : 1); }

// ------------------------------------------------------------------ tela principal
static void screen_clicked_cb(lv_event_t* e) { ui_open_settings(); }
static void gear_clicked_cb(lv_event_t* e) { ui_open_settings(); }

static void close_overlays() {
  if (s_modal) { lv_obj_delete(s_modal); s_modal = nullptr; }
  if (s_location) { lv_obj_delete(s_location); s_location = nullptr; }
  if (s_settings) { lv_obj_delete(s_settings); s_settings = nullptr; }
}

static void build_row_card_common(lv_obj_t* card) {
  // faixa de destaque na primeira linha
  lv_obj_t* hl = plain_obj(card);
  lv_obj_set_size(hl, 220, 22);
  lv_obj_set_pos(hl, 3, 3);
  lv_obj_set_style_radius(hl, 8, 0);
  lv_obj_set_style_bg_color(hl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(hl, 34, 0);
  lv_obj_remove_flag(hl, LV_OBJ_FLAG_CLICKABLE);
}

void ui_init() {
  close_overlays();
  lv_obj_t* scr = lv_screen_active();
  lv_obj_clean(scr);
  lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_event_cb(scr, screen_clicked_cb);
  lv_obj_add_event_cb(scr, screen_clicked_cb, LV_EVENT_CLICKED, NULL);
  style_bg(scr, 0x2A6FD0, 0x5FAEEB);

  const LocalizedStrings* s = S();

  // ---- cabecalho
  s_lbl_city = make_fixed_label(scr, F14, COL_WHITE, 10, 5, 122, LV_TEXT_ALIGN_LEFT);
  char city[LABEL_LEN + 8];
  header_city_text(city, sizeof(city));
  lv_label_set_text(s_lbl_city, city);

  s_lbl_status = make_label(scr, F14, COL_WARN);
  lv_label_set_text(s_lbl_status, LV_SYMBOL_WARNING);
  lv_obj_set_pos(s_lbl_status, 134, 5);
  lv_obj_add_flag(s_lbl_status, LV_OBJ_FLAG_HIDDEN);

  s_lbl_clock = make_fixed_label(scr, F14, COL_SOFT, 152, 5, 56, LV_TEXT_ALIGN_RIGHT);

  s_btn_gear = plain_obj(scr);
  lv_obj_set_size(s_btn_gear, 30, 28);
  lv_obj_set_pos(s_btn_gear, 210, 0);
  lv_obj_add_flag(s_btn_gear, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(s_btn_gear, gear_clicked_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t* gear = make_label(s_btn_gear, F16, COL_WHITE);
  lv_label_set_text(gear, LV_SYMBOL_SETTINGS);
  lv_obj_set_style_text_opa(gear, 220, 0);
  lv_obj_center(gear);

  // ---- heroi
  s_img_hero = lv_image_create(scr);
  lv_image_set_src(s_img_hero, &image_partly_cloudy);
  lv_obj_set_pos(s_img_hero, 4, 24);

  s_lbl_temp = make_label(scr, FTEMP, COL_WHITE);
  lv_label_set_text(s_lbl_temp, "--°");
  lv_obj_set_pos(s_lbl_temp, 112, 30);

  s_lbl_unit = make_label(scr, F20, COL_SOFT);
  lv_label_set_text(s_lbl_unit, g_settings.fahrenheit ? "F" : "C");

  s_lbl_cond = make_fixed_label(scr, F16, COL_WHITE, 112, 76, 124, LV_TEXT_ALIGN_LEFT);
  s_lbl_feels = make_fixed_label(scr, F12, COL_SOFT, 112, 98, 124, LV_TEXT_ALIGN_LEFT);

  // ---- abas
  lv_obj_t* tabs = plain_obj(scr);
  lv_obj_set_size(tabs, LV_SIZE_CONTENT, 24);
  lv_obj_set_pos(tabs, 10, 128);
  lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_column(tabs, 6, 0);
  const char* tab_text[2] = {s->tab_hourly, s->tab_daily};
  for (int i = 0; i < 2; i++) {
    s_tab_btn[i] = plain_obj(tabs);
    lv_obj_set_size(s_tab_btn[i], LV_SIZE_CONTENT, 24);
    lv_obj_set_style_pad_hor(s_tab_btn[i], 12, 0);
    lv_obj_set_style_radius(s_tab_btn[i], 12, 0);
    lv_obj_add_flag(s_tab_btn[i], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s_tab_btn[i], tab_clicked_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    s_tab_lbl[i] = make_label(s_tab_btn[i], F12, COL_WHITE);
    lv_label_set_text(s_tab_lbl[i], tab_text[i]);
    lv_obj_center(s_tab_lbl[i]);
  }
  s_lbl_hilo = make_fixed_label(scr, F12, COL_SOFT, 138, 132, 98, LV_TEXT_ALIGN_RIGHT);

  // ---- cartao: previsao por hora (0) e 7 dias (1)
  for (int c = 0; c < 2; c++) {
    s_card[c] = make_card(scr, 6, 156, 228, 160);
    lv_obj_add_event_cb(s_card[c], card_clicked_cb, LV_EVENT_CLICKED, NULL);
    build_row_card_common(s_card[c]);
  }

  for (int i = 0; i < FORECAST_ROWS; i++) {
    int y = 5 + i * 22;
    // 7 dias
    s_d_day[i] = make_fixed_label(s_card[1], F14, COL_WHITE, 12, y + 1, 52, LV_TEXT_ALIGN_LEFT);
    s_d_icon[i] = lv_image_create(s_card[1]);
    lv_image_set_src(s_d_icon[i], &icon_partly_cloudy);
    lv_obj_set_pos(s_d_icon[i], 66, y + 1);
    s_d_min[i] = make_fixed_label(s_card[1], F14, COL_DIM, 92, y + 1, 36, LV_TEXT_ALIGN_RIGHT);
    s_d_track[i] = plain_obj(s_card[1]);
    lv_obj_set_size(s_d_track[i], 52, 6);
    lv_obj_set_pos(s_d_track[i], 134, y + 8);
    lv_obj_set_style_radius(s_d_track[i], 3, 0);
    lv_obj_set_style_bg_color(s_d_track[i], lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(s_d_track[i], 60, 0);
    s_d_fill[i] = plain_obj(s_d_track[i]);
    lv_obj_set_style_radius(s_d_fill[i], 3, 0);
    lv_obj_set_style_bg_opa(s_d_fill[i], LV_OPA_COVER, 0);
    lv_obj_set_style_bg_grad_dir(s_d_fill[i], LV_GRAD_DIR_HOR, 0);
    lv_obj_set_size(s_d_fill[i], 30, 6);
    s_d_max[i] = make_fixed_label(s_card[1], F14, COL_WHITE, 192, y + 1, 34, LV_TEXT_ALIGN_LEFT);
    lv_obj_remove_flag(s_d_track[i], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(s_d_fill[i], LV_OBJ_FLAG_CLICKABLE);

    // por hora
    s_h_hour[i] = make_fixed_label(s_card[0], F14, COL_WHITE, 12, y + 1, 56, LV_TEXT_ALIGN_LEFT);
    s_h_icon[i] = lv_image_create(s_card[0]);
    lv_image_set_src(s_h_icon[i], &icon_partly_cloudy);
    lv_obj_set_pos(s_h_icon[i], 74, y + 1);
    s_h_rain[i] = make_fixed_label(s_card[0], F14, COL_RAIN, 106, y + 1, 64, LV_TEXT_ALIGN_LEFT);
    s_h_temp[i] = make_fixed_label(s_card[0], F16, COL_WHITE, 160, y, 56, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_pos(s_h_temp[i], 160, y);
  }

  s_tab = 1;
  ui_show_tab(1);
  refresh_all();
}

void ui_update_weather(const WeatherData& data) {
  s_data = data;
  s_has_data = data.valid;
  refresh_all();
}

void ui_update_clock(bool valid, int hour, int minute) {
  if (!main_built()) return;
  if (!valid) {
    lv_label_set_text(s_lbl_clock, "");
    return;
  }
  char buf[16];
  const LocalizedStrings* s = S();
  if (g_settings.h24) {
    snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
  } else {
    int h = hour % 12;
    if (h == 0) h = 12;
    snprintf(buf, sizeof(buf), "%d:%02d%s", h, minute, hour < 12 ? s->am : s->pm);
  }
  lv_label_set_text(s_lbl_clock, buf);
}

void ui_set_location(const char* name) {
  if (name != g_settings.location) snprintf(g_settings.location, sizeof(g_settings.location), "%s", name);
  if (!main_built()) return;
  char city[LABEL_LEN + 8];
  header_city_text(city, sizeof(city));
  lv_label_set_text(s_lbl_city, city);
}

void ui_set_status(bool ok) {
  if (!main_built()) return;
  if (ok) lv_obj_add_flag(s_lbl_status, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_remove_flag(s_lbl_status, LV_OBJ_FLAG_HIDDEN);
}

// ------------------------------------------------------------------ tela de configuracao do Wi-Fi
void ui_show_wifi_setup() {
  close_overlays();
  lv_obj_t* scr = lv_screen_active();
  lv_obj_clean(scr);
  lv_obj_remove_event_cb(scr, screen_clicked_cb);
  lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
  style_bg(scr, 0x1B4F9C, 0x5FAEEB);
  s_lbl_temp = nullptr;  // a tela principal nao existe mais

  const LocalizedStrings* s = S();

  lv_obj_t* col = plain_obj(scr);
  lv_obj_set_size(col, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_pos(col, 0, 0);
  lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(col, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_top(col, 16, 0);
  lv_obj_set_style_pad_row(col, 10, 0);

  lv_obj_t* ring = plain_obj(col);
  lv_obj_set_size(ring, 76, 76);
  lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(ring, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(ring, 46, 0);
  lv_obj_set_style_border_width(ring, 2, 0);
  lv_obj_set_style_border_color(ring, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(ring, 120, 0);
  lv_obj_t* wifi = make_label(ring, FSYMBOL_BIG, COL_WHITE);
  lv_label_set_text(wifi, LV_SYMBOL_WIFI);
  lv_obj_center(wifi);

  lv_obj_t* title = make_label(col, F20, COL_WHITE);
  lv_label_set_text(title, s->wifi_title);

  lv_obj_t* txt = make_label(col, F14, COL_SOFT);
  lv_label_set_long_mode(txt, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(txt, 214);
  lv_obj_set_style_text_align(txt, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(txt, s->wifi_connect);

  lv_obj_t* pill = plain_obj(col);
  lv_obj_set_size(pill, 150, 38);
  lv_obj_set_style_radius(pill, 19, 0);
  lv_obj_set_style_bg_color(pill, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(pill, LV_OPA_COVER, 0);
  lv_obj_t* ssid = make_label(pill, F20, COL_INK);
  lv_label_set_text(ssid, DEFAULT_CAPTIVE_SSID);
  lv_obj_center(ssid);

  lv_obj_t* hint = make_label(col, F12, COL_SOFT);
  lv_label_set_long_mode(hint, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(hint, 214);
  lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(hint, s->wifi_open);
}

// ------------------------------------------------------------------ configuracoes: estilo dos controles
static lv_obj_t* make_overlay(uint32_t top, uint32_t bottom) {
  lv_obj_t* o = plain_obj(lv_layer_top());
  lv_obj_set_size(o, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_pos(o, 0, 0);
  style_bg(o, top, bottom);
  lv_obj_add_flag(o, LV_OBJ_FLAG_CLICKABLE);
  return o;
}

static void style_switch(lv_obj_t* sw) {
  lv_obj_set_size(sw, 42, 24);
  lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(sw, 56, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sw, lv_color_hex(COL_GOOD), LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_KNOB);
  lv_obj_set_style_outline_width(sw, 0, 0);
  lv_obj_set_style_shadow_width(sw, 0, 0);
}

static lv_obj_t* settings_row(lv_obj_t* parent, const char* text) {
  lv_obj_t* row = plain_obj(parent);
  lv_obj_set_size(row, 224, 36);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_border_width(row, 1, 0);
  lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
  lv_obj_set_style_border_color(row, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(row, 28, 0);
  lv_obj_set_style_pad_column(row, 8, 0);
  lv_obj_t* l = make_label(row, F14, COL_WHITE);
  lv_label_set_text(l, text);
  lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
  lv_obj_set_height(l, lv_font_get_line_height(F14));
  lv_obj_set_flex_grow(l, 1);
  return row;
}

// ------------------------------------------------------------------ configuracoes: eventos
static void settings_close_cb(lv_event_t* e) {
  app_save_settings();
  if (s_settings) { lv_obj_delete_async(s_settings); s_settings = nullptr; }
}

static void brightness_cb(lv_event_t* e) {
  lv_obj_t* sl = (lv_obj_t*)lv_event_get_target(e);
  int v = lv_slider_get_value(sl);
  g_settings.brightness = (uint8_t)v;
  app_set_brightness((uint8_t)v);
  if (lv_event_get_code(e) == LV_EVENT_RELEASED) app_save_settings();
}

static void night_cb(lv_event_t* e) {
  g_settings.night_mode = lv_obj_has_state((lv_obj_t*)lv_event_get_target(e), LV_STATE_CHECKED);
  app_save_settings();
}

static void fahrenheit_cb(lv_event_t* e) {
  g_settings.fahrenheit = lv_obj_has_state((lv_obj_t*)lv_event_get_target(e), LV_STATE_CHECKED);
  app_save_settings();
  refresh_all();
}

static void h24_cb(lv_event_t* e) {
  g_settings.h24 = lv_obj_has_state((lv_obj_t*)lv_event_get_target(e), LV_STATE_CHECKED);
  app_save_settings();
  refresh_all();
}

static void reopen_settings_async(void*) {
  ui_init();
  ui_open_settings();
}

static void language_cb(lv_event_t* e) {
  uint32_t sel = lv_dropdown_get_selected((lv_obj_t*)lv_event_get_target(e));
  if (sel >= LANG_COUNT) sel = LANG_EN;
  g_settings.lang = (Language)sel;
  app_save_settings();
  lv_async_call(reopen_settings_async, NULL);  // reconstroi tudo no novo idioma
}

// ------------------------------------------------------------------ confirmacao de reset do Wi-Fi
static void modal_no_cb(lv_event_t* e) {
  if (s_modal) { lv_obj_delete_async(s_modal); s_modal = nullptr; }
}

static void modal_yes_cb(lv_event_t* e) { app_reset_wifi(); }

static void show_reset_modal(lv_event_t* e) {
  if (s_modal) return;
  const LocalizedStrings* s = S();
  s_modal = plain_obj(lv_layer_top());
  lv_obj_set_size(s_modal, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_color(s_modal, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(s_modal, 150, 0);
  lv_obj_add_flag(s_modal, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t* dlg = plain_obj(s_modal);
  lv_obj_set_width(dlg, 212);
  lv_obj_set_height(dlg, LV_SIZE_CONTENT);
  lv_obj_center(dlg);
  lv_obj_set_style_radius(dlg, 16, 0);
  lv_obj_set_style_bg_color(dlg, lv_color_hex(0x1B2B47), 0);
  lv_obj_set_style_bg_opa(dlg, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(dlg, 1, 0);
  lv_obj_set_style_border_color(dlg, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(dlg, 40, 0);
  lv_obj_set_style_pad_all(dlg, 14, 0);
  lv_obj_set_flex_flow(dlg, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_row(dlg, 10, 0);

  lv_obj_t* title = make_label(dlg, F16, COL_WHITE);
  lv_label_set_text(title, s->reset_title);

  lv_obj_t* body = make_label(dlg, F12, COL_SOFT);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(body, LV_PCT(100));
  lv_label_set_text(body, s->reset_confirmation);

  lv_obj_t* row = plain_obj(dlg);
  lv_obj_set_size(row, LV_PCT(100), 34);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_t* no = make_button(row, s->cancel, 88, 34, 0xFFFFFF, 40, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_t* yes = make_button(row, s->reset, 88, 34, 0xE5484D, 255, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_add_event_cb(no, modal_no_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(yes, modal_yes_cb, LV_EVENT_CLICKED, NULL);
}

// ------------------------------------------------------------------ busca de local
static char s_results[MAX_SEARCH_RESULTS][LABEL_LEN];
static int s_result_count = 0;
static int s_selected = -1;
static lv_obj_t* s_loc_ta;
static lv_obj_t* s_loc_kb;
static lv_obj_t* s_loc_list;
static lv_obj_t* s_loc_msg;
static lv_obj_t* s_loc_save;

static void location_close_cb(lv_event_t* e) {
  if (s_location) { lv_obj_delete_async(s_location); s_location = nullptr; }
}

static void location_save_cb(lv_event_t* e) {
  if (s_selected < 0) return;
  char label[LABEL_LEN];
  if (app_select_location(s_selected, label, sizeof(label))) {
    ui_set_location(label);
    if (s_settings) {
      // atualiza o texto do botao "Local" nas configuracoes
      lv_obj_t* btn = (lv_obj_t*)lv_obj_get_user_data(s_settings);
      if (btn && lv_obj_is_valid(btn)) lv_label_set_text(button_label(btn), g_settings.location);
    }
  }
  if (s_location) { lv_obj_delete_async(s_location); s_location = nullptr; }
}

static void result_clicked_cb(lv_event_t* e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  s_selected = idx;
  for (uint32_t i = 0; i < lv_obj_get_child_count(s_loc_list); i++) {
    lv_obj_t* it = lv_obj_get_child(s_loc_list, i);
    if ((int)i == idx) lv_obj_add_state(it, LV_STATE_CHECKED);
    else lv_obj_remove_state(it, LV_STATE_CHECKED);
  }
  lv_obj_remove_state(s_loc_save, LV_STATE_DISABLED);
  lv_obj_set_style_bg_opa(s_loc_save, LV_OPA_COVER, 0);
}

static void do_search() {
  const char* q = lv_textarea_get_text(s_loc_ta);
  if (!q || strlen(q) == 0) return;
  const LocalizedStrings* s = S();
  lv_obj_clean(s_loc_list);
  s_selected = -1;
  lv_obj_add_state(s_loc_save, LV_STATE_DISABLED);
  lv_obj_set_style_bg_opa(s_loc_save, 70, 0);
  lv_label_set_text(s_loc_msg, s->searching);
  lv_obj_remove_flag(s_loc_msg, LV_OBJ_FLAG_HIDDEN);
  lv_refr_now(NULL);  // mostra "Buscando..." antes da chamada bloqueante

  s_result_count = app_search_location(q, s_results, MAX_SEARCH_RESULTS);
  if (s_result_count <= 0) {
    lv_label_set_text(s_loc_msg, s->no_results);
    return;
  }
  lv_obj_add_flag(s_loc_msg, LV_OBJ_FLAG_HIDDEN);
  for (int i = 0; i < s_result_count; i++) {
    lv_obj_t* it = plain_obj(s_loc_list);
    lv_obj_set_size(it, 224, 34);
    lv_obj_add_flag(it, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(it, 10, 0);
    lv_obj_set_style_bg_color(it, lv_color_hex(COL_ACCENT), LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(it, 255, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(it, lv_color_hex(0xFFFFFF), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(it, 40, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(it, 1, 0);
    lv_obj_set_style_border_side(it, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(it, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(it, 24, 0);
    lv_obj_add_event_cb(it, result_clicked_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    lv_obj_t* l = make_label(it, F14, COL_WHITE);
    lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
    lv_obj_set_size(l, 206, lv_font_get_line_height(F14));
    lv_label_set_text(l, s_results[i]);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, 8, 0);
  }
}

static void kb_cb(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_READY) {
    lv_obj_add_flag(s_loc_kb, LV_OBJ_FLAG_HIDDEN);
    do_search();
  } else if (code == LV_EVENT_CANCEL) {
    lv_obj_add_flag(s_loc_kb, LV_OBJ_FLAG_HIDDEN);
  }
}

static void search_btn_cb(lv_event_t* e) {
  lv_obj_add_flag(s_loc_kb, LV_OBJ_FLAG_HIDDEN);
  do_search();
}

static void ta_clicked_cb(lv_event_t* e) { lv_obj_remove_flag(s_loc_kb, LV_OBJ_FLAG_HIDDEN); }

static void open_location(lv_event_t* e) {
  if (s_location) return;
  const LocalizedStrings* s = S();
  s_location = make_overlay(0x0E1B33, 0x1C3257);
  s_result_count = 0;
  s_selected = -1;

  lv_obj_t* title = make_label(s_location, F20, COL_WHITE);
  lv_label_set_text(title, s->change_location);
  lv_obj_set_pos(title, 14, 8);

  lv_obj_t* x = make_button(s_location, LV_SYMBOL_CLOSE, 30, 30, 0xFFFFFF, 36, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_set_pos(x, 202, 5);
  lv_obj_add_event_cb(x, location_close_cb, LV_EVENT_CLICKED, NULL);

  s_loc_ta = lv_textarea_create(s_location);
  lv_textarea_set_one_line(s_loc_ta, true);
  lv_textarea_set_max_length(s_loc_ta, 40);
  lv_textarea_set_placeholder_text(s_loc_ta, s->city_placeholder);
  lv_obj_set_scrollbar_mode(s_loc_ta, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(s_loc_ta, 140, 36);
  lv_obj_set_pos(s_loc_ta, 8, 46);
  lv_obj_set_style_text_font(s_loc_ta, F14, 0);
  lv_obj_set_style_text_color(s_loc_ta, lv_color_hex(COL_WHITE), 0);
  lv_obj_set_style_text_color(s_loc_ta, lv_color_hex(0x9DB4D3), LV_PART_TEXTAREA_PLACEHOLDER);
  lv_obj_set_style_bg_color(s_loc_ta, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(s_loc_ta, 30, 0);
  lv_obj_set_style_border_width(s_loc_ta, 1, 0);
  lv_obj_set_style_border_color(s_loc_ta, lv_color_hex(COL_ACCENT), LV_STATE_FOCUSED);
  lv_obj_set_style_border_color(s_loc_ta, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(s_loc_ta, 60, 0);
  lv_obj_set_style_radius(s_loc_ta, 12, 0);
  lv_obj_set_style_pad_hor(s_loc_ta, 10, 0);
  lv_obj_set_style_pad_ver(s_loc_ta, 8, 0);
  lv_obj_set_style_bg_color(s_loc_ta, lv_color_hex(COL_ACCENT), LV_PART_CURSOR);
  lv_obj_add_state(s_loc_ta, LV_STATE_FOCUSED);
  lv_obj_add_event_cb(s_loc_ta, ta_clicked_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t* sb = make_button(s_location, s->search, 76, 36, COL_ACCENT, 255, COL_INK, COL_WHITE, 0, F14);
  lv_obj_set_pos(sb, 156, 46);
  lv_obj_add_event_cb(sb, search_btn_cb, LV_EVENT_CLICKED, NULL);

  s_loc_list = plain_obj(s_location);
  lv_obj_set_size(s_loc_list, 224, 168);
  lv_obj_set_pos(s_loc_list, 8, 92);
  lv_obj_set_flex_flow(s_loc_list, LV_FLEX_FLOW_COLUMN);
  lv_obj_add_flag(s_loc_list, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scroll_dir(s_loc_list, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(s_loc_list, LV_SCROLLBAR_MODE_OFF);

  s_loc_msg = make_label(s_location, F14, COL_DIM);
  lv_obj_set_width(s_loc_msg, 224);
  lv_obj_set_style_text_align(s_loc_msg, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(s_loc_msg, 8, 120);
  lv_obj_add_flag(s_loc_msg, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* cancel = make_button(s_location, s->cancel, 108, 38, 0xFFFFFF, 36, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_set_pos(cancel, 8, 272);
  lv_obj_add_event_cb(cancel, location_close_cb, LV_EVENT_CLICKED, NULL);

  s_loc_save = make_button(s_location, s->save, 108, 38, COL_GOOD, 255, COL_INK, COL_WHITE, 0, F14);
  lv_obj_set_pos(s_loc_save, 124, 272);
  lv_obj_add_state(s_loc_save, LV_STATE_DISABLED);
  lv_obj_set_style_bg_opa(s_loc_save, 70, 0);
  lv_obj_add_event_cb(s_loc_save, location_save_cb, LV_EVENT_CLICKED, NULL);

  s_loc_kb = lv_keyboard_create(s_location);
  lv_keyboard_set_mode(s_loc_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
  lv_keyboard_set_textarea(s_loc_kb, s_loc_ta);
  lv_obj_set_style_bg_color(s_loc_kb, lv_color_hex(0x0B1730), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(s_loc_kb, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(s_loc_kb, lv_color_hex(0x2A3F66), LV_PART_ITEMS);
  lv_obj_set_style_bg_color(s_loc_kb, lv_color_hex(0x3C5A91), LV_PART_ITEMS | LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(s_loc_kb, lv_color_hex(COL_ACCENT), LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_text_color(s_loc_kb, lv_color_hex(COL_WHITE), LV_PART_ITEMS);
  lv_obj_set_style_text_color(s_loc_kb, lv_color_hex(COL_INK), LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_border_width(s_loc_kb, 0, LV_PART_ITEMS);
  lv_obj_set_style_shadow_width(s_loc_kb, 0, LV_PART_ITEMS);
  lv_obj_set_style_radius(s_loc_kb, 6, LV_PART_ITEMS);
  lv_obj_add_event_cb(s_loc_kb, kb_cb, LV_EVENT_ALL, NULL);
}

// ------------------------------------------------------------------ configuracoes
bool ui_settings_open() { return s_settings != nullptr; }

void ui_open_settings() {
  if (s_settings) return;
  const LocalizedStrings* s = S();
  s_settings = make_overlay(0x0E1B33, 0x1C3257);

  lv_obj_t* title = make_label(s_settings, F20, COL_WHITE);
  lv_label_set_text(title, s->settings_title);
  lv_obj_set_pos(title, 14, 8);

  lv_obj_t* x = make_button(s_settings, LV_SYMBOL_CLOSE, 30, 30, 0xFFFFFF, 36, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_set_pos(x, 202, 5);
  lv_obj_add_event_cb(x, settings_close_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t* list = plain_obj(s_settings);
  lv_obj_set_size(list, 224, 220);
  lv_obj_set_pos(list, 8, 44);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

  // brilho
  lv_obj_t* row = settings_row(list, s->brightness);
  lv_obj_t* sl = lv_slider_create(row);
  lv_slider_set_range(sl, 5, 255);
  lv_slider_set_value(sl, g_settings.brightness < 5 ? 5 : g_settings.brightness, LV_ANIM_OFF);
  lv_obj_set_size(sl, 104, 8);
  lv_obj_set_style_bg_color(sl, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(sl, 56, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sl, lv_color_hex(COL_ACCENT), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(sl, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(sl, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(sl, LV_OPA_COVER, LV_PART_KNOB);
  lv_obj_set_style_pad_all(sl, 4, LV_PART_KNOB);
  lv_obj_add_event_cb(sl, brightness_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(sl, brightness_cb, LV_EVENT_RELEASED, NULL);

  // apagar a tela a noite
  row = settings_row(list, s->night_mode);
  lv_obj_t* sw = lv_switch_create(row);
  style_switch(sw);
  if (g_settings.night_mode) lv_obj_add_state(sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw, night_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // Fahrenheit
  row = settings_row(list, s->use_fahrenheit);
  sw = lv_switch_create(row);
  style_switch(sw);
  if (g_settings.fahrenheit) lv_obj_add_state(sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw, fahrenheit_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // relogio 24h
  row = settings_row(list, s->use_24hr);
  sw = lv_switch_create(row);
  style_switch(sw);
  if (g_settings.h24) lv_obj_add_state(sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw, h24_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // idioma
  row = settings_row(list, s->language_label);
  lv_obj_t* dd = lv_dropdown_create(row);
  lv_dropdown_set_options(dd, LANGUAGE_OPTIONS);
  lv_dropdown_set_selected(dd, (uint32_t)g_settings.lang);
  lv_obj_set_size(dd, 120, 28);
  lv_obj_set_style_text_font(dd, F14, 0);
  lv_obj_set_style_text_color(dd, lv_color_hex(COL_WHITE), 0);
  lv_obj_set_style_bg_color(dd, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(dd, 36, 0);
  lv_obj_set_style_border_width(dd, 0, 0);
  lv_obj_set_style_radius(dd, 10, 0);
  lv_obj_set_style_pad_ver(dd, 4, 0);
  lv_obj_set_style_pad_hor(dd, 10, 0);
  lv_obj_set_style_shadow_width(dd, 0, 0);
  lv_obj_t* ddl = lv_dropdown_get_list(dd);
  lv_obj_set_style_text_font(ddl, F14, 0);
  lv_obj_set_style_text_color(ddl, lv_color_hex(COL_WHITE), 0);
  lv_obj_set_style_bg_color(ddl, lv_color_hex(0x203760), 0);
  lv_obj_set_style_bg_opa(ddl, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(ddl, 1, 0);
  lv_obj_set_style_border_color(ddl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_opa(ddl, 50, 0);
  lv_obj_set_style_radius(ddl, 10, 0);
  lv_obj_set_style_bg_color(ddl, lv_color_hex(COL_ACCENT), LV_PART_SELECTED | LV_STATE_CHECKED);
  lv_obj_set_style_text_color(ddl, lv_color_hex(COL_INK), LV_PART_SELECTED | LV_STATE_CHECKED);
  lv_obj_add_event_cb(dd, language_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // local
  row = settings_row(list, s->location_label);
  lv_obj_t* loc = make_button(row, g_settings.location, 124, 28, 0xFFFFFF, 36, COL_WHITE, COL_WHITE, 0, F14);
  lv_obj_add_event_cb(loc, open_location, LV_EVENT_CLICKED, NULL);
  lv_obj_set_user_data(s_settings, loc);

  // redefinir Wi-Fi
  lv_obj_t* reset = make_button(s_settings, s->reset_wifi, 224, 36, 0xE5484D, 50, 0xFF9EA1, 0xE5484D, 200, F14);
  lv_obj_set_pos(reset, 8, 270);
  lv_obj_add_event_cb(reset, show_reset_modal, LV_EVENT_CLICKED, NULL);

  lv_obj_t* ver = make_label(s_settings, F12, COL_DIM);
  lv_label_set_text(ver, "Aura " AURA_FW_VERSION);
  lv_obj_set_style_text_opa(ver, 150, 0);
  lv_obj_align(ver, LV_ALIGN_BOTTOM_MID, 0, -2);
}
