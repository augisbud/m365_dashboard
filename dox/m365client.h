#ifndef M365client_h
#define M365client_h


//REMOVE LATER:
  #define numscreens 8 //this must be updated with current value from display.h, used for requests




#include "definitions.h"
/*#ifdef usedisplay
  #include "display.h"
#endif
*/
#ifdef usedisplay
  class M365Display;
#endif

#include "HardwareSerialPatched.h"

  #ifndef usengfeatuart
    #define M365SerialInit M365Serial.begin(115200,SERIAL_8N1, UART2RX, UART2TX);
    #ifdef debuguarttiming
      #define switch2TX digitalWrite(M365debugtx,HIGH);
      #define switch2RX digitalWrite(M365debugtx,LOW);
    #else
      #define switch2TX
      #define switch2RX
    #endif
  #endif  

//M365 - serial connection - regarding a patch in the sdk and general explanations:
  /* ATTENZIONE - the Arduino Serial Library uses HardwareSerial from the arduino-esp32 core which uses "esp32-hal-uart.cpp" driver from esp-idf which runs on RTOS
  * ESP32 has 128Byte Hardware RX Buffer
  * esp32-hal-uart uses interrupts, but default config toggles the rx-event only after 112 received bytes
  * this leads to the problem -> on m365 we must listen to the packets sent by other nodes (bms/esc/ble modules) and send our stuff with the right timing
  * so long explanation, short solution:
  * in esp32-hal-uart.c, in void uartEnableInterrupt(uart_t* uart) change
  * uart->dev->conf1.rxfifo_full_thrhd = 112;
  * to
  *  uart->dev->conf1.rxfifo_full_thrhd = 1;
  */
  //custom pins see here: http://www.iotsharing.com/2017/05/how-to-use-serial-arduino-esp32-print-debug.html?m=1
  //enable rx/tx only  modes see: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/HardwareSerial.h

//queue buffer struct
  typedef struct {
    uint8_t len;
    uint8_t buffer[256];
  } queuebuf;

//queuehandles
    extern QueueHandle_t receivedqueue;
    extern QueueHandle_t sendqueue;

class M365Client {
  public:
    M365Client(void); //constructor

    #ifdef usedisplay
      virtual void begin(HardwareSerialPatched *uart, M365Display *disp);
    #else
      virtual void begin(HardwareSerialPatched *uart);
    #endif
    virtual void loop(void);
    virtual void end(void);
    void reset_statistics(void);

    bool newdata = false; //flag - we have update at least one byte in one of the data arrays

    uint8_t sendcommand = 0; //set by sketch -> with tasks we use the queue here...

    //M365 Device Buffers & Structs
      uint8_t bledata[512];
      uint8_t x1data[512];
      uint8_t escdata[512];
      uint8_t bmsdata[512];

      typedef struct {
        uint8_t u1[1];
        uint8_t throttle;
        uint8_t brake;
        //uint8_t u2[509];
      }__attribute__((packed))   blestruct;

      typedef struct {
        uint8_t mode; //offset 0x00 mode: 0-stall, 1-drive, 2-eco stall, 3-eco drive
        uint8_t battleds;  //offset 0x01 battery status 0 - min, 7(or 8...) - max
        uint8_t light;  //offset 0x02 0= off, 0x64 = on
        uint8_t beepaction;  //offset 0x03 "beep request from esc: 1:short beep (ble connected), 2 long beep cruise control en/disabled, 3 short beep battery fully charged
        //uint8_t u1[508];
      }__attribute__((packed))   x1struct;

