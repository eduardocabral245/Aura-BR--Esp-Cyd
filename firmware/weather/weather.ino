// Aura - display de previsao do tempo para ESP32-2432S028R (CYD, tela ILI9341 2,8").
// Versao em portugues do Brasil, com interface redesenhada.
//
// Projeto original: https://github.com/Surrey-Homeware/Aura (GPL 3.0)
// Este arquivo cuida do aparelho (Wi-Fi, rede, memoria, toque, brilho).
// A interface grafica esta em ui.cpp; os textos, em translations.h.
#include <Arduino.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include "esp_system.h"
#include "app_api.h"

#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS
#define LCD_BACKLIGHT_PIN 21
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

// Local padrao na primeira vez que o aparelho liga (pode ser trocado nas configuracoes)
#define LATITUDE_DEFAULT "-23.5505"
#define LONGITUDE_DEFAULT "-46.6333"
#define LOCATION_DEFAULT "São Paulo, São Paulo"

#define UPDATE_INTERVAL 600000UL  // atualiza a cada 10 minutos
#define RETRY_INTERVAL 30000UL    // se falhar, tenta de novo em 30 segundos

// Modo noturno: a tela apaga das 22h as 6h
#define NIGHT_MODE_START_HOUR 22
#define NIGHT_MODE_END_HOUR 6

// Codigo de idioma da API de busca de cidades (mesma ordem do enum Language)
static const char* const LANGUAGE_CODES[LANG_COUNT] = {"en", "es", "de", "fr", "tr", "sv", "it", "pt"};

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
static uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// ------------------------------------------------------------------ estado
AuraSettings g_settings;
static Preferences prefs;
static char latitude[16] = LATITUDE_DEFAULT;
static char longitude[16] = LONGITUDE_DEFAULT;
static WeatherData weather;
static JsonDocument geoDoc;
static JsonArray geoResults;

static bool night_mode_active = false;
static bool temp_screen_wakeup_active = false;
static lv_timer_t* temp_screen_wakeup_timer = nullptr;
static uint32_t next_fetch_at = 0;

// ------------------------------------------------------------------ funcoes auxiliares
static uint32_t tick_millis() { return millis(); }

static int day_of_week(int y, int m, int d) {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static String urlencode(const String& str) {
  String encoded = "";
  char buf[5];
  for (size_t i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    // caracteres nao reservados (RFC 3986)
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' ||
        c == '.' || c == '~') {
      encoded += c;
    } else {
      sprintf(buf, "%%%02X", (unsigned char)c);
      encoded += buf;
    }
  }
  return encoded;
}

// ------------------------------------------------------------------ configuracoes salvas
static void load_settings() {
  String lat = prefs.getString("latitude", LATITUDE_DEFAULT);
  lat.toCharArray(latitude, sizeof(latitude));
  String lon = prefs.getString("longitude", LONGITUDE_DEFAULT);
  lon.toCharArray(longitude, sizeof(longitude));
  String loc = prefs.getString("location", LOCATION_DEFAULT);
  loc.toCharArray(g_settings.location, sizeof(g_settings.location));

  g_settings.fahrenheit = prefs.getBool("useFahrenheit", false);
  g_settings.h24 = prefs.getBool("use24Hour", true);
  g_settings.night_mode = prefs.getBool("useNightMode", false);
  uint32_t b = prefs.getUInt("brightness", 255);
  if (b < 1) b = 1;
  if (b > 255) b = 255;
  g_settings.brightness = (uint8_t)b;
  uint32_t lang = prefs.getUInt("language", LANG_PT);
  g_settings.lang = (lang < LANG_COUNT) ? (Language)lang : LANG_PT;
}

void app_save_settings() {
  prefs.putBool("useFahrenheit", g_settings.fahrenheit);
  prefs.putBool("use24Hour", g_settings.h24);
  prefs.putBool("useNightMode", g_settings.night_mode);
  prefs.putUInt("brightness", g_settings.brightness);
  prefs.putUInt("language", g_settings.lang);
  prefs.putString("location", g_settings.location);
}

void app_set_brightness(uint8_t value) { analogWrite(LCD_BACKLIGHT_PIN, value); }

void app_reset_wifi() {
  Serial.println("Apagando credenciais Wi-Fi e reiniciando");
  WiFiManager wm;
  wm.resetSettings();
  delay(100);
  esp_restart();
}

// ------------------------------------------------------------------ modo noturno (tela apagada)
static bool night_mode_should_be_active() {
  if (!g_settings.night_mode) return false;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return false;
  int hour = timeinfo.tm_hour;
  return (hour >= NIGHT_MODE_START_HOUR || hour < NIGHT_MODE_END_HOUR);
}

static void activate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, 0);
  night_mode_active = true;
}

static void deactivate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, g_settings.brightness);
  night_mode_active = false;
}

