#if isMENU


#define MENUstMain   0
#define MENUstWifi   1
#define MENUstMqtt   2

#define MAX_NUM_CHARS 32 // maximum number of characters read from the Serial Monitor
char cmdSCI[MAX_NUM_CHARS];       // char[] to store incoming serial commands
boolean cmdSCI_complete = false;  // whether the command string is complete
uint8_t menuSCIst =0;

char cmdBT[MAX_NUM_CHARS];       // char[] to store incoming serial commands
boolean cmdBT_complete = false;  // whether the command string is complete
uint8_t menuBTst =0;


//======================================================
/*
const char MenuMainText[] PROGMEM = R"=====(
--------- Main:
ip         time;      reset;   t (temperature)
w (wifi)   m (mqtt)   u (user)
sr (smsread) 
mls (send email)      mlr (read email)
fsr1 (spiffs root)    fsr2 (spiffs all)
h (this help)
)=====";
*/
const char MenuMainText[] PROGMEM = R"=====(
--------- Main:
ip         time;      reset;
w (wifi)   m (mqtt)   u (user)
h (this help)
)=====";

void printMenuMain(Stream *S) {
  S->println((const __FlashStringHelper *)MenuMainText);
}
//======================================================
/*
const char MenuWifiText[] PROGMEM = R"=====(
--------- Wifi:
ssid=<...>  pasw=<...>  mDns=<...>
ip=<...>    mask=<...>  gate=<...>  dns=<...>
ntp=<...>
wifiON      wifiOFF     dhcpON      dhcpOFF
s (print setting)       r (back)    h (this help)
)=====";
*/
const char MenuWifiText[] PROGMEM = R"=====(
--------- Wifi:
ssid=<...>  pasw=<...>  mDns=<...>
ip=<...>    mask=<...>  gate=<...>  dns=<...>
ntp=<...>
dhcpON      dhcpOFF
s (print setting)       r (back)    h (this help)
)=====";

void printMenuWifi(Stream *S) {
  S->println((const __FlashStringHelper *)MenuWifiText);
}
//======================================================
const char MenuMQTTtext[] PROGMEM = R"=====(
--------- MQTT:
srv=<...>   Dname=<...>  Gname=<...>
user=<...>  pasw=<...>
s (print setting)        r (back)
h (this help)
)=====";

void printMenuMQTT(Stream *S) {
  S->println((const __FlashStringHelper *)MenuMQTTtext);
}
//======================================================






//#########################################################
void printSPIFFS_1(void);
void printSPIFFS_2(void);

bool MAINcommand(Stream *S, char *cmd, uint8_t *menust) {
  char answ[100];

  if (strcmp(cmd,"reset") == 0) {
      S->println("reset");
      ESP.restart();
      return false;
  }
  if (strcmp(cmd,"time") == 0) {
      printLocalTime(S);
      return true;
  }
  if (strcmp(cmd,"ip") == 0) {
      S->print("IP: ");
      S->println(WiFi.localIP().toString());
      return true;
  }
  if (strcmp(cmd,"mlr") == 0) {
    Serial.println(ESP.getFreeHeap());
    #if isIMAP
    readEmail();
    #elif isPOP3
    POP3forceRead=true;
    #endif
    return true;
  }
  if (strcmp(cmd,"mls") == 0) {
    Serial.println(ESP.getFreeHeap());
    email_send(SMTPrecipient,SMTPsubject,"test");
    return true;
  }
  if (strcmp(cmd,"fsr1") == 0) {
    Serial.println(ESP.getFreeHeap());
    printSPIFFS_1();
    return true;
  }
  if (strcmp(cmd,"fsr2") == 0) {
    printSPIFFS_2();
    return true;
  }
  if (strcmp(cmd,"sr") == 0) {
    SMSforceRead=true;
    return true;
  }
  if (strcmp(cmd,"w") == 0) {
    *menust=MENUstWifi;
    printMenuWifi(S);
    return true;
  }
  if (strcmp(cmd,"h") == 0) {
    printMenuMain(S);
    return true;
  }
  #if isMQTT
  if (strcmp(cmd,"m") == 0) {
        *menust=MENUstMqtt;
        printMenuMQTT(S);
        return true;
  }
  #endif
 
  #if isTEMPOW
  if (strcmp(cmd,"t") == 0) {
      sprintf(answ,"Teplota: %.1f C\n",Celcius);
      S->print(answ);
      return true;
  }
  #endif
  return false;
}
//======================================================
void avalSET1_print(Stream *S);

