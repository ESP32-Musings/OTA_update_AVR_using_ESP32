#include "hex_parser.h"

//Functions for custom adjustments

static const char *TAG_HEX_PARSER = "hex_parser";

char *extractData(char hex_buff[], char data[], int start, int end)
{
    int c = 0;
    int size = (end - start) + 1;

    while (c < size)
    {
        data[c] = hex_buff[start + c];
        c++;
    }
    data[c] = '\0';

    return data;
}

esp_err_t hexFileParser(char *filepath, uint8_t page[], int *block_count)
{
    int idx = 0;

    logD(TAG_HEX_PARSER, "Reading file: %s", filepath);
    FILE *f = fopen(filepath, "r");
    if (f == NULL)
    {
        logE(TAG_HEX_PARSER, "%s",  "Failed to open file for reading");
        return ESP_FAIL;
    }

    while (1)
    {
        char curr_line[64], prev_line[64];

        fgets(curr_line, sizeof(curr_line), f);
        char *pos = strchr(curr_line, '\n');

        if (strcmp(prev_line, curr_line) == 0)
        {
            break;
        }

        if (pos)
        {
            *pos = '\0';
        }

        char buff[strlen(curr_line) - 1];
        strcpy(buff, curr_line);

        if (strlen(buff) > 12)
        {
            int start = 9;
            int end = strlen(buff) - 4;
            int size = (end - start) + 1;

            char raw_data[size];
            extractData(buff, raw_data, start, end);

            char byte_buff[3];
            byte_buff[2] = '\0';
            for (int i = 0; i < strlen(raw_data) - 1; i += 2)
            {
                byte_buff[0] = raw_data[i];
                byte_buff[1] = raw_data[i + 1];
                page[idx] = strtol(byte_buff, 0, 16);
                idx++;
            }
        }

        strcpy(prev_line, curr_line);
    }

    while (idx % 128 != 0)
    {
        page[idx] = 0xff;
        idx++;
    }

    //ESP_LOG_BUFFER_HEXDUMP("Page: ", page, sizeof(page), ESP_LOG_DEBUG);
    *block_count = (idx - 1) / 128;
    logD(TAG_HEX_PARSER, "Block count: %d", *block_count);

    return ESP_OK;
}