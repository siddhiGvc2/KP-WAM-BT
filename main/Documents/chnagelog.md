## 040925
-- send messages to BT also apart from TCP/MQTT

## 040925
BT code working
Same Led Blink should not repeat.
Number Of Lights set -8
software version 130



## 120825
add BT
what is received from BT - treat it as command.
send reply of command to BT 
same as UART

add BT_NAME as set by a acommand "SetBTName:"
at power on use name as "JnF-SerialNumber" or "JnF-BTName"
## 110825
version - 128
disaable all UART commands except AUDIO PLAY. working (10:30 - 11:00)

In Game mode 0 start - do not show 1/2/3
0001.mp3 Mummy
0002.mp3 daddy
0003.mp3 Bua
0004.mp3 nani
0005.mp3 nanu
0006.mp dadi
0007.mp3 dadu
0008.mp3 chachu

0011.mp3 mango
0012.mp3 banana
0013.mp3 peach

## 080825
playSound.c file added from LiftUPS code.
variable defined for file 'fileToBeRun'
variables added urrent_time_us , elapsed_ms, last_elapsed_ms, last_audio_ms;


to be added - 
1. Change play time, GMode, SMode by user (VC) Start Time : 5:00 PM
power on by pressing a switch
This will display GMode and S Mode -5:30 PM

