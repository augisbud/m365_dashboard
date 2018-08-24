#define Language ENG

#if Language == ENG
  #define defaultFont System5x7mod

  const char noBUS1[] PROGMEM = {"BUS not"};
  const char noBUS2[] PROGMEM = {"connected!"};
  const char noBUS3[] PROGMEM = {"No data to"};
  const char noBUS4[] PROGMEM = {"display!"};

  const char confScr1[] PROGMEM = {"Big speedometer: "};
  const char confScr2[] PROGMEM = {"Big speedo.: "};
  const char confScr2a[] PROGMEM = {"  SPEED"};
  const char confScr2b[] PROGMEM = {"CURRENT"};
  const char confScr3[] PROGMEM = {"Battery warning: "};
  const char confScr4[] PROGMEM = {"Big batt. warn.: "};
  const char confScr5[] PROGMEM = {"Battery info"};
  const char confScr6[] PROGMEM = {"Configure M365"};
  const char confScr7[] PROGMEM = {"Save and exit"};

  const char infoScr1[] PROGMEM = {"Total distance"};
  const char infoScr2[] PROGMEM = {"Power on time"};

  const char battScr[] PROGMEM = {">>> Brake to exit <<<"};

  const char M365CfgScr1[] PROGMEM = {"Cruise control: "};
  const char M365CfgScr2[] PROGMEM = {"Update Cruise"};
  const char M365CfgScr3[] PROGMEM = {"Tailight on:    "};
  const char M365CfgScr4[] PROGMEM = {"Update Tailight"};
  const char M365CfgScr5[] PROGMEM = {"KERS:        "};
  const char M365CfgScr6[] PROGMEM = {"Update KERS"};
  const char M365CfgScr7[] PROGMEM = {"Exit"};

  const char l_Weak[] PROGMEM =   {"  WEAK"};
  const char l_Medium[] PROGMEM = {"MEDIUM"};
  const char l_Strong[] PROGMEM = {"STRONG"};

  const char l_Yes[] PROGMEM = {"YES"};
  const char l_No[] PROGMEM =  {" NO"};
  const char l_On[] PROGMEM =  {" ON"};
  const char l_Off[] PROGMEM = {"OFF"};

  const char l_km[] PROGMEM = {"km"};
  const char l_kmh[] PROGMEM = {"km/h"};
  const char l_mah[] PROGMEM = {"mAh"};
  const char l_v[] PROGMEM = {"V"};
  const char l_a[] PROGMEM = {"A"};
  const char l_c[] PROGMEM = {"C"};
  const char l_t[] PROGMEM = {"T"};
#endif
