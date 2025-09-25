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

#define HIGH 1
#define LOW 0

#define SegC 0x78
#define SegP 0x638 // c d off
#define SegDash 0x200
// N is c,e,g
#define SegN 0x320
// T is seg d,e,f,g
#define SegT 0x268
// L is seg d,e,f
#define SegL 0x68
// same as 5
#define SegS 0x358
// same as 8 except d
#define SegA 0x738
#define SegO 0x578
#define SegU 0x568
#define SegG 0x178
static const char *TAG = "PT6961";

#define SegT 0x268

#define SegBlank 0x0
//Din : MOSI
//Dout : MISO

// P105
//unsigned char SegTable[10] = {0x77, 0x24, 0xb3, 0xb6, 0xe4, 0xd6, 0xd7, 0x34, 0xf7, 0xf6};
// P318
//unsigned int SegTable[10] = {0x678,0x048,0x6A8,0x2E8,0x0D8,0x2F0,0x6F0,0x068,0x6F8,0x2F8};
// P319
unsigned int SegTable[10] = {0x578,0x500,0x670,0x750,0x708,0x358,0x378,0x510,0x778,0x758};

//unsigned char DotTable[8] = {0x10, 0x20, 0x04, 0x02, 0x80, 0x40, 0x8, 0x01};
//unsigned int DotTable[8] = {0x20,0x08,0x40,0x0200,0x400,0x10,0x80,0x0100};
//P319
unsigned int DotTable[8] = {0x10,0x400,0x100,0x40,0x20,0x08,0x200,0x80};
//unsigned char DigitMapTable[7] = {0,1,2,3,4,5,6,7}; //P105
unsigned char DigitMapTable[7] = {6,3,4,5,2,0,1}; // P319
//unsigned char DigitMapTable[7] = {0,1,2,3,4,5,6}; // Simple
//unsigned char DigitMapTable[7] = {5,4,3,6,0,1,2}; // TRY
unsigned int DotValue[14];
unsigned char Mode;
#define MaxModule 1
#define DigitPerModule 7


#define SevenDigit 0
#define ThreeDigit 1
#define FourDigit 2

void delay (unsigned char value)
{
    ets_delay_us(20);
}

void digitalWrite (unsigned char pin, unsigned char value)
{
    gpio_set_level(pin, value);
}

// as per data sheet CLK remain high
// set output LSB.
// pulse clock high - low - high

