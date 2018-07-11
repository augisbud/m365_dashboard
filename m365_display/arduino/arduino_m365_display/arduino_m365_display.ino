#include "defines.h"


/*
 * screen1 //brake
 *  distance
 *  percent cost
 *  capacity cost
 *  time of ride
 *  time power on
 * 
 * screen2 //throttle
 *  remain percent
 *  remain capacity
 *  B-temp 1
 *  B-temp 2
 * 
 * screen3 //both
 *  current
 *  temperature
 *  total mileage
 */

void setup() {
  XIAOMI_PORT.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)

  display.clearDisplay();      //------------------------------
  display.setTextSize(2);      //this segment for debug only
  display.setCursor(1,1);
  display.setTextColor(WHITE);
  display.println(F("Hello Kate")); //for wife :) just my fun tradition for devices with screens
  display.println();
  display.println(F("- no BUS -"));

  display.display();
  delay(200);


  _Menu.dispVar = TRIP;
  _Menu.bigVar = eeprom_read_byte(0);
}


void loop() {
  static unsigned long timer     = 0;
  static unsigned int  counter   = 0;
  static unsigned int  loopsTime = 0;
  counter++;

  if(counter >= 1000){            //simple performance meter - shows maximum ever "loop time" in microseconds
    loopsTime = millis() - timer;
    if(loopsTime > D.loopsTime){
      D.loopsTime = loopsTime;
    }
    timer = millis();
    counter = 0;
    if(_Hibernate == 1){
      _NewDataFlag = 1;
    }
  }

  dataFSM();            //communication with m365
  keyProcessFSM();      //tracks throttle and brake and send messages for menuControlFSM()
  menuControlFSM();     //switch menu variables
  screenSwitcherFSM();  //controls info on display


  if(Message.GetBroadcast(BR_MESSAGE_HIBERNATE_ON)){
    _Hibernate = 1;
  }
  if(Message.GetBroadcast(BR_MESSAGE_HIBERNATE_OFF)){
    _Hibernate = 0;
  }

  if(_Query.prepared == 0){     //it's for possibility sending custom queries instead predetermined
    prepareNextQuery();
  }

  if(_NewDataFlag == 1){
    _NewDataFlag = 0;
    calculate();
    displayRoutine(_Menu.dispVar);
  }


  Message.Process();
  Message.ProcessBroadcast();
}

void screenSwitcherFSM(){ //switch screen between stall and run (BIG_DIGITS) modes
 static unsigned char step = 0;
 static unsigned char dispVar;

  if(Message.GetBroadcast(BR_MESSAGE_HIBERNATE_ON)){
    dispVar = _Menu.dispVar;
    _Menu.dispVar = HIBERNATE;
  }

  if(Message.GetBroadcast(BR_MESSAGE_HIBERNATE_OFF)){
    _Menu.dispVar = dispVar;
  }

  if(_Menu.dispVar == MENU || _Menu.dispVar == HIBERNATE){    //switching not needed
    return;
  }

  switch(step){
    case 0: //stop
      if(Message.Get(MESSAGE_KEY_BOTH)){
        _Menu.dispVar = CELLS;
      }
      if(Message.Get(MESSAGE_KEY_TH)){  //ALL MESSAGES ARE SENT IF SPEED < 1 kmh ONLY!!!
        _Menu.dispVar = BATT;
      }
      if(Message.Get(MESSAGE_KEY_BR)){
        _Menu.dispVar = TRIP;
      }
      if(D.sph == 0 && D.spl == 0 && S25C31.current < 0){ //go into charging screen
        _Menu.dispVar = CHARGING;
        step = 1;
      }
      if(D.sph > 0){
        if(_Menu.bigVar == NO_BIG){
          step = 10;
          break;
        }
        dispVar = _Menu.dispVar;            //store current display
        _Menu.dispVar = _Menu.bigVar;       //switch to BIG
        step = 10;
      }
      break;
    case 1: //charging
      if(Message.Get(MESSAGE_KEY_BOTH)){    //await ends of charging
        _Menu.dispVar = CELLS;
      }
      if(Message.Get(MESSAGE_KEY_BR)){
        _Menu.dispVar = CHARGING;
      }
      if(S25C31.current >= 0){              //return from charging
        step = 10;
      }
      break;
    case 10:                                //await scooter stop
      if(D.sph == 0){
        _Menu.dispVar = dispVar;
        step = 0;
      }
      break;
    default:
      if(D.sph == 0){
        _Menu.dispVar = dispVar;
        step = 0;
      }
      break;
  }
}