      typedef struct {
        uint16_t u1[0x10]; //offset 0-0x1F
        char serial[14]; //offset 0x20-0x2D
        char pin[6]; //offset 0x2E-0x33
        uint8_t fwversion[2]; //offset 0x34-035,  e.g. 0x133 = 1.3.3
        uint16_t u2[10]; //offset 0x36-0x49
        uint16_t remainingdistance; //offset 0x4a-0x4B e.g. 123 = 1.23km
        uint16_t u3[20]; //offset 0x4C-0x73
        uint16_t ontime1; //offset 0x74-0x75 power on time in seconds
        uint16_t triptime; //offset 0x76-0x77 trip time in seconds
        uint16_t u4[2]; //offset 0x78-0x7C
        uint16_t frametemp1; //offset 0x7C-0x7D /10 = temp in °C //Unused on M365/FW1.4.0
        uint16_t u5[54]; //offset 0x7e-0xe9
        uint16_t ecomode; //offset 0xEA-0xEB; on=1, off=0
        uint16_t u6[5]; //offset 0xec-0xf5
        uint16_t kers; //offset 0xf6-0xf7; 0 = weak, 1= medium, 2=strong
        uint16_t cruisemode; //offset 0xf8-0xf9, off 0, on 1
        uint16_t taillight; //offset 0xfa-0xfb, off 0, on 2
        uint16_t u7[50]; //offset  0xfc-0x15f
        uint16_t error; //offset 0x160-0x161
        uint8_t alert; //offset 0x162, alarmstate 0x00=off, 0x09 = on
        uint8_t u8[1]; //offset 0x163
        uint8_t lockstate; //offset 0x164, 0 = unlocked, 2 = locked, 6 = locked & alert
        uint8_t u9; //offset 0x165
        uint16_t u10; //offset 0x166-0x167
        uint16_t battpercent; //offset 0x168-0x169
        //int16_t speed; //0x16A-0x16B /1000 in km/h, negative value = backwards...
        uint16_t speed; //0x16A-0x16B [fixspeed] /1000 in km/h, negative value = backwards...
        uint16_t averagespeed; //0x16C-0x16D /1000 in km/h?
        uint32_t totaldistance; //0x16e-0x171 /1000 in km
        uint16_t tripdistance; //0x172-0x173
        uint16_t ontime2; //offset 0x174-0x175 power on time 2 in seconds
        uint16_t frametemp2; //0x176-0x177 /10 = temp in °C
        uint16_t u11[68]; //offset 0x178-0x200 
      }__attribute__((packed))   escstruct;

      typedef struct {
        uint16_t u1[0x10]; //offset 0-0x1F
        char serial[14]; //offset 0x20-0x2D
        uint8_t fwversion[2]; //offset 0x2E-0x2f e.g. 0x133 = 1.3.3
        uint16_t totalcapacity; //offset 0x30-0x31 mAh
        uint16_t u2a[2]; //offset 0x32-0x35
        uint16_t cycles; //offset 0x36-0x37
        uint16_t chargingtimes; //offset 0x38-0x39
        uint16_t u2b[3]; //offset 0x3a-0x3f
        uint16_t proddate; //offset 0x40-0x41
            //fecha a la batt 7 MSB->año, siguientes 4bits->mes, 5 LSB ->dia ex:
          //b 0001_010=10, año 2010
          //        b 1_000= 8 agosto
          //            b  0_1111=15 
          //  0001_0101_0000_1111=0x150F 
        uint16_t u3[0x10]; //offset 0x42-0x61
        uint16_t remainingcapacity; //offset 0x62-0x63
        uint16_t remainingpercent; //offset 0x64-0x65
        int16_t current; //offset 0x66-67 - negative = charging; /100 = Ampere
        uint16_t voltage; //offset 0x68-69 /10 = Volt
        uint8_t temperature[2]; //offset 0x6A-0x6B -20 = °C
        uint16_t u4[5]; //offset 0x6C-0x75
        uint16_t health; //offset 0x76-0x77; 0-100, 60 schwellwert "kaputt"
        uint16_t u5[4]; //offset 0x78-0x7F
        uint16_t Cell1Voltage; //offset 0x80-0x81
        uint16_t Cell2Voltage; //offset 0x82-0x83
        uint16_t Cell3Voltage; //offset 0x84-0x85
        uint16_t Cell4Voltage; //offset 0x86-0x87
        uint16_t Cell5Voltage; //offset 0x88-0x89
        uint16_t Cell6Voltage; //offset 0x8A-0x8B
        uint16_t Cell7Voltage; //offset 0x8C-0x8D
        uint16_t Cell8Voltage; //offset 0x8E-0x8F
        uint16_t Cell9Voltage; //offset 0x90-0x91
        uint16_t Cell10Voltage; //offset 0x92-0x93
        uint16_t Cell11Voltage; //offset 0x94-0x95 not stock, custom bms with 12S battery, 0 if not connected
        uint16_t Cell12Voltage; //offset 0x96-0x97 not stock, custom bms with 12S battery, 0 if not connected
        //uint16_t u6[178]; //offset 0x98-0x
      }__attribute__((packed))  bmsstruct;

