#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"   
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "driver/uart.h"
#include "esp_netif.h"
#include "rom/ets_sys.h"
#include "esp_smartconfig.h"
#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "externVars.h"
#include "calls.h"

static const char *TAG = "sound";

// void StartPlaySoundRoutine(void)
// {
//     char payload[100];
//     if(fileToBeRun <=1)
//     {
//     sprintf(payload,"Not Playing Sound fileToBeRun %d",fileToBeRun);
     
//     uart_write_string_ln(payload);
//     return;
//     }
//      current_time_us = esp_timer_get_time(); 
//      last_audio_ms = current_time_us / 1000;
//     sprintf(payload,"Playing Sound fileToBeRun %d",fileToBeRun);
//     uart_write_string_ln(payload);

// 	uint16_t CheckSum = 0;
// 	uint8_t i = 0;
//     payload[i] = 0x7e;
// 	i++;
// 	CheckSum = CheckSum+0xff;
//     payload[i] = 0xff;
// 	i++;
	
// 	CheckSum = CheckSum+0x06;
//     payload[i] = 0x06;
//     i++;
	
// 	CheckSum = CheckSum+0x03;
//     payload[i] = 0x03;
//     i++;
	
// 	CheckSum = CheckSum+0x00;
//     payload[i] = 0x00;
//     i++;
	
// 	CheckSum = CheckSum+0x00;
//     payload[i] = 0x00;
//     i++;
	
// 	CheckSum = CheckSum+fileToBeRun;
//     payload[i] = fileToBeRun & 0xff;
//     i++;
	
// 	CheckSum ^= 0xffff;
// 	CheckSum++;
	
//     payload[i] = CheckSum/0x100;
//     i++;
//     payload[i] = CheckSum%0x100;
//     i++;

//     payload[i] = 0xef;
//     i++;
//     payload[i] = 0x00;
//     i++;

//     uart_write_bytes(EX_UART_NUM, payload, 11);



 
// }

void StartPlaySoundRoutine(void)
{
    char log_payload[50]; // Separate buffer for logs
    uint8_t payload[11];  // Fixed-size payload for UART
    uint16_t CheckSum = 0;
    uint8_t i = 0;

    if (fileToBeRun < 1)
    {
        sprintf(log_payload, "Not Playing Sound fileToBeRun %d", fileToBeRun);
        uart_write_string_ln(log_payload);
        return;
    }

    current_time_us = esp_timer_get_time();
    last_audio_ms = current_time_us / 1000;
    
    sprintf(log_payload, "Playing Sound fileToBeRun %d", fileToBeRun);
    uart_write_string_ln(log_payload);

    // Constructing the command payload
    payload[i++] = 0x7E;      // Start byte
    payload[i++] = 0xFF;      // Version
    payload[i++] = 0x06;      // Command Length
    payload[i++] = 0x03;      // Command: Play
    payload[i++] = 0x00;      // Feedback
    payload[i++] = 0x00;      // Parameter High Byte
    payload[i++] = fileToBeRun;    // Parameter Low Byte (Track number or similar)

    // Compute checksum
    CheckSum = 0xFF + 0x06 + 0x03 + 0x00 + 0x00 + fileToBeRun;
    CheckSum = 0xFFFF - CheckSum + 1;

    payload[i++] = (CheckSum >> 8) & 0xFF; // High byte of checksum
    payload[i++] = CheckSum & 0xFF;        // Low byte of checksum
    payload[i++] = 0xEF;                   // End byte

    // Send the payload over UART (11 bytes total)
    uart_write_bytes(EX_UART_NUM, (const char*)payload, 11);
}


void PlayInLoop (void)
{
    while (1)
    {
     current_time_us = esp_timer_get_time(); 
     elapsed_ms = current_time_us / 1000;
        if ((fileToBeRun != ImagePowerOkay) && ((elapsed_ms-last_audio_ms)>15000))
        {
            StartPlaySoundRoutine();
        }
        vTaskDelay(20000/portTICK_PERIOD_MS);  
    }
}