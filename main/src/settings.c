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


void IncrementParameterValue (void)
{
    if (SettingButtonPressed == SettingGameMode)
    {
        GameMode++;
        GameModeChanged = 1;
       
        if (GameMode > MaximumGameMode)
            GameMode = 0;
        
        DisplayGameMode();   
    }
    else if (SettingButtonPressed ==SettingSoundMode )
    {
        SoundMode++;
        SoundModeChanged =1;
        if(SoundMode > MaximumSoundMode)
            SoundMode=0;
        DisplaySoundMode();
    }
    else if (SettingButtonPressed == SettingPlayTime)
    {
        PlayTime++;
        PlayTimeModeChanged=1;
        if(PlayTime > MaximumPlayTime)
           PlayTime=0;
        DisplayTMode();
    }
    else if (SettingButtonPressed == SettingLightTime)
    {
        Mode2LightTime ++;
        LightTimeModeChanged=1;
        if(Mode2LightTime > MaximumLightTime)
        {
           Mode2LightTime=1; 
        } 
        DisplayLightTime();
    }
    else if(SettingButtonPressed==SettingNumberOfLights)
    {
        NumberOfLights++;
        LightSettingModeChanged=1;
        if(NumberOfLights >MaximumNumberOfLights)
        {
            NumberOfLights=3;
        }
        DisplayMaxLights();
    }

}