      blestruct* bleparsed = (blestruct*)bledata;
      x1struct* x1parsed = (x1struct*)x1data;
      escstruct* escparsed = (escstruct*)escdata;
      bmsstruct* bmsparsed = (bmsstruct*)bmsdata;
 
    //M365 - Statistics
      uint16_t packets_rec=0;
      uint16_t packets_crcok=0;
      uint16_t packets_crcfail=0;
      uint16_t packets_rec_bms=0;
      uint16_t packets_rec_esc=0;
      uint16_t packets_rec_x1=0;
      uint16_t packets_rec_ble=0;
      uint16_t packets_rec_unhandled=0;

      uint16_t packetsperaddress[256];
      uint16_t requests_sent_bms=0;
      uint16_t requests_sent_esc=0;
      uint16_t esccommands_sent=0;
      uint16_t x1commands_sent=0;
      uint16_t requests_sent_housekeeper=0;

      int16_t speed_min = 0;
      int16_t speed_max = 0;
      int16_t current_min = 0;
      int16_t current_max = 0;
      int32_t watt_min = 0;
      int32_t watt_max = 0;
      uint8_t tb1_min = 255; //temperature batt 1 min
      uint8_t tb1_max = 0; //temperature batt 1 max
      uint8_t tb2_min = 255;  //temperature batt 2 min
      uint8_t tb2_max = 0;  //temperature batt 2 max
      uint16_t te_min = 1000;  //temperature esc min
      uint16_t te_max = 0;  //temperature esc max
      uint16_t lowest=10000; //Cell Voltages - Lowest
      uint16_t highest=0; //Cell Voltages - Highest

    //Timing Statistics
      unsigned long m365packettimestamp = 0;
      unsigned long m365packetlasttimestamp = 0;
      unsigned long duration_requestcycle=0;
      unsigned long timestamp_requeststart=0;
    
    //alerts
      bool alert_escerror = false;
      bool alert_cellvoltage = false;
      bool alert_bmstemp = false;
      bool alert_esctemp = false;
      bool alert_undervoltage = false;
      bool alert_lockedalarm = false;
      uint16_t alertcounter_escerror = 0;
      uint16_t alertcounter_cellvoltage = 0;
      uint16_t alertcounter_bmstemp = 0;
      uint16_t alertcounter_esctemp = 0;
      uint16_t alertcounter_undervoltage = 0;
      uint16_t alertcounter_lockedalarm = 0;

    //requests
      uint16_t subscribedrequests =  rqsarray[0];
      uint16_t capacitychargestart = 0;
      uint8_t requestindex = 0;

      //friendly names
      #define rq_esc_essentials 0x0001
      #define rq_esc_remdist 0x0002
      #define rq_esc_essentials2 0x0004
      #define rq_bms_essentials 0x0008
      #define rq_bms_cells 0x0010
      #define rq_bms_health 0x0020
      #define rq_bms_versioninfos 0x0040
      #define rq_esc_versioninfos 0x0080
      #define rq_esc_config 0x0100