static void check_for_night_mode() {
  bool night_time = night_mode_should_be_active();
  if (night_time && !night_mode_active && !temp_screen_wakeup_active) {
    activate_night_mode();
  } else if (!night_time && night_mode_active) {
    deactivate_night_mode();
  }
}

static void handle_temp_screen_wakeup_timeout(lv_timer_t* timer) {
  if (temp_screen_wakeup_active) {
    temp_screen_wakeup_active = false;
    if (night_mode_should_be_active()) activate_night_mode();
  }
  if (temp_screen_wakeup_timer) {
    lv_timer_delete(temp_screen_wakeup_timer);
    temp_screen_wakeup_timer = nullptr;
  }
}

// ------------------------------------------------------------------ relogio
static void update_clock(lv_timer_t* timer) {
  check_for_night_mode();
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 0)) {
    ui_update_clock(false, 0, 0);
    return;
  }
  ui_update_clock(true, timeinfo.tm_hour, timeinfo.tm_min);
}

// ------------------------------------------------------------------ toque
static void touchscreen_read(lv_indev_t* indev, lv_indev_data_t* data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);

    // Toque com a tela apagada pela noite: acende por 15 segundos
    if (night_mode_active) {
      analogWrite(LCD_BACKLIGHT_PIN, g_settings.brightness);
      if (temp_screen_wakeup_timer) lv_timer_delete(temp_screen_wakeup_timer);
      temp_screen_wakeup_timer = lv_timer_create(handle_temp_screen_wakeup_timeout, 15000, NULL);
      lv_timer_set_repeat_count(temp_screen_wakeup_timer, 1);
      Serial.println("Tela acesa. Apaga de novo apos 15 segundos sem toque.");

      if (!temp_screen_wakeup_active) {
        // o primeiro toque so acende a tela, nao aciona a interface
        temp_screen_wakeup_active = true;
        data->state = LV_INDEV_STATE_RELEASED;
        return;
      }
      temp_screen_wakeup_active = true;
    }

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ------------------------------------------------------------------ Wi-Fi
static void pump_ui(uint32_t ms) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    lv_timer_handler();
    delay(5);
  }
}

// Chamado pelo WiFiManager quando abre a rede "Aura" para configuracao
static void ap_mode_callback(WiFiManager* mgr) {
  ui_show_wifi_setup();
  pump_ui(200);
}

static bool ensure_wifi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.println("Wi-Fi desconectado. Tentando reconectar...");
  WiFi.reconnect();
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
    lv_timer_handler();
    delay(250);
  }
  return WiFi.status() == WL_CONNECTED;
}

// ------------------------------------------------------------------ busca de cidades
int app_search_location(const char* query, char labels[][LABEL_LEN], int max_results) {
  if (!ensure_wifi()) return 0;
  geoDoc.clear();
  geoResults = JsonArray();

  String url = String("https://geocoding-api.open-meteo.com/v1/search?name=") + urlencode(query) +
               "&count=" + String(max_results) + "&language=" + LANGUAGE_CODES[g_settings.lang] + "&format=json";

  int found = 0;
  HTTPClient http;
  http.begin(url);
  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Busca de local concluida: " + url);
    DeserializationError err = deserializeJson(geoDoc, http.getString());
    if (!err) {
      geoResults = geoDoc["results"].as<JsonArray>();
      for (JsonObject item : geoResults) {
        if (found >= max_results) break;
        String label = item["name"].as<String>();
        if (item["admin1"]) {
          label += ", ";
          label += item["admin1"].as<String>();
        }
        if (item["country_code"]) {
          label += ", ";
          label += item["country_code"].as<String>();
        }
        label.toCharArray(labels[found], LABEL_LEN);
        found++;
      }
    } else {
      Serial.println("Falha ao ler a resposta da busca: " + url);
    }
  } else {
    Serial.println("Falha na busca de local: " + url);
  }
  http.end();
  return found;
}

static bool fetch_and_update_weather();

bool app_select_location(int idx, char* out_label, size_t out_len) {
  if (idx < 0 || idx >= (int)geoResults.size()) return false;
  JsonObject obj = geoResults[idx];
  double lat = obj["latitude"].as<double>();
  double lon = obj["longitude"].as<double>();
  snprintf(latitude, sizeof(latitude), "%.6f", lat);
  snprintf(longitude, sizeof(longitude), "%.6f", lon);
  prefs.putString("latitude", latitude);
  prefs.putString("longitude", longitude);

  String label = obj["name"].as<String>();
  if (obj["admin1"]) {
    label += ", ";
    label += obj["admin1"].as<String>();
  }
  if (obj["country_code"]) {
    label += ", ";
    label += obj["country_code"].as<String>();
  }
  label.toCharArray(out_label, out_len);
  snprintf(g_settings.location, sizeof(g_settings.location), "%s", out_label);
  prefs.putString("location", g_settings.location);

  next_fetch_at = 0;  // busca a previsao do novo local ja no proximo ciclo
  return true;
}

