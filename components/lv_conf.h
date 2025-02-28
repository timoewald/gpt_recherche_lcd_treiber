#ifndef LV_CONF_H
#define LV_CONF_H

/*********************
 *  LVGL 9.x Einstellungen
 *********************/

/* Einfaches Inkludieren von lv_conf.h */
#define LV_CONF_INCLUDE_SIMPLE 1

/* Farbtiefe: 16 Bit (RGB565) */
#define LV_COLOR_DEPTH 16

/* Maximale Auflösung deines Displays */
#define LV_HOR_RES_MAX 160
#define LV_VER_RES_MAX 128

/* Keine LVGL-Beispiele kompilieren (spart Platz, vermeidet Konflikte) */
#define LV_BUILD_EXAMPLES 0

/* Ggf. weitere Einstellungen nach Bedarf:
   - #define LV_USE_PERF_MONITOR 1
   - #define LV_USE_LOG 1
   - etc.
*/

#endif /*LV_CONF_H*/
