#ifndef ST7735_H
#define ST7735_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Display Auflösung */
#define ST7735_HOR_RES 160
#define ST7735_VER_RES 128

/**
 * @brief Initialisiert das ST7735 Display.
 *
 * Konfiguriert den SPI-Bus, richtet die nötigen GPIOs ein, führt einen Hardware-Reset durch
 * und sendet die Initialisierungssequenz an das Display.
 */
void st7735_init(void);

/**
 * @brief Flush Callback für LVGL.
 *
 * Diese Funktion wird von LVGL aufgerufen, um einen definierten Pixelbereich
 * (definiert durch *area*) aus einem Buffer (*color_p*) per SPI an das Display zu übertragen.
 * Am Ende muss mit lv_disp_flush_ready(disp_drv) signalisiert werden, dass der Transfer abgeschlossen ist.
 *
 * @param disp_drv Pointer auf den LVGL Display-Treiber.
 * @param area Pointer auf die Struktur, die den zu aktualisierenden Bereich beschreibt.
 * @param color_p Pointer auf den Buffer mit den zu sendenden Pixeln (im RGB565-Format).
 */
void st7735_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#ifdef __cplusplus
}
#endif

#endif /* ST7735_H */
