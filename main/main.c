#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "st7735.h"

/* Ein Task, der periodisch lv_timer_handler() aufruft */
static void lvgl_task(void *pv)
{
    while(1) {
        lv_timer_handler();        // intern: GUI verarbeiten
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void app_main(void)
{
    /* 1) LVGL init */
    lv_init();

    /* 2) Display init (SPI) */
    st7735_init();

    /* 3) lv_draw_buf_t anlegen (z.B. 10 Zeilen, doppelt gepuffert) */
    static lv_draw_buf_t draw_buf;
    static lv_color_t buf1[ST7735_HOR_RES * 10];
    static lv_color_t buf2[ST7735_HOR_RES * 10];

    /*
     * In LVGL 9 ist die Signatur:
     * lv_draw_buf_init(&draw_buf, w, h, color_format, stride, buf1, buf2);
     * => w = 160, h = 10, color_format = LV_COLOR_FORMAT_RGB565, stride=0
     */
    lv_draw_buf_init(&draw_buf,
                     ST7735_HOR_RES,       /* w */
                     10,                   /* h = Teilpuffer */
                     LV_COLOR_FORMAT_RGB565,
                     0,                    /* stride = 0 => Standard */
                     buf1,
                     buf2);

    /* 4) Ein Display in LVGL erzeugen */
    lv_display_t * disp = lv_display_create(ST7735_HOR_RES, ST7735_VER_RES);

    /* 5) Den Zeichnungspuffer und Flush-Callback einbinden */
    lv_display_set_draw_buffers(disp, &draw_buf, NULL);
    lv_display_set_flush_cb(disp, st7735_flush);

    /*
     * Standardmäßig erstellt LVGL auch schon einen Screen,
     * man kann ihn abrufen oder selbst erstellen. Hier Beispiel:
     */
    lv_obj_t * screen = lv_obj_create(NULL);

    /* 6) Einfaches Label auf dem Screen platzieren */
    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "Hello, ST7735 + LVGL9!");
    lv_obj_center(label);

    /* 7) Einen Task starten, der LVGL periodisch anstößt */
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 1, NULL);

    /* 8) Hauptschleife (falls du weitere Logik hast) */
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