bool WIFIcommand(Stream *S, char *cmd, uint8_t *menust) {
  char answ[100];
  if (strcmp(cmd,"r") == 0) {
        *menust=MENUstMain;
        printMenuMain(S);
        return true;
  }
  if (strcmp(cmd,"h") == 0) {
      printMenuWifi(S);
      return true;
  }
  if (strcmp(cmd,"s") == 0) {
    avalSET1_print(S);
    return true;
  }
  /* !!!!
  if (strcmp(cmd,"wifiON") == 0) {
      WIFIenabled=true;
      prefs.begin(PREDVOLBY_LBMAIN, false);
      prefs.putBool("WIFIenabled", true);
      prefs.end();
      aval_EEsave(&TABVP_SET1[0], "MQT07");      

      S->println("WIFI enabled");
      return true;
  }
  if (strcmp(cmd,"wifiOFF") == 0) {
      WIFIenabled=false;
      prefs.begin(PREDVOLBY_LBMAIN, false);
      prefs.putBool("WIFIenabled", false);
      prefs.end();
      S->println("WIFI disabled");
      return true;
  }
  */
  if (strcmp(cmd,"dhcpOFF") == 0) {
      WIFIstatic=true;
      aval_EEsave(&TABVP_SET1[0], "NET09");      
      S->println("WIFIstaticON");
      return true;
  }
  if (strcmp(cmd,"dhcpON") == 0) {
      WIFIstatic=false;
      aval_EEsave(&TABVP_SET1[0], "NET09");      
      S->println("WIFIstaticOFF");
      return true;
  }

  if (strncmp(cmd,"ssid=",5) == 0) {
    sprintf(WIFIssid,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "NET01");      
    sprintf(answ,"Change WIFIssid to <%s>\n",WIFIssid);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"pasw=",5) == 0) {
    sprintf(WIFIpasw,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "NET02");      
    sprintf(answ,"Change WIFIpasw to <%s>\n",WIFIpasw);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"mDns=",5) == 0) {
    sprintf(WIFImdns,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "NET03");      
    sprintf(answ,"Change WIFImdns to <%s>\n",WIFImdns);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"ip=",3) == 0) {
    sprintf(WIFIip,"%s",cmd+3);
    aval_EEsave(&TABVP_SET1[0], "NET04");      
    sprintf(answ,"Change WIFIip to <%s>\n",WIFIip);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"mask=",5) == 0) {
    sprintf(WIFImask,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "NET05");      
    sprintf(answ,"Change WIFImask to <%s>\n",WIFImask);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"gate=",5) == 0) {
    sprintf(WIFIgate,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "NET06");      
    sprintf(answ,"Change WIFIgate to <%s>\n",WIFIgate);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"dns=",4) == 0) {
    sprintf(WIFIdns,"%s",cmd+4);
    aval_EEsave(&TABVP_SET1[0], "NET07");      
    sprintf(answ,"Change WIFIdns to <%s>\n",WIFIdns);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"ntp=",4) == 0) {
    sprintf(SNTPserver,"%s",cmd+4);
    aval_EEsave(&TABVP_SET1[0], "NET08");      
    sprintf(answ,"Change SNTPserver to <%s>\n",SNTPserver);
    S->print(answ);
    return true;
  }
  /*
  if (strncmp(cmd,"bltn=",5) == 0) {
    sprintf(BTname,"%s",cmd+5);
    prefs.begin(PREDVOLBY_LBMAIN", false);
    prefs.putString("BTname", BTname);
    prefs.end();
    sprintf(answ,"Change BTname to <%s>\n",BTname);
    S->print(answ);
    return true;
  }
  */
  return false;
}
//======================================================
bool MQTTcommand(Stream *S, char *cmd, uint8_t *menust) {
  char answ[100];
  if (strcmp(cmd,"r") == 0) {
        *menust=MENUstMain;
        printMenuMain(S);
        return true;
  }
  if (strcmp(cmd,"h") == 0) {
      printMenuMQTT(S);
      return true;
  }
  if (strcmp(cmd,"s") == 0) {
    avalSET1_print(S);
    return true;
  }
/*
srv=<...>
dname=<...>
gname=<...>
user=<...>
pasw=<...>
*/
  if (strncmp(cmd,"dname=",6) == 0) {
    sprintf(mqtt_DEVname,"%s",cmd+6);
    aval_EEsave(&TABVP_SET1[0], "MQT02");      
    sprintf(answ,"Change mqtt_DEVname to <%s>\n",mqtt_DEVname);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"gname=",6) == 0) {
    sprintf(mqtt_GLOBname,"%s",cmd+6);
    aval_EEsave(&TABVP_SET1[0], "MQT03");      
    sprintf(answ,"Change mqtt_GLOBname to <%s>\n",mqtt_GLOBname);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"pasw=",5) == 0) {
    sprintf(mqtt_pasw,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "MQT05");      
    sprintf(answ,"Change mqtt_pasw to <%s>\n",mqtt_pasw);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"user=",5) == 0) {
    sprintf(mqtt_user,"%s",cmd+5);
    aval_EEsave(&TABVP_SET1[0], "MQT04");      
    sprintf(answ,"Change mqtt_user to <%s>\n",mqtt_user);
    S->print(answ);
    return true;
  }
  if (strncmp(cmd,"srv=",4) == 0) {
    sprintf(mqtt_server,"%s",cmd+4);
    aval_EEsave(&TABVP_SET1[0], "MQT01");      
    sprintf(answ,"Change mqtt_server to <%s>\n",mqtt_server);
    S->print(answ);
    return true;
  }
  return false;
}

