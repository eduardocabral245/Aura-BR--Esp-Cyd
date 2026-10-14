#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

// Idiomas suportados.
// ATENCAO: a ordem deste enum e a mesma da lista LANGUAGE_OPTIONS (dropdown) e do
// valor salvo na memoria do aparelho. Novos idiomas entram sempre no FINAL.
enum Language {
  LANG_EN = 0,
  LANG_ES = 1,
  LANG_DE = 2,
  LANG_FR = 3,
  LANG_TR = 4,
  LANG_SV = 5,
  LANG_IT = 6,
  LANG_PT = 7,  // Portugues do Brasil
  LANG_COUNT = 8
};

#define LANGUAGE_OPTIONS "English\nEspañol\nDeutsch\nFrançais\nTürkçe\nSvenska\nItaliano\nPortuguês"

#ifndef DEFAULT_CAPTIVE_SSID
#define DEFAULT_CAPTIVE_SSID "Aura"
#endif

// Quantidade de descricoes de condicao do tempo (indices em weather_condition_index()).
#define COND_COUNT 13

struct LocalizedStrings {
  // Tela principal
  const char* feels_like;
  const char* tab_hourly;
  const char* tab_daily;
  const char* today;
  const char* now;
  const char* am;
  const char* pm;
  const char* noon;
  const char* invalid_hour;
  const char* h24_suffix;  // sufixo da hora no modo 24h (ex.: "14h")
  // Configuracoes
  const char* settings_title;
  const char* brightness;
  const char* night_mode;
  const char* use_fahrenheit;
  const char* use_24hr;
  const char* language_label;
  const char* location_label;
  // Busca de local
  const char* change_location;
  const char* city_placeholder;
  const char* search;
  const char* searching;
  const char* no_results;
  const char* save;
  const char* cancel;
  const char* close;
  // Wi-Fi
  const char* reset_wifi;
  const char* reset;
  const char* reset_title;
  const char* reset_confirmation;
  const char* wifi_title;
  const char* wifi_connect;
  const char* wifi_open;
  // Dados
  const char* weekdays[7];  // domingo = 0
  const char* cond[COND_COUNT];
};

// Ordem de cond[]: 0 limpo, 1 quase limpo, 2 parcialmente nublado, 3 nublado,
// 4 neblina, 5 garoa, 6 chuva fraca, 7 chuva, 8 chuva forte, 9 chuva gelada,
// 10 neve, 11 tempestade, 12 pancadas de chuva

