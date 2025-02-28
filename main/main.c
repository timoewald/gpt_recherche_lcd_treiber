#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "st7735.h"

/* Eine Task, die alle paar Millisekunden LVGL aktualisiert */
static void lvgl_task(void *pvParameter)
{
    while(1) {
        lv_timer_handler();  // interner LVGL-Loop
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void app_main(void)
{
    /* 1) LVGL initialisieren */
    lv_init();

    /* 2) ST7735 (SPI) initialisieren */
    st7735_init();

    /* 3) Zeichnungspuffer anlegen (für 10 Zeilen doppelt gepuffert) */
    static lv_draw_buf_t draw_buf;
    static lv_color_t buf1[ST7735_HOR_RES * 10];
    static lv_color_t buf2[ST7735_HOR_RES * 10];

    /* Neuer Name in LVGL 9: lv_draw_buf_init() statt lv_disp_draw_buf_init() */
    lv_draw_buf_init(&draw_buf, buf1, buf2, ST7735_HOR_RES * 10, LV_COLOR_FORMAT_RGB565);

    /* 4) Ein Displayobjekt in LVGL erzeugen (statt lv_disp_drv_register) */
    lv_display_t * disp = lv_display_create(ST7735_HOR_RES, ST7735_VER_RES);

    /* 5) Display-Puffer und Flush-Callback zuweisen */
    lv_display_set_draw_buf(disp, &draw_buf);
    lv_display_set_flush_cb(disp, st7735_flush);

    /* 6) Optionale Einstellungen: Rotation, Farbformat, etc. 
       // lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
       // lv_display_set_color_depth(disp, 16);
    */

    /* 7) Einfaches Label erstellen, um zu prüfen, ob es klappt */
    lv_obj_t * label = lv_label_create(lv_display_get_scr_act(disp));
    lv_label_set_text(label, "Hello, ST7735!");
    lv_obj_center(label);

    /* 8) Separaten Task für LVGL starten */
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 1, NULL);

    /* 9) Haupt-Loop kann nun andere Aufgaben übernehmen */
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