//terrible menu, like nuclear war :)
void menuControlFSM(){    //store display mode, switch on display menu, accept messages, switch menu, react on select menu items (long keys)
  static unsigned char step = 0;
  static unsigned long timer = 0;
  static unsigned char dispVar;

  switch(step){
    case 0:
      if(Message.Get(MESSAGE_KEY_MENU)){ //enter into menu
        timer = millis();
        dispVar = _Menu.dispVar;
        _Menu.dispVar = MENU;
        _Menu.selItem = 1;
        step = 1;
      }
      break;
    case 1:                                                           //********* MAIN MENU HERE
      if(millis() - timer >= 3000){ //exit from menu via timeout
        step = 2;
        break;
      }

      if(D.sph > 0){                //exiting from menu if speed increasing
        step = 2;
        break;
      }

      if(Message.Get(MESSAGE_KEY_BR)){    
        timer = millis();               //renew timeout
        switch(_Menu.activeMenu){
          case 0:                       //if already the main menu
            step = 2;                   //exit from menu
            break;
          default:
            _Menu.activeMenu = 0;       //one level up
            _Menu.selItem = 1;
        }
        
      }

      if(Message.Get(MESSAGE_KEY_TH)){
        timer = millis();
        switch(_Menu.activeMenu){       //scrolling main menu
          case 0:
            _Menu.selItem++;
            if(_Menu.selItem > 5){
              _Menu.selItem = 1;
            }
            break;
          case 1: //recup menu
            _Menu.selItem ++;
            if(_Menu.selItem > 3){
              _Menu.selItem = 1;
            }
            break;
          case 2: //cruise menu
            _Menu.selItem ++;
            if(_Menu.selItem > 2){
              _Menu.selItem = 1;
            }
            break;
          case 3: //led menu
            _Menu.selItem ++;
            if(_Menu.selItem > 2){
              _Menu.selItem = 1;
            }
            break;
          case 4: //big display menu
            _Menu.selItem ++;
            if(_Menu.selItem > 8){
              _Menu.selItem = 1;
            }
            break;
        }
      }

      if(Message.Get(MESSAGE_KEY_TH_LONG)){   //get seleted item and send appropriate command
        timer = millis();
        switch(_Menu.activeMenu){
          case 0: //main menu
            switch(_Menu.selItem){            //go into submenu
              case 1: //sel recup
                _Menu.selItem = 1;
                _Menu.activeMenu = 1;
                break;
              case 2: //sel cruise
                _Menu.selItem = 1;
                _Menu.activeMenu = 2;
                break;
              case 3: //sel led
                _Menu.selItem = 1;
                _Menu.activeMenu = 3;
                break;
              case 4: //sel big data
                switch(_Menu.bigVar){
                  case BIG_SPEED:
                    _Menu.selItem = 1;
                    break;
                  case BIG_AVERAGE:
                    _Menu.selItem = 2;
                    break;
                  case BIG_CURRENT:
                    _Menu.selItem = 3;
                    break;
                  case BIG_SPENT:
                    _Menu.selItem = 4;
                    break;
                  case BIG_MILEAGE:
                    _Menu.selItem = 5;
                    break;
                  case BIG_VOLTS:
                    _Menu.selItem = 6;
                    break;
                  case BIG_TIME:
                    _Menu.selItem = 7;
                    break;
                  case NO_BIG:
                    _Menu.selItem = 8;
                    break;
                    
                  default:
                    _Menu.selItem = 1;
                    break;
                }
                _Menu.activeMenu = 4;
                break;
              case 5: //hibernate
                if(_Hibernate == 0){
                  Message.PostBroadcast(BR_MESSAGE_HIBERNATE_ON);
                }else{
                  Message.PostBroadcast(BR_MESSAGE_HIBERNATE_OFF);
                }
                step = 2;
                break;
            }
            break;
          case 1: //recup menu
            switch(_Menu.selItem){
              case 1: //weak
                prepareCommand(CMD_WEAK);
                /*
                memcpy_P(&_Query.buf, recup[0], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
              case 2: //medium
                prepareCommand(CMD_MEDIUM);
                /*
                memcpy_P(&_Query.buf, recup[1], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
              case 3: //strong
                prepareCommand(CMD_STRONG);
                /*
                memcpy_P(&_Query.buf, recup[2], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
            }
            break;
          case 2:  //cruise menu
            switch(_Menu.selItem){
              case 1: //on
                prepareCommand(CMD_CRUISE_ON);
                /*
                memcpy_P(&_Query.buf, cruise[0], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
              case 2: //off
                prepareCommand(CMD_CRUISE_OFF);
                /*
                memcpy_P(&_Query.buf, cruise[1], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
            }
            break;
          case 3:  //led menu
            switch(_Menu.selItem){
              case 1: //on
                prepareCommand(CMD_LED_ON);
                /*
                memcpy_P(&_Query.buf, led[0], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
              case 2: //off
                prepareCommand(CMD_LED_OFF);
                /*
               memcpy_P(&_Query.buf, led[1], 8);
                _Query.DataLen = 6;
                _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
                _Query.prepared = 1;
                */
                step = 2;
                break;
            }
            break;
          case 4:  //big menu dependencies
            switch(_Menu.selItem){
              case 1: //speed
                _Menu.bigVar = BIG_SPEED;
                break;
              case 2: //current
                _Menu.bigVar = BIG_AVERAGE;
                break;
              case 3: //average speed
                _Menu.bigVar = BIG_CURRENT;
                break;
              case 4:
                _Menu.bigVar = BIG_SPENT;
                break;
              case 5:
                _Menu.bigVar = BIG_MILEAGE;
                break;
              case 6:
                _Menu.bigVar = BIG_VOLTS;
                break;
              case 7:
                _Menu.bigVar = BIG_TIME;
                break;
              case 8:
                _Menu.bigVar = NO_BIG;
              break;
            }
            eeprom_write_byte(0, _Menu.bigVar); //store
            step = 2;
            break;
        }
      }

      if(Message.Get(MESSAGE_KEY_BOTH)){
        timer = millis();
      }

      if(Message.Get(MESSAGE_KEY_MENU)){      //long both - exit from menu
        timer = millis();
        break;
      }

      if(Message.Get(MESSAGE_KEY_BR_LONG)){   //exit from menu
        timer = millis();
        break;
      }
      break;
    case 2:
      _Menu.activeMenu = 0;
      _Menu.selItem = 0;
      _Menu.dispVar = dispVar;
      step = 0;
      _NewDataFlag = 1; //refresh screen
      break;
  }
}