// ------------------------------------------------------------------ previsao do tempo
static bool fetch_and_update_weather() {
  if (!ensure_wifi()) {
    Serial.println("Sem Wi-Fi. Nova tentativa em breve.");
    ui_set_status(false);
    return false;
  }

  String url = String("http://api.open-meteo.com/v1/forecast?latitude=") + latitude + "&longitude=" + longitude +
               "&current=temperature_2m,apparent_temperature,is_day,weather_code" +
               "&daily=temperature_2m_min,temperature_2m_max,weather_code" +
               "&hourly=temperature_2m,precipitation_probability,is_day,weather_code" + "&forecast_hours=7" +
               "&timezone=auto";

  HTTPClient http;
  http.begin(url);
  bool ok = false;

  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Previsao atualizada (open-meteo): " + url);
    JsonDocument doc;
    if (deserializeJson(doc, http.getString()) == DeserializationError::Ok) {
      JsonArray times = doc["daily"]["time"].as<JsonArray>();
      JsonArray tmin = doc["daily"]["temperature_2m_min"].as<JsonArray>();
      JsonArray tmax = doc["daily"]["temperature_2m_max"].as<JsonArray>();
      JsonArray dcodes = doc["daily"]["weather_code"].as<JsonArray>();
      JsonArray hours = doc["hourly"]["time"].as<JsonArray>();
      JsonArray htemps = doc["hourly"]["temperature_2m"].as<JsonArray>();
      JsonArray hprec = doc["hourly"]["precipitation_probability"].as<JsonArray>();
      JsonArray hcodes = doc["hourly"]["weather_code"].as<JsonArray>();
      JsonArray hday = doc["hourly"]["is_day"].as<JsonArray>();

      if (times.size() >= FORECAST_ROWS && hours.size() >= FORECAST_ROWS) {
        memset(&weather, 0, sizeof(weather));
        weather.cur.temp_c = doc["current"]["temperature_2m"].as<float>();
        weather.cur.feels_c = doc["current"]["apparent_temperature"].as<float>();
        weather.cur.code = doc["current"]["weather_code"].as<int>();
        weather.cur.is_day = doc["current"]["is_day"].as<int>();

        for (int i = 0; i < FORECAST_ROWS; i++) {
          const char* date = times[i];  // "AAAA-MM-DD"
          weather.daily[i].dow = day_of_week(atoi(date + 0), atoi(date + 5), atoi(date + 8));
          weather.daily[i].min_c = tmin[i].as<float>();
          weather.daily[i].max_c = tmax[i].as<float>();
          weather.daily[i].code = dcodes[i].as<int>();

          const char* hour_iso = hours[i];  // "AAAA-MM-DDTHH:MM"
          weather.hourly[i].hour = atoi(hour_iso + 11);
          weather.hourly[i].temp_c = htemps[i].as<float>();
          weather.hourly[i].precip = hprec[i].as<int>();
          weather.hourly[i].code = hcodes[i].as<int>();
          weather.hourly[i].is_day = hday[i].as<int>();
        }
        weather.valid = true;

        int utc_offset_seconds = doc["utc_offset_seconds"].as<int>();
        configTime(utc_offset_seconds, 0, "pool.ntp.org", "time.nist.gov");
        Serial.print("Hora atualizada via NTP com deslocamento UTC: ");
        Serial.println(utc_offset_seconds);

        ui_update_weather(weather);
        ok = true;
      } else {
        Serial.println("Resposta incompleta de " + url);
      }
    } else {
      Serial.println("Falha ao ler o JSON de " + url);
    }
  } else {
    Serial.println("Falha no HTTP GET em " + url);
  }
  http.end();
  ui_set_status(ok);
  return ok;
}

// ------------------------------------------------------------------ setup / loop
void setup() {
  Serial.begin(115200);
  delay(100);

  TFT_eSPI tft = TFT_eSPI();
  tft.init();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  lv_init();
  lv_tick_set_cb(tick_millis);

  // Touch
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  lv_display_t* disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Configuracoes salvas
  prefs.begin("weather", false);
  load_settings();
  analogWrite(LCD_BACKLIGHT_PIN, g_settings.brightness);

  // Mostra a tela ja no inicio (enquanto conecta ao Wi-Fi)
  ui_init();
  pump_ui(100);

  // Conecta ao Wi-Fi; se nao houver rede salva, abre o portal de configuracao "Aura"
  WiFiManager wm;
  wm.setAPCallback(ap_mode_callback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  ui_init();
  fetch_and_update_weather();
  next_fetch_at = millis() + (weather.valid ? UPDATE_INTERVAL : RETRY_INTERVAL);
}

void loop() {
  lv_timer_handler();

  if ((int32_t)(millis() - next_fetch_at) >= 0) {
    bool ok = fetch_and_update_weather();
    next_fetch_at = millis() + (ok ? UPDATE_INTERVAL : RETRY_INTERVAL);
  }

  delay(5);
}