static const LocalizedStrings strings_en = {
  "Feels like", "Hourly", "7 days",
  "Today", "Now", "am", "pm", "Noon", "Invalid hour", "",
  "Settings", "Brightness", "Screen off at night", "Use °F", "24-hour clock",
  "Language", "Location",
  "Change location", "e.g. London", "Search", "Searching…", "No results found",
  "Save", "Cancel", "Close",
  "Reset Wi-Fi", "Reset", "Reset Wi-Fi?",
  "Are you sure you want to reset the Wi-Fi credentials?\n\n"
  "You'll need to reconnect to the Wi-Fi network " DEFAULT_CAPTIVE_SSID
  " with your phone or browser to set it up again.",
  "Wi-Fi setup",
  "Connect your phone or laptop to this temporary Wi-Fi network:",
  "A setup page should open by itself. If it doesn't, visit http://192.168.4.1 in your web browser.",
  {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
  {"Clear", "Mostly clear", "Partly cloudy", "Overcast", "Fog", "Drizzle", "Light rain",
   "Rain", "Heavy rain", "Freezing rain", "Snow", "Thunderstorm", "Showers"}
};

static const LocalizedStrings strings_es = {
  "Sensación", "Por horas", "7 días",
  "Hoy", "Ahora", "am", "pm", "Mediodía", "Hora inválida", "",
  "Configuración", "Brillo", "Pantalla off de noche", "Usar °F", "Reloj 24 h",
  "Idioma", "Ubicación",
  "Cambiar ubicación", "ej. Madrid", "Buscar", "Buscando…", "Sin resultados",
  "Guardar", "Cancelar", "Cerrar",
  "Restablecer Wi-Fi", "Restablecer", "¿Restablecer Wi-Fi?",
  "¿Está seguro de que desea restablecer las credenciales Wi-Fi?\n\n"
  "Deberá reconectarse a la red Wi-Fi " DEFAULT_CAPTIVE_SSID
  " con su teléfono o navegador para configurarla de nuevo.",
  "Configuración Wi-Fi",
  "Conecte su teléfono o portátil a esta red Wi-Fi temporal:",
  "Debería abrirse una página de configuración. Si no, visite http://192.168.4.1 en su navegador.",
  {"Dom", "Lun", "Mar", "Mié", "Jue", "Vie", "Sáb"},
  {"Despejado", "Poco nuboso", "Parc. nublado", "Nublado", "Niebla", "Llovizna", "Lluvia débil",
   "Lluvia", "Lluvia fuerte", "Lluvia helada", "Nieve", "Tormenta", "Chubascos"}
};

static const LocalizedStrings strings_de = {
  "Gefühlt", "Stündlich", "7 Tage",
  "Heute", "Jetzt", "", "", "Mittag", "Ungültige Stunde", "",
  "Einstellungen", "Helligkeit", "Display nachts aus", "°F verwenden", "24-Stunden-Uhr",
  "Sprache", "Standort",
  "Standort ändern", "z.B. Berlin", "Suchen", "Suche…", "Keine Ergebnisse",
  "Speichern", "Abbrechen", "Schließen",
  "Wi-Fi zurücksetzen", "Zurücksetzen", "Wi-Fi zurücksetzen?",
  "Möchten Sie die Wi-Fi-Zugangsdaten wirklich zurücksetzen?\n\n"
  "Sie müssen sich erneut mit dem WLAN " DEFAULT_CAPTIVE_SSID
  " verbinden, um es neu einzurichten.",
  "Wi-Fi Konfiguration",
  "Verbinden Sie Ihr Telefon oder Laptop mit diesem temporären WLAN:",
  "Eine Konfigurationsseite sollte sich öffnen. Falls nicht, besuchen Sie http://192.168.4.1 in Ihrem Browser.",
  {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"},
  {"Klar", "Heiter", "Wolkig", "Bedeckt", "Nebel", "Nieselregen", "Leichter Regen",
   "Regen", "Starkregen", "Eisregen", "Schnee", "Gewitter", "Schauer"}
};

static const LocalizedStrings strings_fr = {
  "Ressenti", "Par heure", "7 jours",
  "Aujourd'hui", "Maintenant", "h", "h", "Midi", "Heure invalide", "h",
  "Paramètres", "Luminosité", "Écran éteint la nuit", "Utiliser °F", "Horloge 24 h",
  "Langue", "Lieu",
  "Changer de lieu", "ex. Paris", "Chercher", "Recherche…", "Aucun résultat",
  "Enregistrer", "Annuler", "Fermer",
  "Réinit. Wi-Fi", "Réinitialiser", "Réinitialiser le Wi-Fi ?",
  "Voulez-vous vraiment réinitialiser les identifiants Wi-Fi ?\n\n"
  "Vous devrez vous reconnecter au réseau Wi-Fi " DEFAULT_CAPTIVE_SSID
  " avec votre téléphone ou navigateur pour le reconfigurer.",
  "Configuration Wi-Fi",
  "Connectez votre téléphone ou ordinateur à ce réseau Wi-Fi temporaire :",
  "Une page de configuration devrait s'ouvrir. Sinon, visitez http://192.168.4.1 dans votre navigateur.",
  {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"},
  {"Dégagé", "Peu nuageux", "Éclaircies", "Couvert", "Brouillard", "Bruine", "Pluie faible",
   "Pluie", "Forte pluie", "Verglas", "Neige", "Orage", "Averses"}
};

static const LocalizedStrings strings_tr = {
  "Hissedilen", "Saatlik", "7 Gün",
  "Bugün", "Şimdi", "öö", "ös", "Öğle", "Geçersiz saat", "",
  "Ayarlar", "Parlaklık", "Geceleri ekran kapalı", "°F kullan", "24 saat",
  "Dil", "Konum",
  "Konumu değiştir", "örn. Ankara", "Ara", "Aranıyor…", "Sonuç yok",
  "Kaydet", "İptal", "Kapat",
  "Wi-Fi sıfırla", "Sıfırla", "Wi-Fi sıfırlansın mı?",
  "Wi-Fi bilgilerini sıfırlamak istediğinizden emin misiniz?\n\n"
  "Yeniden yapılandırmak için telefonunuz veya tarayıcınızla " DEFAULT_CAPTIVE_SSID
  " ağına tekrar bağlanmanız gerekecek.",
  "Wi-Fi kurulumu",
  "Telefonunuzu veya bilgisayarınızı bu geçici Wi-Fi ağına bağlayın:",
  "Bir yapılandırma sayfası açılmalı. Açılmazsa tarayıcınızda http://192.168.4.1 adresine gidin.",
  {"Paz", "Pzt", "Sal", "Çar", "Per", "Cum", "Cmt"},
  {"Açık", "Az bulutlu", "Parçalı bulutlu", "Kapalı", "Sis", "Çisenti", "Hafif yağmur",
   "Yağmur", "Şiddetli yağış", "Buzlu yağmur", "Kar", "Fırtına", "Sağanak"}
};

static const LocalizedStrings strings_sv = {
  "Känns som", "Timme", "7 dagar",
  "Idag", "Nu", "", "", "Middag", "Ogiltig timme", "",
  "Inställningar", "Ljusstyrka", "Skärm av på natten", "Använd °F", "24-timmarsklocka",
  "Språk", "Plats",
  "Ändra plats", "t.ex. Stockholm", "Sök", "Söker…", "Inga resultat",
  "Spara", "Avbryt", "Stäng",
  "Återställ Wi-Fi", "Återställ", "Återställa Wi-Fi?",
  "Vill du verkligen återställa Wi-Fi-uppgifterna?\n\n"
  "Du måste ansluta till Wi-Fi-nätverket " DEFAULT_CAPTIVE_SSID
  " med telefon eller webbläsare för att ställa in det igen.",
  "Wi-Fi-konfiguration",
  "Anslut din telefon eller laptop till detta tillfälliga Wi-Fi-nätverk:",
  "En konfigurationssida bör öppnas. Om inte, besök http://192.168.4.1 i din webbläsare.",
  {"Sön", "Mån", "Tis", "Ons", "Tor", "Fre", "Lör"},
  {"Klart", "Mest klart", "Halvklart", "Mulet", "Dimma", "Duggregn", "Lätt regn",
   "Regn", "Kraftigt regn", "Underkylt regn", "Snö", "Åska", "Skurar"}
};

static const LocalizedStrings strings_it = {
  "Percepita", "Orario", "7 giorni",
  "Oggi", "Ora", "am", "pm", "Mezzog.", "Ora non valida", "",
  "Impostazioni", "Luminosità", "Schermo off di notte", "Usa °F", "Orologio 24h",
  "Lingua", "Posizione",
  "Cambia posizione", "es. Roma", "Cerca", "Ricerca…", "Nessun risultato",
  "Salva", "Annulla", "Chiudi",
  "Reset Wi-Fi", "Reset", "Resettare il Wi-Fi?",
  "Sei sicuro di voler ripristinare le credenziali Wi-Fi?\n\n"
  "Dovrai riconnetterti alla rete Wi-Fi " DEFAULT_CAPTIVE_SSID
  " con il telefono o il browser per configurarla di nuovo.",
  "Configurazione Wi-Fi",
  "Collega il tuo smartphone o laptop a questa rete Wi-Fi temporanea:",
  "Dovrebbe aprirsi una pagina di configurazione. Altrimenti visita http://192.168.4.1 nel browser.",
  {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"},
  {"Sereno", "Poco nuvoloso", "Parz. nuvoloso", "Coperto", "Nebbia", "Pioviggine", "Pioggia debole",
   "Pioggia", "Pioggia forte", "Pioggia gelata", "Neve", "Temporale", "Rovesci"}
};

static const LocalizedStrings strings_pt = {
  "Sensação", "Por hora", "7 dias",
  "Hoje", "Agora", "am", "pm", "Meio-dia", "Hora inválida", "h",
  "Configurações", "Brilho", "Apagar tela à noite", "Usar °F", "Relógio 24h",
  "Idioma", "Local",
  "Alterar local", "ex.: Campinas", "Buscar", "Buscando…", "Nenhum resultado",
  "Salvar", "Cancelar", "Fechar",
  "Redefinir Wi-Fi", "Redefinir", "Redefinir Wi-Fi?",
  "Tem certeza de que deseja apagar os dados da rede Wi-Fi?\n\n"
  "Será preciso conectar de novo à rede Wi-Fi " DEFAULT_CAPTIVE_SSID
  " com o celular ou o navegador para configurar outra vez.",
  "Configurar Wi-Fi",
  "Conecte seu celular ou notebook a esta rede Wi-Fi temporária:",
  "Uma página de configuração deve abrir sozinha. Se não abrir, acesse http://192.168.4.1 no navegador.",
  {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"},
  {"Céu limpo", "Poucas nuvens", "Parc. nublado", "Nublado", "Neblina", "Garoa", "Chuva fraca",
   "Chuva", "Chuva forte", "Chuva gelada", "Neve", "Tempestade", "Pancadas"}
};

static const LocalizedStrings* get_strings(Language lang) {
  switch (lang) {
    case LANG_ES: return &strings_es;
    case LANG_DE: return &strings_de;
    case LANG_FR: return &strings_fr;
    case LANG_TR: return &strings_tr;
    case LANG_SV: return &strings_sv;
    case LANG_IT: return &strings_it;
    case LANG_PT: return &strings_pt;
    default: return &strings_en;
  }
}

// Converte o codigo WMO (Open-Meteo) em um indice de cond[].
static inline int weather_condition_index(int code) {
  switch (code) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 45: case 48: return 4;
    case 51: case 53: case 55: case 56: case 57: return 5;
    case 61: return 6;
    case 63: return 7;
    case 65: case 82: return 8;
    case 66: case 67: return 9;
    case 71: case 73: case 75: case 77: case 85: case 86: return 10;
    case 95: case 96: case 99: return 11;
    case 80: case 81: return 12;
    default: return 3;
  }
}

#endif  // TRANSLATIONS_H
