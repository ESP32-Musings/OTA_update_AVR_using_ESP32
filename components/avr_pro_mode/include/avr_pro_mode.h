#ifndef _AVR_PRO_MODE_H
#define _AVR_PRO_MODE_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_event.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#include "esp_wifi.h"
#include "nvs_flash.h"

#include "logger.h"

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
#define RESET_PIN (GPIO_NUM_19)
#define HIGH 1
#define LOW 0

#define SYNC 0x14
#define OK 0x10

#define MIN_DELAY_MS 2
#define MAX_DELAY_MS 1000

#define PAGE_SIZE_MAX 24 * 1024
#define BLOCK_SIZE 128

static const int RX_BUF_SIZE = 1024;

//Initialize UART functionalities
void initUART(void);

//Initialize GPIO functionalities
void initGPIO(void);

//Initialize SPIFFS functionalities
void initSPIFFS(void);

//Reset the client MCU
void resetMCU(void);

//Setup the client MCU for flashing
void setupDevice(void);

//End the connection with client MCU
void endConn(void);

//Get in sync with client MCU
int getSync(void);

//Set the STK500 programming parameters for the communication
int setProgParams(void);

//Set the device programming parameters for the communication
int setExtProgParams(void);

//Enter programming mode for client MCU
int enterProgMode(void);

//Exit programming mode
int extProgMode(void);

//Execute the STK500-defined commands passed
int execCmd(char cmd);

//Set the STK500-defined & client device parameters
int execParam(char cmd, char *params, int count);

//UART send data to client MCU & wait for response
int sendBytes(char *bytes, int count);

//Wait for response from client MCU
int waitForSerialData(int dataCount, int timeout);

//UART send data byte-by-byte to client MCU
int sendData(const char *logName, const char *data, int count);

#endif