void printMenu(unsigned char selectedItem, unsigned char itemsAmt, const char * stringsPtr[]){
  unsigned char off;
  if(selectedItem > itemsAmt){    //calculate offset
    off = selectedItem - itemsAmt;
  }else{
    off = 0;
  }

  const unsigned char TEXT_OFF  =   15;
  const unsigned char TEXT_HEIGHT = 16;
  const unsigned char TEXT_START_HEIGHT = 8;
  const unsigned char SYMBOL_WIDTH = 12;

  unsigned char selectorPos;
  unsigned char textLen;
  char str[10];
 
  display.clearDisplay();
  display.setTextColor(WHITE);  //draw items list, without active item
  display.setTextSize(2);

  for(int i = 0; i < itemsAmt; i++){
    display.setCursor(TEXT_OFF, TEXT_START_HEIGHT + (TEXT_HEIGHT * i)); //moving down cursor
    strcpy_P(str, stringsPtr[i+off]);
    display.print(str);
  }

  if(selectedItem > 0){
    selectedItem -= 1;
    selectedItem -= off;
    selectorPos = TEXT_HEIGHT * selectedItem;
    textLen = strlen_P(stringsPtr[selectedItem+off]);   //draw selecting rectangle
    display.fillRect(TEXT_OFF - 1, (TEXT_START_HEIGHT + selectorPos - 1), textLen * SYMBOL_WIDTH ,  TEXT_HEIGHT, WHITE);//x0, y0, w, h, COLOR
    display.setTextColor(BLACK);
    strcpy_P(str, stringsPtr[selectedItem+off]);
    display.setCursor(TEXT_OFF,TEXT_START_HEIGHT + selectorPos);  //draw text into selector
    display.print(str);
  }
  display.display();
}

void calculate(){
  static unsigned char init = 0;
  switch(AnswerHeader.cmd){  //1 8 10 14
    case 0x31: // 1
      D.curh = abs(S25C31.current) / 100;     //current
      D.curl = abs(S25C31.current) % 100;
      D.remCapacity = S25C31.remainCapacity;
      D.remPercent= S25C31.remainPercent;
      D.temp1 = S25C31.temp1 - 20;
      D.temp2 = S25C31.temp2 - 20;
      D.voltage = S25C31.voltage;
      //D.volth = S25C31.voltage / 100;
      //D.voltl = S25C31.voltage % 100;
      D.spentPercent  = abs(D.initialPercent  - D.remPercent);
      D.spentCapacity = abs(D.initialCapacity - D.remCapacity);
      break;
    case 0xB0: // 8 speed average mileage poweron time
      D.sph = abs(S23CB0.speed) / 1000;       //speed
      D.spl = abs(S23CB0.speed) % 1000 / 10;
      D.milh = S23CB0.mileageCurrent / 100;   //mileage
      D.mill = S23CB0.mileageCurrent % 100;
      D.milTotH = S23CB0.mileageTotal / 1000; //mileage total
      D.milTotL = S23CB0.mileageTotal % 10;
      D.aveh = S23CB0.averageSpeed / 1000;
      D.avel = S23CB0.averageSpeed % 1000;
      break;
    case 0x3A: //10
      D.tripMin  = S23C3A.ridingTime  / 60;     //riding time
      D.tripSec  = S23C3A.ridingTime  % 60;
      D.powerMin = S23C3A.powerOnTime / 60;   //power on time
      D.powerSec = S23C3A.powerOnTime % 60;
      break;
    case 0x40: //cellsa
      break;
    default:
      break;
  }

  if(init == 0){ //initialise vars, only once after power up and get data
    if(D.remCapacity != 0 && D.remPercent != 0){
      D.initialPercent = D.remPercent;
      D.initialCapacity = D.remCapacity;
      init = 1;
    }
  }
}

void keyProcessFSM(){ //track throttle and brake and post messages (MENU, TH, BR, BOTH, LONG_TH, LONG_BR)
  enum {NO = 0, BR = 1, TH = 2, BOTH = 3};  
  static unsigned char step = 0;
  static unsigned long timer = millis();
  static unsigned char key = NO, _key;
  
  if(D.sph > 0){        //keys operates if speed < 1 km/h only
    step = 0;
  }
  
  if(D.br >= BR_KEY_TRES){ //detect keys
    key |= BR;
  }else{
    key &= ~BR;
  }
  if(D.th >= TH_KEY_TRES){
    key |= TH;
  }else{
    key &= ~TH;
  }

  switch(step){
    case 0:             //waiting for release both keys
      if(D.sph == 0 &&  key == NO){
        if(millis() - timer >= MENU_INITIAL){
          _key = NO;
          step = 1;
        }
      }else{
        timer = millis();
      }
      break;
    case 1:             //waiting for any key
      if(key != NO){    //depressed any key - go into next step
        timer = millis();
        step = 2;
      }
      break;
    case 2:             //wait for release both keys
      if(key != NO){
        _key |= key;
      }
      if(millis() - timer >= LONG_PRESS){
        switch(_key){
          case BOTH:
            Message.Post(MESSAGE_KEY_MENU);
            break;
          case TH:
            Message.Post(MESSAGE_KEY_TH_LONG);
            break;
          case BR:
            Message.Post(MESSAGE_KEY_BR_LONG);
            break;
        }
        step = 0;
        break;
      }
      if(key == NO){    //it`s short press
        switch(_key){
          case TH:
            Message.Post(MESSAGE_KEY_TH);
            break;
          case BR:
            Message.Post(MESSAGE_KEY_BR);
            break;
          case BOTH:
            Message.Post(MESSAGE_KEY_BOTH);
            break;
        }
        step = 0;
        break;
      }
  }
}