void SPItransfer (unsigned char value)
{
    while (Mode4094)
    {
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    unsigned int i,j;
    Mode4094=1;
    for (i = 0 ; i < 8 ; i++)
    {
        if (value & 0x01)
            gpio_set_level(DAT, 1);
        else    
            gpio_set_level(DAT, 0);
        value = value >> 1;
        ets_delay_us(20);
        gpio_set_level(CLK, 1);
        ets_delay_us(20);
        gpio_set_level(CLK, 0);
        ets_delay_us(20);
    }
    ets_delay_us(10);
    Mode4094=0;
 
}

void ClearSTRBModule (unsigned char module)
{
    digitalWrite(CLK,LOW);
    if (module == 0)
    digitalWrite(STRB1, LOW);
  if (module == 1)
    digitalWrite(STRB2, LOW);
  if (module == 2)
    digitalWrite(STRB3, LOW);
  if (module == 3)
    digitalWrite(STRB4, LOW);
    ets_delay_us(50);

}

void SetSTRBModule (unsigned char module)
{
  ets_delay_us(50);
  if (module == 0)
    digitalWrite(STRB1, HIGH);
  if (module == 1)
    digitalWrite(STRB2, HIGH);
  if (module == 2)
    digitalWrite(STRB3, HIGH);
  if (module == 3)
    digitalWrite(STRB4, HIGH);
  digitalWrite(CLK,LOW);

}
void WriteSPICommand(unsigned char val, unsigned char module)
{
  ClearSTRBModule(module);
  delay(5);
  SPItransfer(val);
  delay(5);
  SetSTRBModule(module);
}

void WriteSPIAddress(unsigned char val, unsigned char module)
{
  ClearSTRBModule(module);
  delay(5);
  SPItransfer(val + 0xc0);
}


unsigned char WriteSPIByte(unsigned char val, unsigned char module)
{
  delay(5); 
  SPItransfer(val);
  delay(5);
}

void Init6961 (void)
{
  unsigned char I, module;
  Mode = SevenDigit;
//  pinMode(STRB1, OUTPUT);
//  pinMode(STRB2, OUTPUT);
//  pinMode(STRB3, OUTPUT);
//  pinMode(STRB4, OUTPUT);
    module = 0;
    WriteSPICommand(0x40, module); // Command 2 Data Setting
    delay(5);

    WriteSPIAddress(0x00, module); // Address Command 3
    delay(5);
    for (int i = 0; i < 14; i++)
    {
      SPItransfer(0x00);
      delay(5);
      DotValue[I] = 0x00;
    }
    delay(5);
    SetSTRBModule(module);

    delay(5);
    WriteSPICommand(0x03, module); // Seven Digit Eleven Segment Command 1
    delay(5);
    WriteSPICommand(0x87, module); // Display Control Command 4
    delay(5);

    WriteSPICommand(0x03, module); // Seven Digit Eleven Segment Command 1
    delay(5);
    WriteSPICommand(0x8F, module); // Display Control Command 4
    delay(5);

   
}


void DisplayDigit1 (unsigned char digit, unsigned char module , unsigned char val)
{
  unsigned char I;
//  digit = digit % DigitPerModule;
  digit = DigitMapTable[digit];
  DotValue[digit] = SegTable[val];

    WriteSPICommand(0x40, module); // Command 2 Data Setting
    delay(5);

    WriteSPIAddress (digit * 2, module);
    delay(5);
    WriteSPIByte(SegTable[val]%0x100,module);
    delay(5);
    WriteSPIByte(SegTable[val]/0x100,module);
    delay(5);
    SetSTRBModule(module);
    delay(5);

    delay(5);
    WriteSPICommand(0x03, module); // Seven Digit Eleven Segment Command 1
    delay(5);
    WriteSPICommand(0x8F, module); // Display Control Command 4
    delay(5);


}

void DisplayDigit3 (unsigned char digit, unsigned char module, unsigned int val)
{
  unsigned char I;
  bool J;
  J = LeadingZeroBlank;

  val = val%1000;
  
  I = val / 100;
  if ((J == true) && (I == 0))
  {
    DisplaySegment1(digit+2,module,0x0000);
  }
  else
  {
      DisplayDigit1(digit + 2,module, I);
      J = false;
  }
  val = val % 100;
  I = val / 10;

  if ((J == true) && (I == 0))
  {
    DisplaySegment1(digit+1,module,0x0000);
  }
  else
  {
      DisplayDigit1(digit + 1,module, I);
  }
  I = val % 10;
  DisplayDigit1(digit,module, I);
  
}


void DisplayDigit2 (unsigned char digit, unsigned char module, unsigned char val)
{
  unsigned char I;
  val = val % 100;
  I = val / 10;
  DisplayDigit1(digit + 1,module, I);
  I = val % 10;
  DisplayDigit1(digit,module, I);
  
}



void DisplayDigit4 (unsigned char digit, unsigned char module, unsigned int val)
{
  unsigned char I;
  bool J;
  J = LeadingZeroBlank;

  I = val / 1000;
  if (LeadingZeroBlank && (I == 0))
    DisplaySegment1(digit+3,module,0x0000);
  else  
  {
    DisplayDigit1(digit + 3,module, I);
    J = false;
  }
  val = val%1000;
  
  I = val / 100;
  if ((J == true) && (I == 0))
  {
    DisplaySegment1(digit+2,module,0x0000);
  }
  else
  {
      DisplayDigit1(digit + 2,module, I);
      J = false;
  }
  val = val % 100;
  I = val / 10;

  if ((J == true) && (I == 0))
  {
    DisplaySegment1(digit+1,module,0x0000);
  }
  else
  {
      DisplayDigit1(digit + 1,module, I);
  }
  I = val % 10;
  DisplayDigit1(digit,module, I);
  
}


void DisplaySegment1 (unsigned char digit, unsigned char module,unsigned int val)
{
  unsigned char I;
  digit = DigitMapTable[digit];
  DotValue[digit] = val;
  WriteSPIAddress (digit * 2, module);
  delay(5);
  WriteSPIByte(val%0x100,module);
  delay(5);
  WriteSPIByte(val/0x100,module);
  delay(5);
  SetSTRBModule(module);
  delay(5);
}

void SetSegment1 (unsigned char digit, unsigned char val)
{
  digit = DigitMapTable[digit];
  DotValue[digit] = val;
}

void OnSegment1 (unsigned char digit,unsigned char module, unsigned char val)
{
  unsigned int I,J;
  I = DotTable[val];
  digit = DigitMapTable[digit];

  DotValue[digit] |= I;
  J = DotValue[digit];
  WriteSPIAddress (digit * 2, module);
  delay(5);
  WriteSPIByte(J%0x100,module);
  delay(5);
  WriteSPIByte(J/0x100,module);
  delay(5);
  SetSTRBModule(module);
  delay(5);
}


void OffSegment1 (unsigned char digit, unsigned char module, unsigned char val)
{
  unsigned int I,J;
  I = DotTable[val];
  digit = DigitMapTable[digit];

  DotValue[digit] &= ~I;
  J = DotValue[digit];
  WriteSPIAddress (digit * 2, module);
  delay(5);
  WriteSPIByte(J%0x100,module);
  delay(5);
  WriteSPIByte(J/0x100,module);
  delay(5);
  SetSTRBModule(module);
  delay(5);
}

void Blank_digits (unsigned char digit,unsigned char module,unsigned char count)
{
    unsigned char I;
    WriteSPIAddress((digit+I)*2,module);   
    for (I=0;I<count;I++)
    {
      WriteSPICommand(0x00,module);
    }
    SetSTRBModule(module);

}

void DisplayRemainingTime (void)
{
    DisplayDigit2(0,0,RemainingTime%60);
    DisplayDigit2(2,0,RemainingTime/60);
    OnSegment1(2,0,7);   
}

void DisplayAverageTime (void)
{
    DisplayDigit1(0,0,AverageTimeToRespond%10);
    DisplayDigit3(1,0,AverageTimeToRespond/10);
    OnSegment1(1,0,7);   
}

// display Cxx where xx is code
void DisplayCode (unsigned int code)
{
 DisplayDigit2(4,0,code);
 DisplaySegment1(6,0,SegC);
}

void DisplayDash (unsigned char digit,unsigned char module,unsigned char count)
{
    unsigned char I;
    for (I=0;I<count;I++)
    {
      DisplaySegment1(digit+I,0,SegDash);
    }

}


void DisplayRemainingCount (void)
{
  int RemainingCount = max_count - current_count;
   DisplayDigit4(0,0,RemainingCount);
   DisplaySegment1(6,0,SegC);
   DisplaySegment1(5,0,SegN);
   DisplaySegment1(4,0,SegT);
}

void DisplayWiFiStatus (void)
{
   DisplaySegment1(6,0,SegS);
   DisplaySegment1(5,0,SegT);
   DisplaySegment1(4,0,SegA);
   if (connected_to_wifi)
   {
    DisplayDigit4(0,0,StatusWiFiOk);
   }
  else
   {
    DisplayDigit4(0,0,StatusWiFiNotOk);
   }
}

// display game and sound mode
void DisplayGameMode (void)
{
    DisplayDigit2(4,0,SegBlank);
    DisplaySegment1(5,0,SegA);
    DisplaySegment1(6,0,SegG);
    DisplayDigit4(0,0,GameMode);

}

void DisplaySoundMode (void)
{
    DisplaySegment1(4,0,SegU);
    DisplaySegment1(5,0,SegO);
    DisplaySegment1(6,0,SegS);
    DisplayDigit4(0,0,SoundMode);
}

// display game and time
void DisplayTMode (void)
{
    // DisplayDigit2(4,0,Mode2LightTime);
    DisplaySegment1(4,0,SegDash);
    DisplaySegment1(5,0,SegT);
    DisplaySegment1(6,0,SegG);
    DisplayDigit2(0,0, PlayTime);
    
}


void DisplayMaxLights(void){
    DisplaySegment1(4,0,SegDash);
    DisplaySegment1(5,0,SegL);
    DisplaySegment1(6,0,SegN);
    DisplayDigit2(0,0,NumberOfLights);
}

void DisplayLightTime(void){
    DisplaySegment1(4,0,SegDash);
    DisplaySegment1(5,0,SegT);
    DisplaySegment1(6,0,SegL);
    DisplayDigit2(0,0,Mode2LightTime);
}


void DisplayOTP (void)
{
    DisplaySegment1(4,0,SegP);
    DisplaySegment1(5,0,SegT);
    DisplaySegment1(6,0,SegO);
    DisplayDigit4(0,0,OTPValue);
}

void DisplayNormal (void)
{
  DisplayDash(0,0,7);
}

void DisplayStatus (void)
{
  SettingButtonPressed++;
  if(SettingButtonPressed==1){
    DisplayWiFiStatus();
  }
  else if(SettingButtonPressed==2)
  {
    DisplayRemainingCount();
  }
  else if(SettingButtonPressed==SettingGameMode)
  {
     DisplayGameMode();
  }
  else if(SettingButtonPressed==SettingSoundMode)
  {
     DisplaySoundMode();
  }
  else if(SettingButtonPressed==SettingPlayTime)
  {
     DisplayTMode();
  }
  else if(SettingButtonPressed==SettingNumberOfLights)
  {
    DisplayMaxLights();
  }
  else if(SettingButtonPressed==SettingLightTime)
  {
    DisplayLightTime();
  }
  else if(SettingButtonPressed== MaximumSettingNumber)
  {
      DisplayGameOverValues();
      SettingButtonPressed = 0;
      // memorise all values changed.
      if (GameModeChanged)
      {
        GameModeChanged = 0;
         utils_nvs_set_int(NVS_GAME_MODE,GameMode);
        
      }
      else if(SoundModeChanged)
      {
        SoundModeChanged=0;
         utils_nvs_set_int(NVS_SOUND_MODE,SoundMode);
      }
      else if(PlayTimeModeChanged)
      {
        PlayTimeModeChanged=0;
        utils_nvs_set_int(NVS_PLAY_TIME,PlayTime);
      }
      else if(LightTimeModeChanged)
      {
        LightTimeModeChanged=0;
        utils_nvs_set_int(NVS_MODE2_TIME,Mode2LightTime);
      }
      else if(LightSettingModeChanged)
      {
        LightSettingModeChanged=0;
         utils_nvs_set_int(NVS_TOTAL_LIGHTS,NumberOfLights);
      }


  }
 
}

void DisplayGameOverValues (void)
{
  // show active time
  if (GameMode == 2)
  {
    DisplayDigit3(4,0,OKSwitchPressedCount);
    DisplayDigit2(0,0,(PlayTime * 60 - RemainingTime)%60);
    DisplayDigit2(2,0,(PlayTime * 60 - RemainingTime)/60);
    OnSegment1(2,0,7);   
  }
  else
  {
            DisplayDigit3(4,0,OKSwitchPressedCount);
            DisplayDigit4(0,0,WrongSwitchPressedCount);

  }
}  