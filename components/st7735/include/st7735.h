#ifndef ST7735_H
#define ST7735_H

#include "lvgl.h"  /* Damit lv_display_t, lv_area_t, lv_color_t etc. bekannt sind */

#ifdef __cplusplus
extern "C" {
#endif

/* Display-Auflösung */
#define ST7735_HOR_RES  160
#define ST7735_VER_RES  128

/**
 * @brief  Initialisiert den ST7735 (SPI, Reset, Init-Sequenz).
 */
void st7735_init(void);

/**
 * @brief  Flush-Callback für LVGL 9.
 *
 * In LVGL 9 hat das Flush-CB die Signatur:
 *   void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
 */
void st7735_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);

#ifdef __cplusplus
}
#endif

#endif /* ST7735_H */
