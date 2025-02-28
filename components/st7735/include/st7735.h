#ifndef ST7735_H
#define ST7735_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ST7735_HOR_RES 160
#define ST7735_VER_RES 128

/**
 * @brief Initialisiert das ST7735-Display (SPI, Reset, Init-Sequenz).
 */
void st7735_init(void);

/**
 * @brief Flush-Callback für LVGL 9.0.
 * 
 * LVGL ruft diese Funktion auf, um den in 'color_p' enthaltenen Bildbereich
 * (definiert durch 'area') ans Display zu senden. Anschließend
 * muss lv_display_flush_ready(disp) aufgerufen werden.
 *
 * @param disp    Zeiger auf das Display-Objekt in LVGL
 * @param area    Der darzustellende Bereich (Koordinaten)
 * @param color_p Pointer auf die Pixel-Daten (RGB565)
 */
void st7735_flush(lv_display_t * disp, const lv_area_t * area, lv_color_t * color_p);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* ST7735_H */
