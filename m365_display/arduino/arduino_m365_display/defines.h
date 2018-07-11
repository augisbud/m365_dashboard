#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <gfxfont.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "messages.h"

#define FONT FreeSerifBoldItalic18pt7b
#include <./Fonts/FreeSerifBoldItalic18pt7b.h>

MessagesClass Message;

#define OLED_RESET -1 //no reset
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define XIAOMI_PORT Serial
#define RX_DISABLE UCSR0B &= ~_BV(RXEN0);
#define RX_ENABLE  UCSR0B |=  _BV(RXEN0);

const unsigned char TH_KEY_TRES  =  53; //38-190
const unsigned char BR_KEY_TRES  =  43; //35-169
const unsigned int  LONG_PRESS   = 500;  //
const unsigned int  MENU_INITIAL = 100; //ms

//available screens
enum {NONE, MILEAGE, TEST, BATT, TRIP, MENU, BIG_SPEED, BIG_CURRENT, BIG_AVERAGE, BIG_REMAIN, BIG_MILEAGE, BIG_SPENT, CELLS, CHARGING, NO_BIG, BIG_VOLTS, HIBERNATE, BIG_TIME};
//available messages
enum {MESSAGE_KEY_TH = 0, MESSAGE_KEY_BR, MESSAGE_KEY_BOTH, MESSAGE_KEY_MENU, MESSAGE_KEY_RELEASED, MESSAGE_KEY_TH_LONG, MESSAGE_KEY_BR_LONG, MESSAGE_KEY_ANY};
//available broadcast
enum {BR_MESSAGE_HIBERNATE_ON = 0, BR_MESSAGE_HIBERNATE_OFF};


struct {
  unsigned char prepared; //1 if prepared, 0 after writing
  unsigned char DataLen;  //lenght of data to write
  unsigned char buf[16];  //buffer
  unsigned int  cs;       //cs of data into buffer
  unsigned char _dynQueries[5];
  unsigned char _dynSize = 0;
}_Query;

volatile unsigned char _NewDataFlag = 0; //assign '1' for renew display once
volatile unsigned char _Hibernate = 0;   //disable requests. For flashind or other things



enum {CMD_CRUISE_ON, CMD_CRUISE_OFF, CMD_LED_ON, CMD_LED_OFF, CMD_WEAK, CMD_MEDIUM, CMD_STRONG};
struct __attribute__((packed)) CMD{
  unsigned char len;
  unsigned char addr;
  unsigned char rlen;
  unsigned char param;
  int           value;
}_cmd;


const unsigned char _commandsWeWillSend[] = {1, 8, 10}; //insert INDEXES of commands, wich will be send in a circle
        // INDEX                     //0     1     2     3     4     5     6     7     8     9    10    11    12    13    14
const unsigned char _q[] PROGMEM = {0x3B, 0x31, 0x20, 0x1B, 0x10, 0x1A, 0x69, 0x3E, 0xB0, 0x23, 0x3A, 0x7B, 0x7C, 0x7D, 0x40}; //commands
const unsigned char _l[] PROGMEM = {   2,   10,    6,    4,   18,   12,    2,    2,   32,    6,    4,    2,    2,    2,   30}; //expected answer length of command
const unsigned char _f[] PROGMEM = {   1,    1,    1,    1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    1}; //format of packet

//wrappers for known commands
const unsigned char _h0[]    PROGMEM = {0x55, 0xAA};
const unsigned char _h1[]    PROGMEM = {0x03, 0x22, 0x01};
const unsigned char _h2[]    PROGMEM = {0x06, 0x20, 0x61};
const unsigned char _hc[]    PROGMEM = {0x04, 0x20, 0x03}; //head of control commands

struct __attribute__ ((packed)){ //dynamic end of long queries
  unsigned char hz; //unknown
  unsigned char th; //current throttle value
  unsigned char br; //current brake value
}_end20t;

struct {
  unsigned char activeMenu; //0 - nomenu (0,1,2,3,4)
  unsigned char selItem;
  unsigned char bigVar;     //variant of big digits in riding mode (speed > 1kmh)
  unsigned char dispVar;    //variant of data on display (menu, big, min)
}_Menu;


const char mm1[] PROGMEM = {"RECUP"}; //main menu items
const char mm2[] PROGMEM = {"CRUISE"};
const char mm3[] PROGMEM = {"R-LED"};
const char mm4[] PROGMEM = {"BIG DISP"};
const char mm5[] PROGMEM = {"HIBERNATE"};

const char rm1[] PROGMEM = {"WEAK"};
const char rm2[] PROGMEM = {"MEDIUM"};
const char rm3[] PROGMEM = {"STRONG"};

const char m1[]  PROGMEM = {"ON"};
const char m2[]  PROGMEM = {"OFF"};

const char bm1[] PROGMEM = {"SPEED"};   //BIG screens list
const char bm2[] PROGMEM = {"AVERAGE"};
const char bm3[] PROGMEM = {"CURRENT"};
const char bm4[] PROGMEM = {"SPENT %"};
const char bm5[] PROGMEM = {"MILEAGE"};
const char bm6[] PROGMEM = {"VOLTAGE"};
const char bm7[] PROGMEM = {"PWR TIME"};
const char bm8[] PROGMEM = {"NO BIG"};

