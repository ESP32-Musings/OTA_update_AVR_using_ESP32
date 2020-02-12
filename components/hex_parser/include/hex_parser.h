#ifndef _HEX_PARSER_H
#define _HEX_PARSER_H

#include "avr_pro_mode.h"

/**
 * @brief Parse the data from a hex Record
 *   For e.g. =>
 *   :100000000C9434000C9446000C9446000C9446006A
 *   : => Record start
 *   10 => Data length (16 bytes for most records)
 *   0000 => Start Address
 *   00 => Record Type (00 = Data Record)
 *   0C 94 34 00 0C 94 46 00 0C 94 46 00 0C 94 46 00 => Data; to be extracted
 *   6A => Checksum
 * 
 * @param buff Raw hex Record
 * @param data To store the parsed result
 * @param start Starting index of Data in Record
 * @param end Ending index of Data in Record  
 * 
 * @return data : parsed data from raw hex Record
 */
char *extractData(char buff[], char data[], int start, int end);

/**
 * @brief Parse an entire .hex file for data
 * 
 * It gives out a 'page' of data, containing all the Data parsed from each of the hex Records 
 * from the .hex file
 * 
 * @param filepath the .hex file to be parsed
 * @param page To store the parsed result
 * @param block_count Total no. of blocks (128 bytes each)
 *   
 * @return ESP_OK - success, ESP_FAIL - failed
 */
esp_err_t hexFileParser(char *filepath, uint8_t page[], int *block_count);

#endif
