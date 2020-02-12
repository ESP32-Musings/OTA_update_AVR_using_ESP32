idf_component_register(SRCS "avr_pro_mode.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event freertos logger 
                       esp_http_server esp_wifi nvs_flash spiffs)