const char bp0[] PROGMEM = {"Vlt"};     //for BIG screens
const char bp1[] PROGMEM = {"Ave"};
const char bp2[] PROGMEM = {"Amp"};
const char bp3[] PROGMEM = {"Mil"};
const char bp4[] PROGMEM = {"Spd"};
const char bp5[] PROGMEM = {"Tim"};

const char * menuMainItems[]  = {mm1, mm2, mm3, mm4, mm5};
const char * menuRecupItems[] = {rm1, rm2, rm3};
const char * menuOnOffItems[] = {m1,  m2};
const char * menuBigItems[]   = {bm1, bm2, bm3, bm4, bm5, bm6, bm7, bm8};
const char * dispBp[]         = {bp0, bp1, bp2, bp3, bp4, bp5};


//const unsigned char BR_RELEASED_TRES = 40; //full range (35 -)    range may be different from one to other machines
//const unsigned char TH_RELEASED_TRES = 43; //full range (38 - 196)
const unsigned char RECV_TIMEOUT =  5;
const unsigned char RECV_BUFLEN  = 64;

struct { //D
    //unsigned char recup;  //0 - weak, 1 - medium, 2 - strong
    //unsigned char cruise; //1 - on, 0 - off
    //unsigned char tail;   //tail led 1 - on, 0 - off
    unsigned char th; //throttle
    unsigned char br; //brake
    char          initialPercent;
    int           initialCapacity;
    char          spentPercent;     //percent spented from power On
    int           spentCapacity;
    unsigned int  chargedCapacity;
    unsigned int  sph;      //1  km/h
    unsigned int  spl;      //0.01 km/h
    unsigned int  aveh;     //average
    unsigned int  avel;
    unsigned int  milh;     //mileage current
    unsigned int  mill;
    unsigned int  curh;     //current
    unsigned int  curl;
    char          remPercent;  //remain percent
    int           remCapacity;
    unsigned int  tripMin;
    unsigned int  tripSec;
    unsigned char powerMin;
    unsigned char powerSec;
    unsigned int  milTotH;  //total mileage
    unsigned char milTotL;
    unsigned char temp1;    //first temperature of battery
    unsigned char temp2;
    int           voltage;
    //unsigned char volth;
    //unsigned char voltl;
    unsigned int  loopsTime;
  }D;


struct __attribute__((packed)) ANSWER_HEADER{ //header of receiving answer
  unsigned char len;
  unsigned char addr;
  unsigned char hz;
  unsigned char cmd;
} AnswerHeader;

/*
//----- states of controllable var :)
struct __attribute__ ((packed)) A23C7B{
  int recupMode; // 0 - weak, 1 - medium, 2 - strong
}S23C7B;

struct __attribute__ ((packed)) A23C7D{
  int rearLight; // 2 = ON, 0 = OFF
}S23C7D;

struct __attribute__((packed)) A23C7C{ //UNKNOWN
  int u1; //0 - cruise mode OFF; 1 - cruise mode ON
}S23C7C;
*/

struct __attribute__ ((packed)) {
  unsigned char state;      //0-stall, 1-drive, 2-eco stall, 3-eco drive
  unsigned char ledBatt;    //battery status 0 - min, 7(or 8...) - max
  unsigned char headLamp;   //0-off, 0x64-on
  unsigned char beepAction;
}S21C00HZ64;

struct __attribute__((packed))A20C00HZ65{
  unsigned char hz1;
  unsigned char throttle; //throttle
  unsigned char brake;    //brake
  unsigned char hz2;
  unsigned char hz3;
}S20C00HZ65;

struct __attribute__((packed))A25C31{
  unsigned int  remainCapacity;     //remaining capacity mAh
  unsigned char remainPercent;      //charge in percent
  unsigned char u4;                 //battery status??? (unknown)
  int           current;            //current        /100 = A
  int           voltage;            //batt voltage   /100 = V
  unsigned char temp1;              //-=20
  unsigned char temp2;              //-=20
}S25C31;

struct __attribute__((packed))A25C40{
  int c1; //cell1 /1000
  int c2; //cell2
  int c3; //etc.
  int c4;
  int c5;
  int c6;
  int c7;
  int c8;
  int c9;
  int c10;
  int c11;
  int c12;
  int c13;
  int c14;
  int c15;
}S25C40;

struct __attribute__((packed))A23C3E{
  int i1;                           //mainframe temp
}S23C3E;

struct __attribute__((packed))A23CB0{
  //32 bytes;
  unsigned char u1[10];
  int           speed;              // /1000
  unsigned int  averageSpeed;       // /1000
  unsigned long mileageTotal;       // /1000
  unsigned int  mileageCurrent;     // /100
  unsigned int  elapsedPowerOnTime; //time from power on, in seconds
  int           mainframeTemp;      // /10
  unsigned char u2[8];
}S23CB0;

struct __attribute__((packed))A23C23{ //skip
  unsigned char u1;
  unsigned char u2;
  unsigned char u3; //0x30
  unsigned char u4; //0x09
  unsigned int  remainMileage;  // /100 
}S23C23;

struct __attribute__((packed))A23C3A{
  unsigned int powerOnTime;
  unsigned int ridingTime;
}S23C3A;