void displayRoutine(unsigned char var){
  const unsigned char textP = 60;
  const unsigned char oneDigOffset = 13;

  if(var == MENU){
    switch(_Menu.activeMenu){
      case 0: //main
        printMenu(_Menu.selItem, 3, menuMainItems);
        break;
      case 1: //recup
        printMenu(_Menu.selItem, 3, menuRecupItems);
        break;
      case 2: //cruise
        printMenu(_Menu.selItem, 2, menuOnOffItems);
        break;
      case 3: //led
        printMenu(_Menu.selItem, 2, menuOnOffItems);
        break;
      case 4: //big
        printMenu(_Menu.selItem, 3, menuBigItems);
        break;
      default:
        break;
    }
    return;
  }
  
  display.setTextColor(WHITE);
  display.clearDisplay();

  switch(var){
    case HIBERNATE:
      display.setFont(NULL);
      display.setTextSize(2);
      display.setCursor(0,0);
      display.println(F("HIBERNATE"));
      display.print(F("Lt:"));
      display.println(D.loopsTime);
      display.print(F("TH:"));
      display.println(D.th);
      display.print(F("BR:"));
      display.println(D.br);
      break;
    case BIG_TIME:
      unsigned int tTime;
      tTime  = D.powerMin * 100;
      tTime += D.powerSec;
      printBig(tTime, dispBp[5]);
      break;   
    case BIG_VOLTS:
      printBig(S25C31.voltage, dispBp[0]);
      break;
    case BIG_AVERAGE:
      printBig(S23CB0.averageSpeed / 10, dispBp[1]);
      break;
    case BIG_CURRENT:                               //-- BIG CURRENT
      printBig(S25C31.current, dispBp[2]);
      break;
    case BIG_MILEAGE:                              //-- BIG MILEAGE
      printBig(S23CB0.mileageCurrent, dispBp[3]);
      break;
    case BIG_SPEED:                                 //-- BIG SPEED
      printBig(S23CB0.speed / 10, dispBp[4]);
      break;

    case CELLS:
      display.setFont(NULL);
      display.setTextSize(1);
      display.setCursor(0,0);
      unsigned char counter;
      int v;
      counter = 0;
      int * ptr;
      int * ptr2;
      ptr = (int*)&S25C40;
      ptr2 = ptr + 5;
      for(int i = 0; i < 5; i++){         //print cells voltages
        counter ++;
        display.print(counter);           //column 1
        display.print(": ");
        v = *ptr / 1000;
        display.print(v);
        display.print('.');
        v = *ptr % 1000;
        if(v < 100){
          display.print('0');
        }
        if(v < 10){
          display.print('0');
        }
        display.print(v);
        
        if(counter < 5){
          display.print(F("   "));
        }else{
          display.print(F("  "));
        }
        display.print(counter + 5);       //column 2
        display.print(": ");
        display.print((int)(*ptr2 / 1000));
        display.print('.');
        v = *ptr2 % 1000;
        if(v < 100){
          display.print('0');
        }
        if(v < 10){
          display.print('0');
        }
        display.print((int)v);
        display.println();
        ptr++;
        ptr2++;
      }

      display.println();
      v = D.voltage / 100;          //print battery voltage
      display.print  ("U: "); 
      if(v < 100){
        display.print(' ');
      }
      display.print  (v);
      display.print  ('.');
      v = D.voltage % 100;
      if(v < 10){
        display.print('0');
      }
      display.println(v);
      
      display.print(F("t1:"));         //print battery temperatures
      display.print((int)D.temp1);
      display.print(F(" t2:"));
      display.print((int)D.temp2);
      
      //display.print(F(" Lt:"));
      //display.print((unsigned int)D.loopsTime);

      display.setCursor(90, 40); //84 one dig 6 pix wide
      display.print(F("TH:"));
      display.print(D.th);

      display.setCursor(90, 48);
      display.print(F("BR:"));
      display.print(D.br);

      display.setCursor(90, 56);
      display.print(F("LT:"));
      display.print((unsigned int)D.loopsTime);
      break;
    case CHARGING:
      display.setFont(NULL);
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("I:  ");
      if(D.curh < 10){
        display.print(' ');;
      }
      display.print(D.curh);
      display.print('.');
      if(D.curl < 10){
        display.print('0');
      }
      display.print(D.curl);
      display.println("A");
      //--------------------------
      display.print(F("Cap:  "));
      if(D.remCapacity < 1000){
        display.print(' ');
      }
      if(D.remCapacity < 100){
        display.print(' ');
      }
      if(D.remCapacity < 10){
        display.print(' ');
      }
      display.println(D.remCapacity);
      //--------------------------
      unsigned char left;       //how much percentage not enough to full charge
      left = 100 - D.remPercent;
      display.print(F("Left: "));
      if(left < 100){
        display.print(' ');
      }
      if(left < 10){
        display.print(' ');
      }
      display.print(left);
      display.println('\%');
      //--------------------------
      /*
      display.print(D.th);
      display.print(' ');
      display.print(D.br);
      */
      display.print("Tim:");
      if(D.powerMin < 100){
        display.print(' '); 
      }
      if(D.powerMin < 10){
        display.print(' ');
      }
      display.print(D.powerMin);
      display.print(':');
      if(D.powerSec < 10){
        display.print('0');
      }
      display.print(D.powerSec);
      break;

    case BIG_SPENT:                                 //-- BIG SPENT
      display.setFont(&FONT);
      display.setTextSize(2);
      display.setCursor(18,52); 

      if(D.spentPercent < 10){
        display.setCursor(53,52);
      }
      display.print((int)D.spentPercent);
      
      display.setTextSize(1);
      display.setCursor(90,32); 
      display.print('\%');
      display.setFont(NULL);
      display.setTextSize(2);
      display.setCursor(92, 40);
      display.print(F("Spn"));
      break;

    case NONE: //U can`t touch this :)
      return;
      break;
    case BATT: //not moving + TH
      display.setTextSize(1);    //microtext
      display.setCursor(10,0); 
      display.print(F("Spent"));
      display.setCursor(10,7); 
      display.print(F("perc"));
      
      display.setCursor(textP + oneDigOffset, 0);  
      display.setTextSize(2);
      if(D.spentPercent < 100){
        display.print(' ');
      }
      if(D.spentPercent < 10){
        display.print(' ');
      }
      display.print((int)D.spentPercent);
      display.print('\%');


      display.setTextSize(1);       //microtext
      display.setCursor(10,16); 
      display.print(F("Spent"));
      display.setCursor(10,23); 
      display.print(F("cap"));
      
      display.setCursor(textP + oneDigOffset, 16);
      display.setTextSize(2);
      if(D.spentCapacity < 1000){
        display.print(' ');
      }
      if(D.spentCapacity < 100){
        display.print(' ');
      }
      if(D.spentCapacity < 10){
        display.print(' ');
      }
      display.print(D.spentCapacity);


      display.setTextSize(1);       //microtext
      display.setCursor(10,32); 
      display.print(F("Curr"));
      display.setCursor(10,39); 
      display.print(F("perc"));
      display.setCursor(textP +(oneDigOffset) , 32); //
      display.setTextSize(2);
      if(D.remPercent < 100){
        display.print(' ');
      }
      if(D.remPercent < 10){
        display.print(' ');
      }
      display.print((int)D.remPercent);
      display.print('\%');


      display.setTextSize(1);       //microtext
      display.setCursor(10,48); 
      display.print(F("Curr"));
      display.setCursor(10,56); 
      display.print(F("cap"));

      display.setCursor(textP + oneDigOffset, 48);
      display.setTextSize(2);
      if(D.remCapacity < 1000){
        display.print(' ');
      }
      if(D.remCapacity < 100){
        display.print(' ');
      }
      if(D.remCapacity < 10){
        display.print(' ');
      }
      display.print((int)D.remCapacity);
      break;
    case TRIP: //stall + brake
      display.setTextSize(1);    //microtext
      display.setCursor(10,0); 
      display.print(F("Trip"));
      display.setCursor(10,7); 
      display.print(F("time"));
      
      display.setCursor(textP - oneDigOffset, 0);  //trip time
      display.setTextSize(2);
      if(D.tripMin < 100){
        display.print(' ');
      }
      if(D.tripMin < 10){
        display.print(' ');
      }
      display.print(D.tripMin);
      display.print(':');
      if(D.tripSec < 10){
        display.print('0');
      }
      display.print(D.tripSec);

      display.setTextSize(1);       //microtext
      display.setCursor(10,16); 
      display.print(F("Power"));
      display.setCursor(10,23); 
      display.print(F("time"));
      
      display.setCursor(textP - oneDigOffset, 16); //power-on time
      display.setTextSize(2);

      if(D.powerMin < 100){
        display.print(' ');
      }
      if(D.powerMin < 10){
        display.print(' ');
      }
      display.print(D.powerMin);
      display.print(':');
      if(D.powerSec < 10){
        display.print('0');
      }
      display.print(D.powerSec);


      display.setTextSize(1);       //microtext
      display.setCursor(10,32); 
      display.print(F("Trip"));
      display.setCursor(10,39); 
      display.print(F("mile"));

      display.setCursor(textP - (oneDigOffset * 2), 32); //trip mileage
      display.setTextSize(2);
      display.print(' ');
      if(D.milh < 100){
        display.print(' ');
      }
      if(D.milh < 10){
        display.print(' ');
      }
      display.print(D.milh);
      display.print('.');
      if(D.mill < 10){
        display.print('0');
      }
      display.print(D.mill);

      display.setTextSize(1);       //microtext
      display.setCursor(10,48); 
      display.print(F("Aver"));
      display.setCursor(10,56); 
      display.print(F("speed"));

      display.setCursor(textP - oneDigOffset, 48); //average speed
      display.setTextSize(2);
      display.print(' ');
      if(D.aveh < 10){
        display.print(' ');
      }
      display.print(D.aveh);
      display.print('.');
      if(D.avel < 10){
        display.print('0');
      }
      display.print(D.avel);

      
      /*
      if(D.milTotH < 10000){
        display.print(' ');
      }
      if(D.milTotH < 1000){
        display.print(' ');
      }
      if(D.milTotH < 100){
        display.print(' ');
      }
      if(D.milTotH < 10){
        display.print(' ');
      }
      display.print(D.milTotH);
      display.print('.');
      display.print(D.milTotL);
      */
      break;
    case MILEAGE:
      display.setFont(NULL);
      display.setTextSize(2);
      display.clearDisplay();
      display.setCursor(0,0);

      //speed    
      if(D.sph < 10){
        display.print(' ');
      }
      display.print(D.sph);
      display.print('.');
      display.print(D.spl);
      display.setTextSize(1);
      display.print(F("k/h "));

      //remain charge
      display.setTextSize(2);
      display.print(" ");
      if(D.remPercent < 100){
        display.print(' ');
      }
      if(D.remPercent < 10){
        display.print(' ');
      }
      display.print(D.remPercent);
      display.setTextSize(1);
      display.println('\%');

      //second row
      display.setTextSize(2);
      display.setCursor(0,16); //(x, y)

      //current mileage
      if(D.milh < 10){
        display.print(' ');
      }
      display.print(D.milh);
      display.print('.');
      if(D.mill < 10){
        display.print('0'); //leading zero
      }

      display.print(D.mill);
      display.setTextSize(1);
      display.print(F("km"));

      display.setTextSize(2);
      display.setCursor(96,16); //(x, y)
      if(D.spentPercent < 10){
        display.print(' ');
      }
      display.print(D.spentPercent);
      display.setTextSize(1);
      display.println('\%');

      //third row
      display.setTextSize(2);
      display.setCursor(0,32); //(x, y)

      //time
      if(D.tripMin < 10){
        display.print(' ');
      }
      display.print(D.tripMin);
      display.print(':');
      if(D.tripSec < 10){
        display.print('0');
      }
      display.print(D.tripSec);
      //four row
      display.setCursor(0,48); //(x, y)
      if(D.curh < 10){         //current
        display.print(' ');
      }
      display.print(D.curh);
      display.print('.');
      if(D.curl < 10){
        display.print('0');
      }
      display.print(D.curl);

      display.setTextSize(1);
      display.print('A');
      display.setTextSize(2);

      break;
      default:
        display.setFont(NULL);
        display.setTextSize(2);
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Select BIG"));
        display.println(F("into menu"));
        break;
  }

  display.display();

}

