#pragma once

#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_CONF_INCLUDE_SIMPLE 1
#define LV_BUILD_EXAMPLES 0   // Keine LVGL-Beispiele WIEDER RAUSNEHMEN DENKE ICH

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *  Grundlegende Einstellungen
 *********************/

/* Farbtiefe: 16 Bit (RGB565) */
#define LV_COLOR_DEPTH     16

/* Auflösung des Displays */
#define LV_HOR_RES_MAX     160
#define LV_VER_RES_MAX     128

/* Aktiviert Betriebssystemunterstützung (z.B. FreeRTOS) */
#define LV_USE_OS          1

/* Größe des Arbeitsspeichers, der LVGL zugewiesen wird */
#define LV_MEM_SIZE        (16 * 1024)

/* Standard-Refresh-Periode in ms */
#define LV_DISP_DEF_REFR_PERIOD 30

/* Vereinfacht das Einbinden von lv_conf.h */
#define LV_CONF_INCLUDE_SIMPLE 1

/*********************
 *  Feature-Einstellungen
 *********************/

/* Aktiviert Animationen */
#define LV_USE_ANIMATION   1

/* Aktiviert Benutzer-Daten in Objekten */
#define LV_USE_USER_DATA   1

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CONF_H*/
