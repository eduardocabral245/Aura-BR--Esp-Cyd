#!/bin/bash
# Regenera as fontes do Aura (LVGL) a partir da Montserrat.
# Requisitos: Node.js e `npm install lv_font_conv`, mais os TTFs da Montserrat
# (https://github.com/JulietaUla/Montserrat -> fonts/ttf).
#
# Uso: ./gerar_fontes.sh <pasta com Montserrat-*.ttf e node_modules> <pasta do sketch (weather/)>
set -e
T="${1:?pasta das ferramentas (TTFs + node_modules)}"
OUT="${2:?pasta do sketch}"
CONV="node $T/node_modules/lv_font_conv/lv_font_conv.js"

gen() { # nome tamanho ttf fallback [opcoes...]
  name=$1; size=$2; ttf=$3; fb=$4; shift 4
  args=(--bpp 4 --size "$size" --no-compress --font "$T/$ttf" "$@" --format lvgl --lv-font-name "$name" -o "$OUT/$name.c")
  [ -n "$fb" ] && args+=(--lv-fallback "$fb")
  $CONV "${args[@]}"
}

# Latin-1 completo + Latin Estendido-A (cobre portugues, espanhol, frances, alemao,
# italiano, sueco, turco e nomes de cidades europeias) + setas e pontuacao tipografica.
LATIN=(-r 0x20-0x7E -r 0xA0-0xFF -r 0x100-0x17F --symbols "‐–—‘’“”•…→↑↓")

gen lv_font_montserrat_latin_12 12 Montserrat-Medium.ttf   lv_font_montserrat_12 "${LATIN[@]}"
gen lv_font_montserrat_latin_14 14 Montserrat-Medium.ttf   lv_font_montserrat_14 "${LATIN[@]}"
gen lv_font_montserrat_latin_16 16 Montserrat-Medium.ttf   lv_font_montserrat_16 "${LATIN[@]}"
gen lv_font_montserrat_latin_20 20 Montserrat-SemiBold.ttf lv_font_montserrat_20 "${LATIN[@]}"
# Temperatura grande: so digitos, grau e sinal de menos
gen lv_font_montserrat_temp_56 56 Montserrat-Light.ttf "" -r 0x30-0x39 --symbols "°-"

# Ajusta o include para funcionar no Arduino IDE (lvgl.h) e em builds fora do Arduino.
python3 - "$OUT" <<'PY'
import sys, glob, re
old = '#ifdef LV_LVGL_H_INCLUDE_SIMPLE\n#include "lvgl.h"\n#else\n#include "lvgl/lvgl.h"\n#endif'
new = ('#ifdef __has_include\n    #if __has_include("lvgl.h")\n        #ifndef LV_LVGL_H_INCLUDE_SIMPLE\n'
       '            #define LV_LVGL_H_INCLUDE_SIMPLE\n        #endif\n    #endif\n#endif\n\n'
       '#ifdef LV_LVGL_H_INCLUDE_SIMPLE\n    #include "lvgl.h"\n#else\n    #include "lvgl/lvgl.h"\n#endif')
for f in glob.glob(sys.argv[1] + '/lv_font_montserrat_*.c'):
    s = open(f, encoding='utf-8').read()
    s = s.replace(old, new)
    s = re.sub(r'--font \S*/(Montserrat-\w+\.ttf)', r'--font \1', s)
    open(f, 'w', encoding='utf-8').write(s)
PY
echo "fontes geradas em $OUT"