void printBig(int n1, const char * str){ // n1 format: 1234 will be printed as 12.34
  int h;
  int l;
  char strl[5];
  strcpy_P(strl, str);
  h = abs(n1 / 100);
  l = abs(n1 % 100);
  display.clearDisplay();
  display.setFont(&FONT);
  display.setTextSize(2);
  if(n1 < 0){
    display.setCursor(0,45); 
    display.print('-');
  }
  display.setCursor(18,52); 
  if(h < 10){
    display.setCursor(53,52);
  }
  display.print(h);
      
  display.setTextSize(1); //mini text
  display.setCursor(90,32);
  if(l < 10){
    display.print('0');
  }
  display.print(l);
  display.setFont(NULL);
  display.setTextSize(2);
  display.setCursor(92, 40);
  display.print(strl);
  display.display();
}

void dataFSM(){
  static unsigned char   step = 0, _step = 0, entry = 1;
  static unsigned long   beginMillis;
  static unsigned char   Buf[RECV_BUFLEN];
  static unsigned char * _bufPtr;
  _bufPtr = (unsigned char*)&Buf;

  switch(step){
    case 0:                                                             //search header sequence
      while(XIAOMI_PORT.available() >= 2){
        if(XIAOMI_PORT.read() == 0x55 && XIAOMI_PORT.peek() == 0xAA){
          XIAOMI_PORT.read();                                           //discard second part of header
          step = 1;
          break;
        }
      }
      break;
    case 1: //preamble complete, receive body
      static unsigned char   readCounter;
      static unsigned int    _cs;
      static unsigned char * bufPtr;
      static unsigned char * asPtr; //
      unsigned char bt;
      if(entry){      //init variables
        memset((void*)&AnswerHeader, 0, sizeof(AnswerHeader));
        bufPtr = _bufPtr;
        readCounter = 0;
        beginMillis = millis();
        asPtr   = (unsigned char *)&AnswerHeader;   //pointer onto header structure
        _cs = 0xFFFF;
      }
      if(readCounter >= RECV_BUFLEN){               //overrun
        step = 2;
        break;
      }
      if(millis() - beginMillis >= RECV_TIMEOUT){   //timeout
        step = 2;
        break;
      }

      while(XIAOMI_PORT.available()){               //read available bytes from port-buffer
        bt = XIAOMI_PORT.read();
        readCounter++;
        if(readCounter <= sizeof(AnswerHeader)){    //separate header into header-structure
          *asPtr++ = bt;
          _cs -= bt;
        }
        if(readCounter > sizeof(AnswerHeader)){     //now begin read data
          *bufPtr++ = bt;
          if(readCounter < (AnswerHeader.len + 3)){
            _cs -= bt;
          }
        }
        beginMillis = millis();                     //reset timeout
      }

      if(AnswerHeader.len == (readCounter - 4)){    //if len in header == current received len
        unsigned int   cs;
        unsigned int * ipcs;
        ipcs = (unsigned int*)(bufPtr-2);
        cs = *ipcs;
        if(cs != _cs){   //check cs
          step = 2;
          break;
        }
        //here cs is ok, header in AnswerHeader, data in _bufPtr
        processPacket(_bufPtr, readCounter);

        step = 2;
        break;
      }
      break; //case 1:
    case 2:  //end of receiving data
      step = 0;
      break;
  }

  if(_step != step){
    _step = step;
    entry = 1;
  }else{
    entry = 0;
  }
}
void processPacket(unsigned char * data, unsigned char len){
  unsigned char RawDataLen;
  //unsigned char unknownPacket = 0;
  RawDataLen = len - sizeof(AnswerHeader) - 2;//(crc)

  switch(AnswerHeader.addr){ //store data into each other structure
    case 0x20: //0x20
      switch(AnswerHeader.cmd){
        case 0x00:
          switch(AnswerHeader.hz){
            case 0x64: //BLE ask controller
              break;
            case 0x65:
              if(_Query.prepared == 1 && _Hibernate == 0){
                writeQuery();
              }
              memcpy((void*)& S20C00HZ65, (void*)data, RawDataLen);
              D.th = S20C00HZ65.throttle;
              D.br = S20C00HZ65.brake;
              break;
            default: //no suitable hz
              break;
          }
          break;
        case 0x1A:
          break;
        case 0x69:
          break;
        case 0x3E:
          break;
        case 0xB0:
          break;
        case 0x23:
          break;
        case 0x3A:
          break;
        case 0x7C:
          break;
        default: //no suitable cmd
          break;
      }
      break;
    case 0x21:
      switch(AnswerHeader.cmd){
        case 0x00:
        switch(AnswerHeader.hz){
          case 0x64: //answer to BLE
            memcpy((void*)& S21C00HZ64, (void*)data, RawDataLen);
            break;
          }
          break;
      default:
        break;
      }
      break;
    case 0x22:
      switch(AnswerHeader.cmd){
        case 0x3B:
          break;
        case 0x31:
          break;
        case 0x20:
          break;
        case 0x1B:
          break;
        case 0x10:
          break;
        default:
          break;
      }
      break;
    case 0x23:
      //_NewDataFlag = 1;
      switch(AnswerHeader.cmd){
        case 0x17:
          break;
        case 0x1A:
          //if(RawDataLen == sizeof(A23C1A)){
          //  memcpy((void*)& S23C1A, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23C1A_OK");
          //}
          break;
        case 0x69:
          //if(RawDataLen == sizeof(A23C69)){
          //  memcpy((void*)& S23C69, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23C69_OK");
          //}
          break;
        case 0x3E: //mainframe temperature
          if(RawDataLen == sizeof(A23C3E)){
            memcpy((void*)& S23C3E, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23C3E_OK");
          }
          break;
        case 0xB0: //speed, average speed, mileage total, mileage current, power on time, mainframe temp
          if(RawDataLen == sizeof(A23CB0)){
            memcpy((void*)& S23CB0, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23CB0_OK");
          }
          break;
        case 0x23: //remain mileage
          if(RawDataLen == sizeof(A23C23)){
            memcpy((void*)& S23C23, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23C23_OK");
          }
          break;
        case 0x3A: //power on time, riding time
          if(RawDataLen == sizeof(A23C3A)){
            memcpy((void*)& S23C3A, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A23C3A_OK");
          }
          break;
        case 0x7C:
          //if(RawDataLen == sizeof(A23C7C)){
            //memcpy((void*)& S23C7C, (void*)data, RawDataLen);
          //}
          break;
        case 0x7B:
          //if(RawDataLen == sizeof(A23C7B)){
            //memcpy((void*)& S23C7B, (void*)data, RawDataLen);
          //}
          break;
        case 0x7D:
          //if(RawDataLen == sizeof(A23C7D)){
            //memcpy((void*)& S23C7D, (void*)data, RawDataLen);
          //}
          break;
        default:
          break;
      }
      break;          
    case 0x25:
      switch(AnswerHeader.cmd){
        case 0x40: //cells info
          if(RawDataLen == sizeof(A25C40)){
            memcpy((void*)& S25C40, (void*)data, RawDataLen);
          }
          break;
        case 0x3B:
          //if(RawDataLen == sizeof(A25C3B)){
          //  memcpy((void*)& S25C3B, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A25C3B_OK");
          //}
          break;
        case 0x31: //capacity, remain persent, current, voltage
          if(RawDataLen == sizeof(A25C31)){
            memcpy((void*)& S25C31, (void*)data, RawDataLen);
            if(D.initialPercent == 0){
              D.initialPercent  = S25C31.remainPercent;
              D.initialCapacity = S25C31.remainCapacity;
            }else{
              D.spentPercent  = D.initialPercent  - S25C31.remainPercent; //calculate spent energy
              D.spentCapacity = D.initialCapacity - S25C31.remainCapacity;
            }
            //DEBUG_PORT.println("A25C31_OK");
          }
          break;
        case 0x20:
          //if(RawDataLen == sizeof(A25C20)){
          //  memcpy((void*)& S25C20, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A25C20_OK");
          //}
          break;
        case 0x1B:
          //_Hibernate = 1;
          //if(RawDataLen == sizeof(A25C1B)){
          //  memcpy((void*)& S25C1B, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A25C1B_OK");
          //}
          break;
        case 0x10:
          //if(RawDataLen == sizeof(A25C10)){
          //  memcpy((void*)& S25C10, (void*)data, RawDataLen);
            //DEBUG_PORT.println("A25C10_OK");
          //}
          break;
        default:
          break;
        }
        break;
      default:
        break;
  }


  for(unsigned char i = 0; i < sizeof(_commandsWeWillSend); i++){
    if(AnswerHeader.cmd == _q[_commandsWeWillSend[i]]){
      _NewDataFlag = 1;
      break;
    }
  }
}

void prepareNextQuery(){
  static unsigned char index = 0;
  switch(_Menu.dispVar){
    case CHARGING:
      _Query._dynQueries[0]=1;
      _Query._dynQueries[1]=8;
      _Query._dynQueries[2]=10;
      _Query._dynSize = 3;
      break;
    case BIG_CURRENT:
      _Query._dynQueries[0] = 1;
      _Query._dynQueries[1] = 8;
      _Query._dynSize = 2;
      break;
    case BIG_SPEED:   
      _Query._dynQueries[0] = 8;
      _Query._dynSize = 1;
      break;
    case TRIP:
      _Query._dynQueries[0] =  1;
      _Query._dynQueries[1] =  8;
      _Query._dynQueries[2] = 10;
      _Query._dynSize = 3;
      break;
    case BATT:
      _Query._dynQueries[0] =  1;
      _Query._dynQueries[1] =  8;
      _Query._dynSize = 2;
      break;
    case MENU:
      break;
    case CELLS:
      _Query._dynQueries[0] =  1;
      _Query._dynQueries[1] =  8;
      _Query._dynQueries[2] = 14;
      _Query._dynSize = 3;
      break;
    default:
      _Query._dynQueries[0]=1;
      _Query._dynQueries[1]=8;
      _Query._dynQueries[2]=10;
      _Query._dynSize = 3;
      break;
  }


  if(preloadQueryFromTable(_Query._dynQueries[index]) == 0){
    _Query.prepared = 1;
  }
  index++;
  if(index >= _Query._dynSize){
    index = 0;
  }  
}
unsigned char preloadQueryFromTable(unsigned char index){
  unsigned char * ptrBuf;
  unsigned char * pp; //pointer preamble
  unsigned char * ph; //pointer header
  unsigned char * pe; //pointer end

  unsigned char cmdFormat;
  unsigned char hLen; //header length
  unsigned char eLen; //ender length

  if(index >= sizeof(_q)){  //unknown index
    return 1;
  }

  if(_Query.prepared != 0){ //if query not send yet
    return 2;
  }

  cmdFormat = pgm_read_byte_near(_f + index);

  pp = (unsigned char*)&_h0;
  ph = NULL;
  pe = NULL;

  switch(cmdFormat){
    case 1: //h1 only
      ph = (unsigned char*)&_h1;
      hLen = sizeof(_h1);
      pe = NULL;
      break;
    case 2: //h2 + end20t
      ph = (unsigned char*)&_h2;
      hLen = sizeof(_h2);

      //copies last known throttle & brake values
      _end20t.hz = 0x02;
      _end20t.th = S20C00HZ65.throttle;
      _end20t.br = S20C00HZ65.brake;
      pe = (unsigned char*)&_end20t;
      eLen = sizeof(_end20t);
      break;
  }

  ptrBuf = (unsigned char*)&_Query.buf;

  memcpy_P((void*)ptrBuf, (void*)pp, sizeof(_h0));  //copy preamble
  ptrBuf += sizeof(_h0);

  memcpy_P((void*)ptrBuf, (void*)ph, hLen);         //copy header
  ptrBuf += hLen;
  
  memcpy_P((void*)ptrBuf, (void*)(_q + index), 1);  //copy query
  ptrBuf++;
  
  memcpy_P((void*)ptrBuf, (void*)(_l + index), 1);  //copy expected answer length
  ptrBuf++;
/*
  if(pe != NULL){
    memcpy_P((void*)ptrBuf, (void*)pe, eLen);       //if needed - copy ender
    ptrBuf+= hLen;
  }
*/
  if(pe != NULL){
    memcpy((void*)ptrBuf, (void*)pe, eLen);       //if needed - copy ender
    ptrBuf+= hLen;
  }

  //unsigned char 
  _Query.DataLen = ptrBuf - (unsigned char*)&_Query.buf[2]; //calculate length of data in buf, w\o preamble and cs

  _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);    //calculate cs of buffer
  return 0;
}
void prepareCommand(unsigned char cmd){
  unsigned char * ptrBuf;

  _cmd.len  =    4;
  _cmd.addr = 0x20;
  _cmd.rlen = 0x03;

  switch(cmd){
    case CMD_CRUISE_ON:   //0x7C, 0x01, 0x00
      _cmd.param = 0x7C;
      _cmd.value =    1;
      break;
    case CMD_CRUISE_OFF:  //0x7C, 0x00, 0x00
      _cmd.param = 0x7C;
      _cmd.value =    0;
      break;
    case CMD_LED_ON:      //0x7D, 0x02, 0x00
      _cmd.param = 0x7D;  
      _cmd.value =    2;
      break;
    case CMD_LED_OFF:     //0x7D, 0x00, 0x00
      _cmd.param = 0x7D;
      _cmd.value =    0;
      break;
    case CMD_WEAK:        //0x7B, 0x00, 0x00
      _cmd.param = 0x7B;
      _cmd.value =    0;
      break;
    case CMD_MEDIUM:      //0x7B, 0x01, 0x00
      _cmd.param = 0x7B;
      _cmd.value =    1;
      break;
    case CMD_STRONG:      //0x7B, 0x02, 0x00
      _cmd.param = 0x7B;
      _cmd.value =    2;
      break;
    default:
      return; //undefined command - do nothing
      break;
  }
  ptrBuf = (unsigned char*)&_Query.buf;

  memcpy_P((void*)ptrBuf, (void*)_h0, sizeof(_h0));  //copy preamble
  ptrBuf += sizeof(_h0);

  memcpy((void*)ptrBuf, (void*)&_cmd, sizeof(_cmd)); //copy command body
  ptrBuf += sizeof(_cmd);

  //unsigned char 
  _Query.DataLen = ptrBuf - (unsigned char*)&_Query.buf[2];               //calculate length of data in buf, w\o preamble and cs
  _Query.cs = calcCs((unsigned char*)&_Query.buf[2], _Query.DataLen);     //calculate cs of buffer

  _Query.prepared = 1;
}

void writeQuery(){
  RX_DISABLE;
  XIAOMI_PORT.write((unsigned char*)&_Query.buf, _Query.DataLen + 2);     //DataLen + length of preamble
  XIAOMI_PORT.write((unsigned char*)&_Query.cs, 2);
  RX_ENABLE;
  _Query.prepared = 0;
}

unsigned int calcCs(unsigned char * data, unsigned char len){
  unsigned int cs = 0xFFFF;
  for(int i = len; i > 0; i--){
    cs -= *data++;
  }
  return cs;
}
/*
void printErrno(unsigned char err){
    display.clearDisplay();
    display.setFont(NULL);
    display.setCursor(0,0);
    display.print(err);
    display.display();
    delay(200);
}
*/