2. Game Level - 0(simple - any switch pressed is counted as 1. NO qyestion of error /1 (present)/2/upto 9

3. Remote command *GMode:0/1/2 and *GMode?# and *SMode:0 to 99# and *SMode?# (SD)

4. Display Game Mode and Sound Mode when displaying Stat and Cnt (SD/VC)-05:00 - 05:25


6. 

## 060825
Display remaining count at power on and every game
10:35

Dispaly Wifi status when WiFi switch is pressed
unit goes to wifi mode if switch is pressed for more than 4 seconds else it moves to normal mode

coding done at 10:58 (25 minute)
compilation - burning - testing



## 050825
add code for 8th LED - VC
remove reset when wifi not found - VC
two set  commands - MAXGAMES, RESET CURRENT COUNT, - done
three query commands - MAXGAMES, CURRENTCOUNT, LIFE COUNT - done
three varaiables, max count, current count, life count(32 bit) - done
three NVS - max count, current count,life count (32 bit) - done
stop games when counts exceed -done
reduce wait time from 30 secs to 6 seconds -done
reduce ready time from 5 to 3 -done

how to make 8 point game - starting at 2:10 PM
1. choose random number divided by 8 - 2:13
2. when 8 th light selected - switch on INH light - 2:25
3. Sense 8th Switch - 2:31 OVER


ESP_LOGI when game not starting due to count issue - 2:35
Displa ERR when above condition
## 300725
when time is over
display okay entry in top row and wrong entries in second row

## 1.22I - 010525
## Modify
- All MQTT commands to have user name and time stamp similar to TCP commands
#3 020525
-- send TC command when coin inserted / hardware.c
-- created a new function SendTCResponse / mqtt.c
-- created new variable TCPRequired, Send response only if TCPRequired is set / vars. externvars, calls
-- create one more sub topic as GVC/KP/BROADCAST mqtt.c
-- if HBT-OK not received in X minutes and wifi is okay and MQTT is connected the reset the device
-- HBT-D is the command from device and HBT-S is the command from server
-- Pulse wisth is between 20 and 250 msec / hardware.c

#4 030525
-- stack for publish_mqtt task increased from 5k to 8k
#5 050525
-- do not reply when *HBT-S# is received / command.c
-- QOS option added on Subscribe
-- compare topic with broadcast as well serial number to allow action
-- ??? some times data not being senses by MQTT
-- ??? on 3rd May 2:52:00 pm one pulse missed while generating or receiving
-- blink leds when MQTT received moved 3 lines from analysePacketTcp.c to command.c

## 1.23- 090525
#1 090525
-- Add setting for MQTT_BROKER1, MQTT_BROKER2, MQTT_BROKER3. changed in /main/inc/defs
-- Change mqtt server as per *SIP:x# changed in /main/src/SaveRecallNVS.c
-- *MQTT:user:password# command added & saved in memory  changed in /main/src/commands.c
-- sent TC-D at sendTCresponse  changed in /main/src/mqttRoutine.c
-- *MQTT?# command added changed in /main/src/commands/c
--- & saved MQTT default user & pASSWORD values in memory. changed in/main/src/SaveRecallNvs.c
-- remove comparison of topic with expected topic  case MQTT_EVENT_DATA:/mqttRoutines.c
-- when INH input changes, send TCP/MQTT/UARt

## 1.23 - 100525
-- write code to get free heap and free internal heap
-- get that value on command *HEAP?# /main/src/analysePacketUart.c
-- send value to MQTT and TCP and UART 
-- removed retyrMqtt task . /main/main.c
-- enabled auto reconnect in mqtt configuration. //.network.disable_auto_reconnect = false,
-- *MIP:MipUsername:MipDateTime:MipNumber# command added in commands.c
-- & recalled from SaveRecallNVS.c
-- *MIP? command added in commands.c

## 1.23A - 290525
-- *TESTON# will start self test. Send pulse once per second and MQTT when all seven pulses match with input
-- change TESTCOIN and gpio_read_n_act
-- get RSSI info every 15 seocnds if wifi connected
-- *RSSI?# reply with *RSSI,value);


## 1.23 C - 130625
-- UartDebugInfo variannble added to disable Uart Logs.
-- Only QR:, STATUS? commands enabled on uart.

## KP-WAM  010725
-- *LTime:LightTime# 
-- *PTime:PlayTime#
-- *NL:NumberOfLights#
commands added.

## KP-WAM
-- at power on randomly switch on lights for Light Time
-- Start when power on (will add start switch later)
-- switch on random light for LTime

## 020725
-- add code in task and not in for loop as this waits for WiFi
-- light on time is as per setting
-- sense switch (if pressed when light is on - This is success, if wrong pressed - error 1, if not prrssed - error 2)

-- Blink Time = LightTime * 100, do this at NVS recall and when setting

-- remove switch press width
-- increment count

-- set all three error count = 0 at power on
-- send error coyunt to MQTT/TCP as *LedErrCount:OKCount:WrongCount:NoCount# when command is *LedErrCoun?#

-- set time gap between lights 
-- start when any switch is pressed
-- stop when three missed counts

## 030725
-- GapBeforeNextTime SET/RECALL added

## 040725

-- New format added *Status,START# and *Status,STOP#
-- new format to be added
*Status,x,y,ok,notok,missed#
change to *Status,x,y,ok,notokay,missed,time,avgtime#
time is in x.y Secs 4.3 Secs

-- change number of lights to 7
-- and shift them to output now

## 050725
-- started at 11:30
-- sense switch. if sensed, switch off light - done 11:52
-- and not time period in 100s mssec ie 1 second as 10 - done at 12:20 PM
-- stop game if key not pressed for some value (GTime as it is already set)
-- stop game after playtime is over
-- display remaining time
-- whem game over display result
-- no sensing for 30 seconds

## 060725
-- blink result when game over.
-- blink starting count down timer
-- add leading zero blanking
-- display total okay and avaerga time for switch pressed
-- add buzzer , Buzzer time is set by LTime and saved as BlinkLedTime  = 10 * LTime

## 210725
-- add #START# COMMAND

# 220725
-- display serial number at power on (11:30 - 12:00)
-- Do WiFi addition via Boot Switch (12:00 - 12:30)
-- Enable or disable local start (12:40 -01:10)
-- when wrong switch pressed - do nothing / reduce count on display (1:10 - 1:35)
-- set Negative marking , save local enable
-- change command to set GameMode = 0/1/2/3
-- gameMode = NegativeMarkeing * 2 + LocalEnable
-- 0 - No Negative marking, no local enable
-- 1 - No Negative marking,  local enable
-- 2 - Negative marking, no local enable
-- 3 - Negative marking, local enable






