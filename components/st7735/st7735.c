#include "st7735.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Beispiel-Pins, anpassen an deine Hardware */
#define PIN_NUM_MOSI  (10)
#define PIN_NUM_CLK   (9)
#define PIN_NUM_CS    (8)
#define PIN_NUM_DC    (7)
#define PIN_NUM_RST   (6)
#define PIN_NUM_BCKL  (5)

/* SPI-Gerätehandle */
static spi_device_handle_t spi;
static const int SPI_CLOCK_HZ = 40 * 1000 * 1000;

/* Callback, um DC-Pin zu setzen (Kommando=0, Daten=1) */
static void IRAM_ATTR spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

/* Sendet ein einzelnes Kommando (8 Bit) */
static esp_err_t st7735_send_cmd(uint8_t cmd)
{
    spi_transaction_t t = {0};
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // DC=0
    return spi_device_polling_transmit(spi, &t);
}

/* Sendet len Bytes an Daten */
static esp_err_t st7735_send_data(const uint8_t *data, int len)
{
    if(len == 0) return ESP_OK;
    spi_transaction_t t = {0};
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // DC=1
    return spi_device_polling_transmit(spi, &t);
}

/* Hardware-Reset über RST-Pin */
static void st7735_reset(void)
{
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

/* Display init (SPI-Bus, Reset, Befehle) */
void st7735_init(void)
{
    /* GPIO config (DC, RST, CS, Backlight) */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ( (1ULL << PIN_NUM_DC) |
                          (1ULL << PIN_NUM_RST) |
                          (1ULL << PIN_NUM_CS)  |
                          (1ULL << PIN_NUM_BCKL) )
    };
    gpio_config(&io_conf);

    /* Backlight an */
    gpio_set_level(PIN_NUM_BCKL, 1);

    /* SPI-Bus init */
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,   // kein MISO
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ST7735_HOR_RES * ST7735_VER_RES * 2 + 8
    };
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK) {
        // Fehlerbehandlung
    }

    /* SPI-Gerät konfigurieren */
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLOCK_HZ,
        .mode = 0, // Mode 0
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = spi_pre_transfer_callback,
    };
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    if(ret != ESP_OK) {
        // Fehlerbehandlung
    }

    /* Display resetten */
    st7735_reset();

    /* Init-Sequenz */
    st7735_send_cmd(0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(150));

    st7735_send_cmd(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(150));

    uint8_t pix_fmt = 0x05; // 16-Bit (RGB565)
    st7735_send_cmd(0x3A);
    st7735_send_data(&pix_fmt, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Memory Data Access Control */
    uint8_t madctl = 0x00;
    st7735_send_cmd(0x36);
    st7735_send_data(&madctl, 1);

    /* Display ON */
    st7735_send_cmd(0x29);
    vTaskDelay(pdMS_TO_TICKS(100));
}

/* Flush-Callback für LVGL 9 (Parameter color_p = uint8_t *) */
void st7735_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    /* 1) Column Address Set */
    uint8_t data_col[4] = {
        (uint8_t)(area->x1 >> 8), (uint8_t)(area->x1 & 0xFF),
        (uint8_t)(area->x2 >> 8), (uint8_t)(area->x2 & 0xFF)
    };
    st7735_send_cmd(0x2A);
    st7735_send_data(data_col, 4);

    /* 2) Row Address Set */
    uint8_t data_row[4] = {
        (uint8_t)(area->y1 >> 8), (uint8_t)(area->y1 & 0xFF),
        (uint8_t)(area->y2 >> 8), (uint8_t)(area->y2 & 0xFF)
    };
    st7735_send_cmd(0x2B);
    st7735_send_data(data_row, 4);

    /* 3) Write RAM */
    st7735_send_cmd(0x2C);

    /* Pixel-Daten senden */
    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    st7735_send_data(color_p, size * 2); // 16 Bit pro Pixel => size * 2 Bytes

    /* LVGL signalisieren, dass der Flush fertig ist */
    lv_display_flush_ready(disp);
}