    //mapping oled screens / requeired data
         uint16_t rqsarray[numscreens] = {
        rq_esc_essentials|rq_esc_remdist|rq_esc_essentials2|rq_bms_essentials|rq_bms_cells|rq_bms_health|rq_bms_versioninfos|rq_esc_versioninfos, //rq_esc_essentials|rq_bms_essentials|rq_esc_essentials2, //screen_stop
        //0xff, //screen_stop, TODO: Request infos like Serial/FW Version only _once_ (when entering subscreen)
        rq_esc_essentials|rq_bms_essentials, //screen_drive
        rq_esc_essentials, //screen_error
        rq_esc_essentials, //screen_timeout
        rq_esc_essentials|rq_bms_essentials|rq_bms_cells, //charging
        rq_esc_essentials|rq_esc_config, //configmenu
        rq_esc_essentials|rq_esc_essentials2|rq_bms_essentials, //alarm
        rq_esc_essentials //screen_locked
      };

    //M365 - packets
      //offsets in recbuf
      #define i_address 0
      #define i_hz 1
      #define i_offset 2
      #define i_payloadstart 3
      #define address_ble 0x20 //actively sent data by BLE Module with gas/brake & mode values
      #define address_x1 0x21 //actively sent data by ?BLE? with status like led on/off, normal/ecomode...
      #define address_esc 0x23 
      #define address_bms_request 0x22 //used by apps to read data from bms
      #define address_bms 0x25 //data from bms sent with this address (only passive if requested via address_bms_request)

    //M365 - serial receiver
      uint8_t m365receiverstate = 0;
      uint8_t m365receiverstateold = 0;
      
    //queue debugstuff between core & controller
			uint16_t recqueuein_ok = 0;
			uint16_t recqueuein_fail = 0;
			uint16_t recqueueout_ok = 0;
			uint16_t recqueueout_fail = 0;
			uint16_t recqueuewanted=0;
			uint16_t recqueueignored=0;

			uint16_t sendqueuein_ok = 0;
			uint16_t sendqueuein_fail = 0;
			uint16_t sendqueueout_ok = 0;
			uint16_t sendqueueout_fail = 0;  
  
  protected:
    HardwareSerialPatched *_uart;
    #ifdef usedisplay
      M365Display *_disp;
    #endif

    //M365 - serial receiver
      //#define maxreclen 256 //serial buffer size in bytes
      uint8_t crc1=0;
      uint8_t crc2=0;
      uint16_t crccalc=0;
      //uint8_t recbuf[maxreclen];
      //uint8_t reclen=0;
      uint8_t readindex=0;
      #define m365receiveroff 0 //Serial1 will not be read...
      #define m365receiverready 1 //reading bytes until we find 0x55
      #define m365receiverpacket1 2 //preamble 0x55 received, waiting for 0xAA
      #define m365receiverpacket2 3 //len preamble received waiting for LEN
      #define m365receiverpacket3 4 //payload this state is kept until LEN bytes received
      #define m365receiverpacket4 5 //checksum receiving checksum
      #define m365receiverpacket5 6 //received a packet, test for checksum
      #define m365receiverstorepacket 7 //packet received, checksum valid, store in array, jump back to receiverready for next packet, set newpacket flag

      queuebuf receiverbuffer; //reiverbuffer used by receiver
      bool senddata = false; //flag - set in receiver to signal controller to send data now

    //M365 - command stuff
      #define esccommandlen 10
      //uint8_t esccommand[esccommandlen] = { 0x55,0xAA,0x04,0x20,0x03,0x01,0x02,0x03,0xB7,0xFF};
      #define esccommand_cmd 5
      #define esccommand_value1 6
      #define esccommand_value2 7
      #define esccommand_crcstart 2
      #define esccommand_crc1 8
      #define esccommand_crc2 9

      #define x1commandlen 12
      //uint8_t x1command[x1commandlen] = { 0x55,0xAA,0x06,0x21,0x64,0x00,0x02,0x07,0x00,0x03,0x68,0xff};
      #define x1command_beepnum 9
      #define x1command_crcstart 2
      #define x1command_crc1 10
      #define x1command_crc2 11

