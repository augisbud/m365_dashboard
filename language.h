//***********************************
//LANGUAGE SELECTION. Uncomment only one line
//***********************************
#define LANG_ENG //English
//#define LANG_FRA //French
//#define LANG_RU //Russian, use together with the russian font in libraries folder
//#define LANG_ES //Spanish

#ifdef LANG_ENG
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
  const char M365CfgScr8[] PROGMEM = {"Exit"};
  const char M365CfgScr7[] PROGMEM = {"Wheel Size:     "};

  const char l_85inch[] PROGMEM = {" 8,5\""};
  const char l_10inch[] PROGMEM = {"  10\""};

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
  
  #ifdef LANG_FRA
  #define defaultFont System5x7mod

  const char noBUS1[] PROGMEM = {"BUS non"};
  const char noBUS2[] PROGMEM = {"connecté!"};
  const char noBUS3[] PROGMEM = {"Pas de données"};
  const char noBUS4[] PROGMEM = {"à afficher!"};

  const char confScr1[] PROGMEM = {"Gros speedométre: "};
  const char confScr2[] PROGMEM = {"Gros speedo.: "};
  const char confScr2a[] PROGMEM = {" VITESSE"};
  const char confScr2b[] PROGMEM = {"ACTUELLE"};
  const char confScr3[] PROGMEM = {"Alerte Batterie: "};
  const char confScr4[] PROGMEM = {"Alerte Batt.: "};
  const char confScr5[] PROGMEM = {"Informations batterie"};
  const char confScr6[] PROGMEM = {"Configure M365"};
  const char confScr7[] PROGMEM = {"Enregistrer et quitter"};

  const char infoScr1[] PROGMEM = {"Distance Totale"};
  const char infoScr2[] PROGMEM = {"Allumée depuis :"};

  const char battScr[] PROGMEM = {">>> Freiner pour sortir <<<"};

  const char M365CfgScr1[] PROGMEM = {"Contrôle régulateur: "};
  const char M365CfgScr2[] PROGMEM = {"MAJ Régulateur"};
  const char M365CfgScr3[] PROGMEM = {"Allumer feu arrière:    "};
  const char M365CfgScr4[] PROGMEM = {"Maj feu arrière"};
  const char M365CfgScr5[] PROGMEM = {"KERS:        "};
  const char M365CfgScr6[] PROGMEM = {"Maj KERS"};
  const char M365CfgScr8[] PROGMEM = {"Sortir"};
  const char M365CfgScr7[] PROGMEM = {"Wheel Size:     "};

  const char l_85inch[] PROGMEM = {" 8,5\""};
  const char l_10inch[] PROGMEM = {"  10\""};

  const char l_Weak[] PROGMEM =   {"  LEGER"};
  const char l_Medium[] PROGMEM = {"  MOYEN"};
  const char l_Strong[] PROGMEM = {"  DUR"};

  const char l_Yes[] PROGMEM = {"   OUI"};
  const char l_No[] PROGMEM =  {"   NON"};
  const char l_On[] PROGMEM =  {"Allumé"};
  const char l_Off[] PROGMEM = {"Eteint"};

  const char l_km[] PROGMEM = {"km"};
  const char l_kmh[] PROGMEM = {"km/h"};
  const char l_mah[] PROGMEM = {"mAh"};
  const char l_v[] PROGMEM = {"V"};
  const char l_a[] PROGMEM = {"A"};
  const char l_c[] PROGMEM = {"C"};
  const char l_t[] PROGMEM = {"T"};
  #endif
  
  #ifdef LANG_RU
  #define defaultFont System5x7mod

  const char noBUS1[] PROGMEM = {"Ytn"};
  const char noBUS2[] PROGMEM = {"Cdzpb"};
  const char noBUS3[] PROGMEM = {"c"};
  const char noBUS4[] PROGMEM = {"cfvjrfnjv!"};
  
  const char confScr1[] PROGMEM = {"Cgbljvtnh: "};
  const char confScr2[] PROGMEM = {"Ht;.Cgbljv.:"};
  const char confScr2a[] PROGMEM = {"CRJHJCNM"};
  const char confScr2b[] PROGMEM = {"  NJR   "};
  const char confScr3[] PROGMEM = {"Ghtleght;ltybt:"};
  const char confScr4[] PROGMEM = {"Ghtl. ?fnfhtb: "};
  const char confScr5[] PROGMEM = {"Bya. J ?fnfhtb"};
  const char confScr6[] PROGMEM = {"Yfcnhjqrf"};
  const char confScr7[] PROGMEM = {"Cj[hfybnm B Dsqnb"};

  const char infoScr1[] PROGMEM = {"Ghj,tu"};
  const char infoScr2[] PROGMEM = {"Dhtvz hf,jns"};

  const char battScr[] PROGMEM = {">>Njhvjp lkz ds[jlf<<"};

  const char M365CfgScr1[] PROGMEM = {"Rhebp rjynhjkm: "};
  const char M365CfgScr2[] PROGMEM = {">>J,yjdbnm rhebp<<"};
  const char M365CfgScr3[] PROGMEM = {"Pflybq Ajyfhm: "};
  const char M365CfgScr4[] PROGMEM = {">>J,yjdbnm Ajyfhm<<"};
  const char M365CfgScr5[] PROGMEM = {"Htregthfwbz: "};
  const char M365CfgScr6[] PROGMEM = {">>J,yjdbnm htr.<<"};
  const char M365CfgScr8[] PROGMEM = {"--------Dsqnb-------"};
  const char M365CfgScr7[] PROGMEM = {"Hfpvth rjktcf:  "};
  
  const char l_85inch[] PROGMEM = {"8.5\""};
  const char l_10inch[] PROGMEM = {" 10\""};

  const char l_Weak[] PROGMEM =   {" CKF,J"};
  const char l_Medium[] PROGMEM = {"CHTLYT"};
  const char l_Strong[] PROGMEM = {"CBKMYJ"};

  const char l_Yes[] PROGMEM = {" LF"};
  const char l_No[] PROGMEM =  {"YTN"};
  const char l_On[] PROGMEM =  {" DRK"};
  const char l_Off[] PROGMEM = {"DSRK"};

  const char l_km[] PROGMEM = {"RV"};
  const char l_kmh[] PROGMEM = {"RV/X"};
  const char l_mah[] PROGMEM = {"vFx"};
  const char l_v[] PROGMEM = {"D"};
  const char l_a[] PROGMEM = {"F"};
  const char l_c[] PROGMEM = {"C"};
  const char l_t[] PROGMEM = {"N"};
  #endif
  
  #ifdef LANG_ES
  #define defaultFont System5x7mod

  const char noBUS1[] PROGMEM = {"BUS no"};
  const char noBUS2[] PROGMEM = {"conectado"};
  const char noBUS3[] PROGMEM = {"pantalla"};
  const char noBUS4[] PROGMEM = {"sin datos"};

  const char confScr1[] PROGMEM = {"Gran velocimet.: "};
  const char confScr2[] PROGMEM = {"Gran veloc.: "};
  const char confScr2a[] PROGMEM = {"    KMH"};
  const char confScr2b[] PROGMEM = {"ENERGIA"};
  const char confScr3[] PROGMEM = {"Alarma bateria : "};
  const char confScr4[] PROGMEM = {"Gran alar. bat.: "};
  const char confScr5[] PROGMEM = {"Informacion "};
  const char confScr6[] PROGMEM = {"Configurar M365"};
  const char confScr7[] PROGMEM = {"Grabar y salir"};

  const char infoScr1[] PROGMEM = {"Distancia total"};
  const char infoScr2[] PROGMEM = {"Tiempo encendido"};

  const char battScr[] PROGMEM = {">>> Freno para salir <<<"};

  const char M365CfgScr1[] PROGMEM = {"Control crucero: "};
  const char M365CfgScr2[] PROGMEM = {"Actualizar crucero"};
  const char M365CfgScr3[] PROGMEM = {"Luz trasera:    "};
  const char M365CfgScr4[] PROGMEM = {"Actual. luz tras."};
  const char M365CfgScr5[] PROGMEM = {"KERS:        "};
  const char M365CfgScr6[] PROGMEM = {"Actualizar KERS"};
  const char M365CfgScr8[] PROGMEM = {"Salir"};
  const char M365CfgScr7[] PROGMEM = {"Tipo de Rueda:  "};

  const char l_85inch[] PROGMEM = {" 8,5\""};
  const char l_10inch[] PROGMEM = {"  10\""};

  const char l_Weak[] PROGMEM =   {" DEBIL"};
  const char l_Medium[] PROGMEM = {" MEDIO"};
  const char l_Strong[] PROGMEM = {"FUERTE"};

  const char l_Yes[] PROGMEM = {" SI"};
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
