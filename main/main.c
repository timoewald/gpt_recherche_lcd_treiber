#include "lvgl.h"
#include "st7735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Task zur periodischen Ausführung von lv_timer_handler() */
static void lvgl_task(void *pvParameter) {
    while(1) {
        lv_timer_handler();  // Verarbeitet LVGL-Timer und Animationen
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void app_main(void) {
    /* LVGL initialisieren */
    lv_init();

    /* ST7735 Display initialisieren (SPI, Reset, Initialisierungssequenz) */
    st7735_init();

    /* LVGL Display-Puffer einrichten */
    static lv_color_t buf1[160 * 10];
    static lv_color_t buf2[160 * 10];
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, 160 * 10);

    /* LVGL Display-Treiber konfigurieren */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = 160;
    disp_drv.ver_res = 128;
    disp_drv.flush_cb = st7735_flush;
    lv_disp_drv_register(&disp_drv);

    /* Starte einen Task, der lv_timer_handler() regelmäßig aufruft */
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 1, NULL);

    /* Erstelle ein einfaches Label auf dem aktiven Screen */
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, ST7735!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /* Die Hauptschleife kann für weitere Aufgaben genutzt werden.
       Die GUI wird über den separaten LVGL-Task aktualisiert. */
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
