/**
 * Following are the Reference links for the code
 * ['esp_avr_programmer' by rene-win](https://github.com/rene-win/esp_avr_programmer)
 * 
 * [STK500 AVR Protocol](http://www.amelek.gda.pl/avr/uisp/doc2525.pdf)
 */

#include "avr_pro_mode.h"

static const char *TAG_AVR_PRO = "avr_pro_mode";

//Functions for custom adjustments
void initUART(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    logI(TAG_AVR_PRO, "%s", "UART initialized");
}

void initGPIO(void)
{
    gpio_set_direction(RESET_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RESET_PIN, HIGH);
    logI(TAG_AVR_PRO, "%s", "GPIO initialized");
}

void initSPIFFS(void)
{
    logI(TAG_AVR_PRO, "%s", "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            logE(TAG_AVR_PRO, "%s", "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            logE(TAG_AVR_PRO, "%s", "Failed to find SPIFFS partition");
        }
        else
        {
            logE(TAG_AVR_PRO, "%s", "Failed to initialize SPIFFS");
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        logE(TAG_AVR_PRO, "%s", "Failed to get SPIFFS partition information");
    }
    else
    {
        logD(TAG_AVR_PRO, "Partition size: total: %d, used: %d", total, used);
    }
}

void resetMCU(void)
{
    logI(TAG_AVR_PRO, "%s", "Starting Reset Procedure");

    gpio_set_level(RESET_PIN, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(RESET_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(RESET_PIN, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(RESET_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    logI(TAG_AVR_PRO, "%s", "Reset Procedure finished");
}

void setupDevice(void)
{
    resetMCU();
    getSync();
    setProgParams();
    setExtProgParams();
    enterProgMode();
}

void endConn(void)
{
    extProgMode();
    resetMCU();
}

int getSync(void)
{
    logI(TAG_AVR_PRO, "%s", "Synchronizing");
    return execCmd(0x30);
}

int setProgParams(void)
{
    char params[] = {0x86, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00};
    logI(TAG_AVR_PRO, "%s", "Setting Prog Parameters");

    return execParam(0x42, params, sizeof(params));
}

int setExtProgParams(void)
{
    char params[] = {0x05, 0x04, 0xd7, 0xc2, 0x00};
    logI(TAG_AVR_PRO, "%s", "Setting ExProg Parameters");
    return execParam(0x45, params, sizeof(params));
}

int enterProgMode(void)
{
    logI(TAG_AVR_PRO, "%s", "Entering Pro Mode");
    return execCmd(0x50);
}

int extProgMode(void)
{
    logI(TAG_AVR_PRO, "%s", "Exiting Pro Mode");
    return execCmd(0x51);
}

int execCmd(char cmd)
{
    char bytes[] = {cmd, 0x20};
    return sendBytes(bytes, 2);
}

int execParam(char cmd, char *params, int count)
{
    char bytes[32];
    bytes[0] = cmd;

    int i = 0;
    while (i < count)
    {
        bytes[i + 1] = params[i];
        i++;
    }

    bytes[i + 1] = 0x20;
    return sendBytes(bytes, i + 2);
}

int sendBytes(char *bytes, int count)
{
    sendData(TAG_AVR_PRO, bytes, count);
    int length = waitForSerialData(MIN_DELAY_MS, MAX_DELAY_MS);

    if (length > 0)
    {
        uint8_t data[length];
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, length, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0)
        {
            if (data[0] == SYNC && data[1] == OK)
            {
                logI(TAG_AVR_PRO, "%s", "Sync Success");
                return 1;
            }
            else
            {
                logE(TAG_AVR_PRO, "%s", "Sync Failure");
                return 0;
            }
        }
    }
    else
    {
        logE(TAG_AVR_PRO, "%s", "Serial Timeout");
    }
    return 0;
}

int waitForSerialData(int dataCount, int timeout)
{
    int timer = 0;
    int length = 0;
    while (timer < timeout)
    {
        uart_get_buffered_data_len(UART_NUM_1, (size_t *)&length);
        if (length >= dataCount)
        {
            return length;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
        timer++;
    }
    return 0;
}

int sendData(const char *logName, const char *data, const int count)
{
    const int txBytes = uart_write_bytes(UART_NUM_1, data, count);
    //ESP_LOG_BUFFER_HEXDUMP(logName, data, count, ESP_LOG_DEBUG);
    return txBytes;
}