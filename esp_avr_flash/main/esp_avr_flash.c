#include "hex_parser.h"
#include "avr_flash.h"

static const char *TAG = "esp_avr_flash";

uint8_t page[PAGE_SIZE_MAX];
int block_count = 0;

void flashTask(void)
{
    //Hard coding the file name to be flashed
    char *filepath = "/spiffs/blink.hex"; 

    logI(TAG, "%s", "Writing file to page");
    ESP_ERROR_CHECK(hexFileParser(filepath, page, &block_count));

    logI(TAG, "%s", "Writing code to AVR memory");
    ESP_ERROR_CHECK(writeTask(page, block_count));

    logI(TAG, "%s", "Reading Memory");
    ESP_ERROR_CHECK(readTask(page, block_count));

    logI(TAG, "%s", "Ending Connection");
    endConn();
}

void initTask(void)
{
    initUART();
    initGPIO();
    initSPIFFS();
    setupDevice();
}

void app_main(void)
{
    initTask();
    flashTask();
}