    //sendcommand stuff  
      #define cmd_none 0
      #define cmd_kers_weak 1
      #define cmd_kers_medium 2
      #define cmd_kers_strong 3
      #define cmd_cruise_on 4
      #define cmd_cruise_off 5
      #define cmd_light_on 6
      #define cmd_light_off 7
      #define cmd_turnoff 8
      #define cmd_lock 9
      #define cmd_unlock 10
      #define cmd_beep5 11

    //housekeeper (manages alerts,...)
      uint8_t hkstate = 0;
      uint8_t hkstateold=0;
      #define hkwaiting 0
      #define hkrequesting 1
      #define hkevaluating 2
      unsigned long housekeepertimestamp=0;

    //M365 - request stuff 
      /*
      request data from esc:   (Read 0x7x 2 words)
      PREAMBLE  LEN  Adr  HZ   Off  LEN  FIX1 FIX2 FIX30x
      0x55 0xaa 0x06 0x20 0x61 0x7c 0x02 0x02 0x28 0x27 CRC1 CRC2
      request data from bms: (Read Serial @ Offset 0x10, 0x10 words)
      PREAMBLE    LEN   Adr   HZ    Off   Len   CRC1  CRC2
      0x55  0xAA  0x03  0x22  0x01  0x10  0x12  0xB7  0xFF
      */

    //packet for bms requests and offsets
      #define requestbmslen 9
      //uint8_t request_bms[requestbmslen] = { 0x55,0xAA,0x03,0x22,0x01,0x10,0x3A,0xB7,0xFF};
      #define bms_request_offset 5
      #define bms_request_len 6
      #define bms_request_crcstart 2
      #define bms_request_crc1 7
      #define bms_request_crc2 8
    //packets for esc requests and offsets
      #define requestesclen 12
      //uint8_t request_esc[requestesclen] = { 0x55,0xAA,0x06,0x20,0x61,0x10,0xAC,0x02,0x28,0x27,0xB7,0xFF};
      #define esc_request_offset 5 
      #define esc_request_len 6
      #define esc_request_throttle 8
      #define esc_request_brake 9
      #define esc_request_crcstart 2
      #define esc_request_crc1 10
      #define esc_request_crc2 11

    //request arrays  
      #define requestmax 9
      uint8_t requests[requestmax][3]= {
          { address_esc, 0xB0, 0x20}, //error, alarm, lockstate, battpercent,speed,averagespeed,totaldistance,tripdistance,ontime2,frametemp2
          { address_esc, 0x25, 0x02}, //remaining distance
          { address_esc, 0x3A, 0x0A}, //ontime, triptime, frametemp1
          { address_bms, 0x31, 0x0A}, //remaining cap & percent, current, voltage, temperature
          { address_bms, 0x40, 0x18}, //cell voltages (Cells 1-10 & 11-12 for 12S Batt/Custom BMS)
          { address_bms, 0x3B, 0x02}, //Battery Health
          { address_bms, 0x10, 0x22}, //serial,fwversion,totalcapacity,cycles,chargingtimes,productiondate
          { address_esc, 0x10, 0x16}, //serial,pin,fwversion
          { address_esc, 0x7B, 0x06}  //kers, cruisemode, taillight
      };
      uint16_t housekeeperrequests = rq_bms_cells|rq_esc_essentials|rq_bms_essentials;
      uint8_t hkrequestindex=0;

    //private methods
      void handlehousekeeper(void);
      void updatebattstats(void);
      void updatestats(void);
      void handlerequests(void);
      void handlepacket(void);
      void receiver(void);
      void sendesccommand(uint8_t cvalue, uint8_t cparam1, uint8_t cparam2);
      void sendx1command(uint8_t beepnum);
      void sendrequest(uint8_t radr, uint8_t roffset, uint8_t rlen);
}; //M365 Class

#endif