//#################################################################################
void process_command(Stream *S, char *cmd, bool *cmd_complete, uint8_t *menust) {
  //char answ[100];
  bool rb=false;
  uint8_t menu=*menust;

  if(menu==MENUstMain) {
    rb=MAINcommand(S, cmd, menust);
  } else if(menu==MENUstWifi) {
    rb=WIFIcommand(S, cmd, menust);
  #if isMQTT
  } else if(menu==MENUstMqtt) {
    rb=MQTTcommand(S,cmd, menust);
  #endif    
  }

  if(!rb) {
    S->print("Menu<");
    S->print(menu);
    S->print("> ");
    S->print("Bad cmd: ");
    S->println(cmd);
  }
   cmd[0] = '\0';         // reset the commandstring
  *cmd_complete = false;  // reset command complete
}
//====================================================================================
void recvChar(Stream *S, char *cmd, bool *cmd_complete) {
  static byte index = 0;
  while (S->available() > 0 && *cmd_complete == false) {
    char rc = S->read();
    if ((rc == '\n') || (rc == '\r')) {
      if(index>0) {
          cmd[index] = '\0'; // terminate the string
          index = 0;
          *cmd_complete = true;
          //S->print("received '"); S->print(cmd); S->println("'");
          //Serial.print("received '"); Serial.print(cmd); Serial.println("'");
      }
    } else if(rc < 0x20) {
      // bad char  
    } else {
      if(index < MAX_NUM_CHARS) cmd[index++] = rc;
    }
  }
}

//#####################################################################

//#if isMENU ==1
void MENUrecv(void) {
  //serial com ..............
  recvChar(&Serial,cmdSCI, &cmdSCI_complete);
  if(cmdSCI_complete) {
    process_command(&Serial,cmdSCI, &cmdSCI_complete, &menuSCIst);
  }
  //serial BT ..............
  #if isBTSER
  recvChar(&SerialBT,cmdBT, &cmdBT_complete);
  if(cmdBT_complete) {
    process_command(&SerialBT,cmdBT, &cmdBT_complete, &menuBTst);
  }
  #endif
}
//#endif

//########################################################
#if isMENU ==2
//  menu task
void menuTask( void * pvParameters )
{
  while(1) {
    MENUrecv();
    delay(5);
  }
  
  vTaskDelete( NULL );
}
#endif

#endif
