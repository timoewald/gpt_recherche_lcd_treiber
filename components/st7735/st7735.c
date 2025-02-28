#include "st7735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

/* Definition der GPIO-Pins (Standardpins, anpassbar falls gewünscht) */
#define PIN_NUM_MOSI  (GPIO_NUM_10)
#define PIN_NUM_CLK   (GPIO_NUM_9)
#define PIN_NUM_CS    (GPIO_NUM_8)
#define PIN_NUM_DC    (GPIO_NUM_7)
#define PIN_NUM_RST   (GPIO_NUM_6)
#define PIN_NUM_BCKL  (GPIO_NUM_5)  // Optional: Backlight

/* SPI-Gerätehandle */
static spi_device_handle_t spi;
static const int SPI_CLOCK_HZ = 40 * 1000 * 1000; // 40 MHz

/* Pre-Transfer Callback: Setzt den DC-Pin basierend auf dem user-Feld des SPI-Transfers.
   (0: Kommando, 1: Daten) */
static void IRAM_ATTR spi_pre_transfer_callback(spi_transaction_t *t) {
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

/* Hilfsfunktion: Sendet ein Kommando (8 Bit) an das Display */
static esp_err_t st7735_send_cmd(const uint8_t cmd) {
    spi_transaction_t t = {0};
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*) 0; // Kommandomodus (DC = 0)
    return spi_device_polling_transmit(spi, &t);
}

/* Hilfsfunktion: Sendet Daten an das Display */
static esp_err_t st7735_send_data(const uint8_t *data, int len) {
    if (len == 0) return ESP_OK;
    spi_transaction_t t = {0};
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*) 1; // Datenmodus (DC = 1)
    return spi_device_polling_transmit(spi, &t);
}

/* Führt einen Hardware-Reset des Displays durch */
static void st7735_reset(void) {
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

/* Initialisiert den SPI-Bus, fügt das Display als SPI-Gerät hinzu und führt die Display-Initialisierung durch */
void st7735_init(void) {
    /* Konfiguration der benötigten GPIOs (DC, RST, CS, Backlight) */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) |
                         (1ULL << PIN_NUM_CS) | (1ULL << PIN_NUM_BCKL))
    };
    gpio_config(&io_conf);
    /* Backlight einschalten (angenommen aktiv high) */
    gpio_set_level(PIN_NUM_BCKL, 1);

    /* SPI-Bus konfigurieren */
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,      // Nicht benötigt
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ST7735_HOR_RES * ST7735_VER_RES * 2 + 8
    };
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        // Fehlerbehandlung falls notwendig
    }
    /* SPI-Gerät konfigurieren */
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLOCK_HZ,
        .mode = 0,                  // SPI Mode 0
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = spi_pre_transfer_callback,
    };
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        // Fehlerbehandlung falls notwendig
    }

    /* Display zurücksetzen */
    st7735_reset();

    /* Initialisierungssequenz */
    st7735_send_cmd(0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(150));

    st7735_send_cmd(0x11); // Exit Sleep Mode
    vTaskDelay(pdMS_TO_TICKS(150));

    uint8_t pix_fmt = 0x05;  // 16-Bit Farbtiefe (RGB565)
    st7735_send_cmd(0x3A);   // Set Pixel Format
    st7735_send_data(&pix_fmt, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Memory Data Access Control (optional, je nach Orientierung) */
    uint8_t madctl = 0x00;
    st7735_send_cmd(0x36);
    st7735_send_data(&madctl, 1);

    /* Display einschalten */
    st7735_send_cmd(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(150));
}

/* LVGL Flush Callback:
   Überträgt einen definierten Bereich des Displays mit Pixeln, die im Buffer *color_p* liegen.
   Nach erfolgreichem Transfer wird lv_disp_flush_ready() aufgerufen. */
void st7735_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    /* Spaltenadresse setzen */
    uint8_t data_col[4];
    data_col[0] = area->x1 >> 8;
    data_col[1] = area->x1 & 0xFF;
    data_col[2] = area->x2 >> 8;
    data_col[3] = area->x2 & 0xFF;
    st7735_send_cmd(0x2A); // Column Address Set
    st7735_send_data(data_col, 4);

    /* Zeilenadresse setzen */
    uint8_t data_row[4];
    data_row[0] = area->y1 >> 8;
    data_row[1] = area->y1 & 0xFF;
    data_row[2] = area->y2 >> 8;
    data_row[3] = area->y2 & 0xFF;
    st7735_send_cmd(0x2B); // Row Address Set
    st7735_send_data(data_row, 4);

    /* Speicher schreiben */
    st7735_send_cmd(0x2C);

    /* Berechne die Anzahl der Pixel im zu aktualisierenden Bereich */
    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    /* Sende Pixel-Daten; lv_color_t wird hier als 16-Bit (RGB565) angenommen */
    st7735_send_data((uint8_t *)color_p, size * 2);

    /* Informiere LVGL, dass der Flush-Vorgang abgeschlossen ist */
    lv_disp_flush_ready(disp_drv);
}
