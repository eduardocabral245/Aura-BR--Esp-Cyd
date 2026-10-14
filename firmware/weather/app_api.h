// Interface entre a logica do aparelho (weather.ino: Wi-Fi, rede, memoria, toque)
// e a interface grafica (ui.cpp: so LVGL). Mantida sem dependencias do Arduino
// para que a mesma tela possa ser renderizada no PC (simulador).
#ifndef APP_API_H
#define APP_API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "translations.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define FORECAST_ROWS 7
#define MAX_SEARCH_RESULTS 15
#define LABEL_LEN 72

// ---------------------------------------------------------------- dados do tempo
struct WeatherCurrent {
  float temp_c;
  float feels_c;
  int code;    // codigo WMO
  int is_day;  // 1 dia, 0 noite
};

struct WeatherDay {
  int dow;  // 0 = domingo
  float min_c;
  float max_c;
  int code;
};

struct WeatherHour {
  int hour;  // 0..23
  float temp_c;
  int precip;  // probabilidade de chuva, %
  int code;
  int is_day;
};

struct WeatherData {
  bool valid;
  WeatherCurrent cur;
  WeatherDay daily[FORECAST_ROWS];
  WeatherHour hourly[FORECAST_ROWS];
};

// ---------------------------------------------------------------- configuracoes
struct AuraSettings {
  bool fahrenheit;
  bool h24;
  bool night_mode;
  uint8_t brightness;  // 1..255
  Language lang;
  char location[LABEL_LEN];
};

// Definida em weather.ino (ou no simulador); a UI so le e altera os campos.
extern AuraSettings g_settings;

// ---------------------------------------------------------------- UI -> app
// Chamadas que a UI faz; implementadas em weather.ino (ou stubs no simulador).
void app_save_settings();                        // grava g_settings na memoria
void app_set_brightness(uint8_t value);          // aplica o brilho do backlight agora
void app_reset_wifi();                           // apaga o Wi-Fi e reinicia
// Busca cidades (bloqueante). Preenche labels e devolve a quantidade encontrada.
int app_search_location(const char* query, char labels[][LABEL_LEN], int max_results);
// Confirma o resultado idx da ultima busca; devolve o nome em out_label.
bool app_select_location(int idx, char* out_label, size_t out_len);

// ---------------------------------------------------------------- app -> UI
void ui_init();                                      // (re)cria a tela principal
void ui_show_wifi_setup();                           // tela do modo de configuracao Wi-Fi
void ui_update_weather(const WeatherData& data);     // atualiza os dados exibidos
void ui_update_clock(bool valid, int hour, int minute);
void ui_set_location(const char* name);
void ui_set_status(bool ok);                         // false = mostra aviso de falha na atualizacao
void ui_open_settings();
void ui_show_tab(int tab);                           // 0 = por hora, 1 = 7 dias
bool ui_settings_open();

#endif  // APP_API_H
