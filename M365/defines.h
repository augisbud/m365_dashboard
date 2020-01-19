#include "WatchDog.h"

// Select either SPI or I2C(Wire) Display Mode
#define DISPLAY_SPI
//#define DISPLAY_I2C

//#define US_Version

#include "SSD1306Ascii.h"
#ifdef DISPLAY_SPI
  #include <SPI.h>
  #include "SSD1306AsciiSpi.h"
  #define PIN_CS  10
  #define PIN_RST 9
  #define PIN_DC  8
  #define PIN_D0 13
  #define PIN_D1 11
#endif
#ifdef DISPLAY_I2C
  #include "SSD1306AsciiWire.h"
#endif
#include "fonts/m365.h"
#if Language == RU
  #include "fonts/System5x7ru.h"
#else
  #include "fonts/System5x7mod.h"
#endif
#include "fonts/stdNumb.h"
#include "fonts/bigNumb.h"

#include <EEPROM.h>

#include "language.h"
#include "messages.h" 

MessagesClass Message;

const uint16_t LONG_PRESS = 2000;
uint8_t warnBatteryPercent = 5;

bool autoBig = true;
uint8_t bigMode = 0;
bool bigWarn = true;

bool Settings = false;
bool ShowBattInfo = false;
bool M365Settings = false;

uint8_t menuPos = 0;
uint8_t sMenuPos = 0;

bool cfgCruise = false;
bool cfgTailight = false;
uint8_t cfgKERS = 0;

volatile int16_t oldBrakeVal = -1;
volatile int16_t oldThrottleVal = -1;
volatile bool btnPressed = false;
bool bAlarm = false;

uint32_t timer = 0; 

#ifdef DISPLAY_SPI
SSD1306AsciiSpi display;
#endif
#ifdef DISPLAY_I2C
SSD1306AsciiWire display;
#endif

bool WheelSize = true; //whell 8,5"

uint8_t WDTcounts = 0;
void(* resetFunc) (void) = 0;

// -----------------------------------------------------------------------------------------------------------

#define XIAOMI_PORT Serial
#define RX_DISABLE UCSR0B &= ~_BV(RXEN0);
#define RX_ENABLE  UCSR0B |=  _BV(RXEN0);

struct {
  uint8_t prepared; //1 if prepared, 0 after writing
  uint8_t DataLen;  //lenght of data to write
  uint8_t buf[16];  //buffer
  uint16_t  cs;       //cs of data into buffer
  uint8_t _dynQueries[5];
  uint8_t _dynSize = 0;
} _Query;

volatile uint8_t _NewDataFlag = 0; //assign '1' for renew display once
volatile bool _Hibernate = false;   //disable requests. For flashind or other things

enum {CMD_CRUISE_ON, CMD_CRUISE_OFF, CMD_LED_ON, CMD_LED_OFF, CMD_WEAK, CMD_MEDIUM, CMD_STRONG};
struct __attribute__((packed)) CMD{
  uint8_t  len;
  uint8_t  addr;
  uint8_t  rlen;
  uint8_t  param;
  int16_t  value;
}_cmd;

const uint8_t _commandsWeWillSend[] = {1, 8, 10}; //insert INDEXES of commands, wich will be send in a circle

        // INDEX                     //0     1     2     3     4     5     6     7     8     9    10    11    12    13    14
const uint8_t _q[] PROGMEM = {0x3B, 0x31, 0x20, 0x1B, 0x10, 0x1A, 0x69, 0x3E, 0xB0, 0x23, 0x3A, 0x7B, 0x7C, 0x7D, 0x40}; //commands
const uint8_t _l[] PROGMEM = {   2,   10,    6,    4,   18,   12,    2,    2,   32,    6,    4,    2,    2,    2,   30}; //expected answer length of command
const uint8_t _f[] PROGMEM = {   1,    1,    1,    1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    1}; //format of packet

//wrappers for known commands
const uint8_t _h0[]    PROGMEM = {0x55, 0xAA};
const uint8_t _h1[]    PROGMEM = {0x03, 0x22, 0x01};
const uint8_t _h2[]    PROGMEM = {0x06, 0x20, 0x61};
const uint8_t _hc[]    PROGMEM = {0x04, 0x20, 0x03}; //head of control commands

struct __attribute__ ((packed)){ //dynamic end of long queries
  uint8_t hz; //unknown
  uint8_t th; //current throttle value
  uint8_t br; //current brake value
}_end20t;

const uint8_t RECV_TIMEOUT =  5;
const uint8_t RECV_BUFLEN  = 64;

struct __attribute__((packed)) ANSWER_HEADER{ //header of receiving answer
  uint8_t len;
  uint8_t addr;
  uint8_t hz;
  uint8_t cmd;
} AnswerHeader;

struct __attribute__ ((packed)) {
  uint8_t state;      //0-stall, 1-drive, 2-eco stall, 3-eco drive
  uint8_t ledBatt;    //battery status 0 - min, 7(or 8...) - max
  uint8_t headLamp;   //0-off, 0x64-on
  uint8_t beepAction;
} S21C00HZ64;

struct __attribute__((packed))A20C00HZ65 {
  uint8_t hz1;
  uint8_t throttle; //throttle
  uint8_t brake;    //brake
  uint8_t hz2;
  uint8_t hz3;
} S20C00HZ65;

struct __attribute__((packed))A25C31 {
  uint16_t  remainCapacity;     //remaining capacity mAh
  uint8_t   remainPercent;      //charge in percent
  uint8_t   u4;                 //battery status??? (unknown)
  int16_t           current;            //current        /100 = A
  int16_t           voltage;            //batt voltage   /100 = V
  uint8_t   temp1;              //-=20
  uint8_t   temp2;              //-=20
} S25C31;

struct __attribute__((packed))A25C40 {
  int16_t c1; //cell1 /1000
  int16_t c2; //cell2
  int16_t c3; //etc.
  int16_t c4;
  int16_t c5;
  int16_t c6;
  int16_t c7;
  int16_t c8;
  int16_t c9;
  int16_t c10;
  int16_t c11;
  int16_t c12;
  int16_t c13;
  int16_t c14;
  int16_t c15;
} S25C40;

struct __attribute__((packed))A23C3E {
  int16_t i1;                           //mainframe temp
} S23C3E;

struct __attribute__((packed))A23CB0 {
  //32 bytes;
  uint8_t u1[10];
  int16_t           speed;              // /1000
  uint16_t  averageSpeed;       // /1000
  uint32_t mileageTotal;       // /1000
  uint16_t  mileageCurrent;     // /100
  uint16_t  elapsedPowerOnTime; //time from power on, in seconds
  int16_t           mainframeTemp;      // /10
  uint8_t u2[8];
} S23CB0;

struct __attribute__((packed))A23C23 { //skip
  uint8_t u1;
  uint8_t u2;
  uint8_t u3; //0x30
  uint8_t u4; //0x09
  uint16_t  remainMileage;  // /100 
} S23C23;

struct __attribute__((packed))A23C3A {
  uint16_t powerOnTime;
  uint16_t ridingTime;
} S23C3A;
