
//const char EEPLB[] = "EEPLB";
//const char EEPID[] = "EEPID";
#define EEPLB "EEPLB"
#define EEPID "EEPID"
//==================================================================================
const char SET1_EElabel[] = "ESPgsmMain";
const uint32_t SET1_EEid = 0x27441301;

const struct value_pop TABVP_SET1[] = {
  {EEPLB,VPTYP_STR,20,0,0,(void *)&SET1_EElabel,NULL },
  {EEPID,VPTYP_UI32,4,0,0,(void *)&SET1_EEid,NULL },
  {"NET01",VPTYP_STR,XNAME_SIZE,1,0,(void *)&WIFIssid,NULL },
  {"NET02",VPTYP_STR,XPASS_SIZE,1,0,(void *)&WIFIpasw,NULL },
  {"NET03",VPTYP_STR,XNAME_SIZE,1,0,(void *)&WIFImdns,"esp32gsm" },
  {"NET04",VPTYP_STR,XIP_SIZE,1,0,(void *)&WIFIip,NULL },
  {"NET05",VPTYP_STR,XIP_SIZE,1,0,(void *)&WIFImask,"255.255.255.0" },
  {"NET06",VPTYP_STR,XIP_SIZE,1,0,(void *)&WIFIgate,NULL },
  {"NET07",VPTYP_STR,XIP_SIZE,1,0,(void *)&WIFIdns,NULL },
  {"NET08",VPTYP_STR,XNAME_SIZE,1,0,(void *)&SNTPserver,"pool.ntp.org" },
  {"NET09",VPTYP_BOOL,1,1,0,(void *)&WIFIstatic,NULL },
  {"NET10",VPTYP_UI8,1,0,0,(void *)&wifi_log,NULL },

  {"MQT00",VPTYP_BOOL,1,1,0,(void *)&mqtt_enabled,NULL },
  {"MQT01",VPTYP_STR,XIP_SIZE,1,0,(void *)&mqtt_server,NULL },
  {"MQT02",VPTYP_STR,XNAME_SIZE,1,0,(void *)&mqtt_DEVname,"ESP32gsm"},
  {"MQT03",VPTYP_STR,XNAME_SIZE,1,0,(void *)&mqtt_GLOBname,"Tasmota" },
  {"MQT04",VPTYP_STR,XUSER_SIZE,1,0,(void *)&mqtt_user,"admin" },
  {"MQT05",VPTYP_STR,XPASS_SIZE,1,0,(void *)&mqtt_pasw,"1234" },
  {"MQT06",VPTYP_UI32,4,1,0,(void *)&mqtt_refrSTATUS,"60" },
  {"MQT07",VPTYP_UI32,4,1,0,(void *)&mqtt_refrSENSOR,"30" },
  {"MQT10",VPTYP_UI8,1,0,0,(void *)&mqtt_log,NULL },

  {"WEB01",VPTYP_STR,XUSER_SIZE,1,0,(void *)&WEBuser1,"admin" },
  {"WEB02",VPTYP_STR,XPASS_SIZE,1,0,(void *)&WEBpass1,"admin" },
  {"WEB03",VPTYP_STR,XUSER_SIZE,1,0,(void *)&WEBuser2,"root" },
  {"WEB04",VPTYP_STR,XPASS_SIZE,1,0,(void *)&WEBpass2,"root" },

  {"BLT01",VPTYP_STR,XNAME_SIZE,1,0,(void *)&BTname,"ESP32btGSM" },
  
  {"RESET",VPTYP_FNPOST,0,0,123,NULL,NULL },
  {"end",VPTYP_NONE,0,0,0,NULL,NULL },
};

//==================================================================================
const char MAIL_EElabel[] = "ESPgsmMail";
const uint32_t MAIL_EEid = 0x27441301;
bool IMAPdefined=0;
const struct value_pop TABVP_EML1[] = {
  {EEPLB,VPTYP_STR,20,0,0,(void *)&MAIL_EElabel,NULL },
  {EEPID,VPTYP_UI32,4,0,0,(void *)&MAIL_EEid,NULL },
  {"SMTP00",VPTYP_BOOL,1,1,0,(void *)&SMTPenabled,NULL },
  {"SMTP01",VPTYP_STR,XNAME_SIZE,1,0,(void *)&SMTPserverName,"smtp.gmail.com" },
  {"SMTP02",VPTYP_UI16,2,1,0,(void *)&SMTPserverPort,"587" },
  {"SMTP03",VPTYP_STR,XNAME_SIZE,1,0,(void *)&SMTPuserName,NULL },
  {"SMTP04",VPTYP_STR,XPASS_SIZE,1,0,(void *)&SMTPpassword,NULL },
  {"SMTP05",VPTYP_STR,XNAME_SIZE,1,0,(void *)&SMTPsenderName,"ESP32" },
  {"SMTP06",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&SMTPsubject,"subject" },
  {"SMTP07",VPTYP_STR,XRECIPIENT_SIZE,1,0,(void *)&SMTPrecipient,NULL },
  {"SMTP10",VPTYP_UI8,1,1,0,(void *)&SMTPdebug,NULL },
  
  {"IMAPdf",VPTYP_UI8,1,0,0,(void *)&IMAPdefined,isIMAP },
  {"IMAP00",VPTYP_BOOL,1,1,0,(void *)&IMAPenabled,NULL },
  {"IMAP01",VPTYP_STR,XNAME_SIZE,1,0,(void *)&IMAPserverName,"imap.gmail.com" },
  {"IMAP02",VPTYP_UI16,2,1,0,(void *)&IMAPserverPort,"993" },
  {"IMAP03",VPTYP_STR,XNAME_SIZE,1,0,(void *)&IMAPuserName,NULL },
  {"IMAP04",VPTYP_STR,XPASS_SIZE,1,0,(void *)&IMAPpassword,NULL },
  {"IMAP05",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&IMAPsubject,"miga" },
  {"IMAP10",VPTYP_UI8,1,1,0,(void *)&IMAPdebug,NULL },
  
  {"POP00",VPTYP_BOOL,1,1,0,(void *)&POP3enabled,NULL },
  {"POP01",VPTYP_STR,XNAME_SIZE,1,0,(void *)&POP3serverName,NULL },
  {"POP02",VPTYP_UI16,2,1,0,(void *)&POP3serverPort,"110" },
  {"POP03",VPTYP_STR,XNAME_SIZE,1,0,(void *)&POP3userName,NULL },
  {"POP04",VPTYP_STR,XPASS_SIZE,1,0,(void *)&POP3password,NULL },
  {"POP05",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&POP3subject,"miga" },
  {"POP06",VPTYP_UI16,2,1,0,(void *)&POP3reread,"30" },
  {"POP07",VPTYP_BOOL,1,1,0,(void *)&POP3toFile,NULL },
  {"POP08",VPTYP_BOOL,1,1,0,(void *)&POP3toSMS,NULL },
  {"POP09",VPTYP_BOOL,1,1,0,(void *)&POP3toMQTT,NULL },
  {"POP10",VPTYP_UI8,1,1,0,(void *)&POP3debug,NULL },

  {"RESET",VPTYP_FNPOST,0,0,123,NULL,NULL },
  {"end",VPTYP_NONE,0,0,0,NULL,NULL },
};
//==================================================================================
const char MGSMS_EElabel[] = "ESPgsmSMSG";
const uint32_t MGSMS_EEid = 0x27451301;

const struct value_pop TABVP_MGSMS[] = {
  {EEPLB,VPTYP_STR,20,0,0,(void *)&MGSMS_EElabel,NULL },
  {EEPID,VPTYP_UI32,4,0,0,(void *)&MGSMS_EEid,NULL },
  {"MGSMS00",VPTYP_UI8,1,1,0,(void *)&MGSMS.cfg,"48" },
  {"MGSMS01",VPTYP_UI8,1,1,0,(void *)&MGSMS.replys,"2" },
  {"MGSMS02",VPTYP_UI16,2,1,0,(void *)&MGSMS.maxSMSday,"6" },
  {"MGSMS03",VPTYP_UI16,2,1,0,(void *)&MGSMS.resend,"5" },
  {"MGSMS04",VPTYP_UI16,2,1,0,(void *)&MGSMS.rerecv,"20" },
  {"MGSMS05",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&MGSMS.subject,"from sms" },
  {"MGSMS06",VPTYP_STR,XRECIPIENT_SIZE,1,0,(void *)&MGSMS.recipient,NULL },
  {"end",VPTYP_NONE,0,0,0,NULL,NULL },
};
//###################################################################
//  
//      GLOBAL
//===================================================================
bool aval_check_table(const struct value_pop *pv) {
  if(strcmp(pv->key,EEPLB)) {
    Serial.println("AVtable no EEPLB");
    return false;
  }
  pv++;
  if(strcmp(pv->key,EEPID)) {
    Serial.println("AVtable no EEPID");
    return false;
  }
  return true;
}
uint32_t aval_str2UI(const char *tx) {
  int v;
  if(tx==NULL) return 0;
  v = atoi(tx);
  return (v & 0xFFFFFFFF);
}
//###################################################################
//  
//      DEFAULT
//===================================================================
bool aval_default(void *mem, size_t size, const struct value_pop *pv) {
  bool *pb;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t v,*p32;
  char *pc;
  if(mem!=NULL) {
    memset(mem,0,size);
  }
  if(!aval_check_table(pv)) return false;
  pv++;
  pv++;
  while(pv->p!=NULL) {
      switch(pv->typ) {
      case VPTYP_BOOL:
        pb=(bool *)pv->p;
        if(pv->vdef==NULL) {
          *pb=false;
        } else if(strcmp(pv->vdef,"true")==0) {
          *pb=true;
        } else {
          *pb=false;
        }
        break;
      case VPTYP_UI8:
      case VPTYP_UI16:
      case VPTYP_UI32:
        v=aval_str2UI(pv->vdef);
        if(pv->typ==VPTYP_UI8) {
          p8=(uint8_t *)pv->p;
          *p8=(v &255);
        } else if(pv->typ==VPTYP_UI16) {
          p16=(uint16_t *)pv->p;
          *p16=(v & 0xFFFF);
        } else {
          p32=(uint32_t *)pv->p;
          *p32=v;
        }
        break;
      case VPTYP_STR:
        pc=(char *)pv->p;
        if(pv->vdef==NULL) {
          pc[0]=0;
        } else {
          snprintf(pc,pv->size,"%s",pv->vdef);
        }
        break;
      }
    
    pv++;

    
  }

}
//###################################################################
//  
//      EEPROM
//===================================================================
bool aval_EEsave(const struct value_pop *pv, const char *key) {
  bool *pb;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t *p32;
  char *pc;
  bool mam;
  uint32_t id;
  const char *lb;
  if(strcmp(pv->key,EEPLB)) {
    Serial.println(F("AVsave bad table no EEPLB"));
    return false;
  }
  lb=(const char *)pv->p;
  pv++;
  if(strcmp(pv->key,EEPID)) {
    Serial.println(F("AVsave bad table no EEPID"));
    return false;
  }
  p32=(uint32_t *)pv->p;
  id=*p32;
  pv++;
  //..........................................
  //prefs.begin(PREDVOLBY_LBMAIN, false);
  prefs.begin(lb, false);
  while(pv->p!=NULL) {
  //while(pv->typ!=VPTYP_NONE) {
    mam=true;
    if(key != NULL) {
        if(strcmp(pv->key,key)) mam=false;
    }
    if(!pv->toEEP) mam=false;
    if(mam) {
      switch(pv->typ) {
      case VPTYP_BOOL:
        pb=(bool *)pv->p;
        prefs.putBool(pv->key, *pb);
        break;
      case VPTYP_UI8:
        p8=(uint8_t *)pv->p;
        prefs.putUChar(pv->key, *p8);
        break;
      case VPTYP_UI16:
        p16=(uint16_t *)pv->p;
        prefs.putUShort(pv->key, *p16);
        break;
      case VPTYP_UI32:
        p32=(uint32_t *)pv->p;
        prefs.putUInt(pv->key, *p32);
        break;
      case VPTYP_STR:
        pc=(char *)pv->p;
        prefs.putString(pv->key, pc);
        break;
      }
      if(key != NULL) break;
    }
    pv++;
  }
  if(id != prefs.getUInt(EEPID, 0)) {
    prefs.putUInt(EEPID, id);
  }
  prefs.end();
  return true;
}
//===================================================================
bool aval_EEload(const struct value_pop *pv, const char *key, bool errdel) {
  bool *pb;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t *p32;
  char *pc;
  bool mam;
  uint32_t id;
  const char *lb;
  if(strcmp(pv->key,EEPLB)) {
    Serial.println(F("AVload bad table no EEPLB"));
    return false;
  }
  lb=(const char *)pv->p;
  pv++;
  if(strcmp(pv->key,EEPID)) {
    Serial.println("AVload bad table no EEPID");
    return false;
  }
  p32=(uint32_t *)pv->p;
  id=*p32;
  pv++;
  prefs.begin(lb, false);
  if(id != prefs.getUInt("EEPID", 0)) {
    Serial.println("AVload bad ID");
    if(errdel) {
      prefs.clear(); 
    }
    prefs.end();
    return false;
  }
  prefs.end();
  //..........................................
  prefs.begin(lb, true);
  while(pv->p!=NULL) {
    mam=true;
    if(key != NULL) {
        if(strcmp(pv->key,key)) mam=false;
    }
    if(!pv->toEEP) mam=false;
    if(mam) {
      switch(pv->typ) {
      case VPTYP_BOOL:
        pb=(bool *)pv->p;
        *pb=prefs.getBool(pv->key,false);
        break;
      case VPTYP_UI8:
        p8=(uint8_t *)pv->p;
        *p8=prefs.getUChar(pv->key, 0);
        break;
      case VPTYP_UI16:
        p16=(uint16_t *)pv->p;
        *p16=prefs.getUShort(pv->key, 0);
        break;
      case VPTYP_UI32:
        p32=(uint32_t *)pv->p;
        *p32=prefs.getUInt(pv->key, 0);
        break;
      case VPTYP_STR:
        pc=(char *)pv->p;
        prefs.getString(pv->key, pc,pv->size);
        break;
      }
      if(key != NULL) break;
    }
    pv++;
  }
  prefs.end();
  return true;
}
//###################################################################
//  
//      WEB
//
//===================================================================
String procGet(const String& var,const struct value_pop *pv) {
  char tx[30];
  bool *pb;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t *p32;
  char *pc;
  //Serial.println(var);
  pv++;
  pv++;
  while(pv->p!=NULL) {
    if(var == String(pv->key) ) {
      switch(pv->typ) {
      case VPTYP_BOOL:
        pb=(bool *)pv->p;
        if(*pb) return String("1");
        return String("0");
      case VPTYP_UI8:
        p8=(uint8_t *)pv->p;
        sprintf(tx,"%d",*p8);
        return String(tx);
      case VPTYP_UI16:
        p16=(uint16_t *)pv->p;
        sprintf(tx,"%d",*p16);
        return String(tx);
      case VPTYP_UI32:
        p32=(uint32_t *)pv->p;
        sprintf(tx,"%d",*p32);
        return String(tx);
      case VPTYP_STR:
        pc=(char *)pv->p;
        return String(pc);
      }
      Serial.print("ERROR badtyp key: ");
      Serial.println(var);
      return String();
    }
    pv++;
  }
  if(var == "MAINIP"){
    return String(WiFi.localIP().toString().c_str());
  }
  Serial.print("ERROR no key: ");
  Serial.println(var);
  return String();
}
//----------------------------------------------------------------------
String procGetEML1(const String& var) {
  return procGet(var, &TABVP_EML1[0] );
}
String procGetSET1(const String& var) {
  return procGet(var, &TABVP_SET1[0] );
}
String procGetMGSMS(const String& var) {
  return procGet(var, &TABVP_MGSMS[0] );
}
//======================================================================
void procPost(AsyncWebServerRequest *request,const struct value_pop *pv) {
  char tx[30];
  bool *pb;
  bool mam;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t *p32;
  char *pc;
  String keyVal;
  int v;
  const struct value_pop *pvpush = pv;
  pv++;
  pv++;
  //while(pv->p!=NULL) {
  while(pv->typ!=VPTYP_NONE) {
    if (request->hasParam(pv->key,true)) {
      keyVal= request->getParam(pv->key,true)->value();
      mam=true;
      switch(pv->typ) {
      case VPTYP_BOOL:
        pb=(bool *)pv->p;
        if(keyVal == String("0")) *pb=false;
        if(keyVal == String("1")) *pb=true;
        break;
      case VPTYP_UI8:
        p8=(uint8_t *)pv->p;
        v = atoi(keyVal.c_str());
        *p8=(v & 255);
        break;
      case VPTYP_UI16:
        p16=(uint16_t *)pv->p;
        v = atoi(keyVal.c_str());
        *p16=(v & 0xFFFF);
        break;
      case VPTYP_UI32:
        p32=(uint32_t *)pv->p;
        v = atoi(keyVal.c_str());
        *p32=(v & 0xFFFFFFFF);
        break;
      case VPTYP_STR:
        pc=(char *)pv->p;
        //sprintf(pc,"%s",keyVal.c_str());
        snprintf(pc,pv->size,"%s",keyVal.c_str());
        break;
      case VPTYP_FNPOST:
        if(!strcmp(pv->key,"RESET")) {
          if(keyVal=="123") {
            Serial.println("Go to reset");
            toReset=true;
          }
        }
        mam=false;
        break;
      default:
        mam=false;
      }
      if(mam) {
        aval_EEsave(pvpush, pv->key);
      }
    }
    pv++;
  }
}
//----------------------------------------------------------------------
void procPostEML1(AsyncWebServerRequest *request) {
   procPost(request, &TABVP_EML1[0] );
}
void procPostSET1(AsyncWebServerRequest *request) {
   procPost(request, &TABVP_SET1[0] );
}
void procPostMGSMS(AsyncWebServerRequest *request) {
   procPost(request, &TABVP_MGSMS[0] );
}
//######################################################################






//######################################################################
//
//    SET1
//
//============================================================
void avalSET1_print(Stream *S) {
  char tx[200];
  sprintf(tx,"WIFI enabled=%d, static=%d, connected=%d\n",WIFIenabled,WIFIstatic,WIFIconnected);
  S->print(tx);
  sprintf(tx,"ssid:passw=%s:%s\n",WIFIssid,WIFIpasw);
  S->print(tx);
  sprintf(tx,"mDNSname=%s\n",WIFImdns);
  S->print(tx);
  sprintf(tx,"ip/mask/gate/dns=%s, %s, %s, %s\n",WIFIip,WIFImask,WIFIgate,WIFIdns);
  S->print(tx);
  sprintf(tx,"SNTPserver=%s, BTname=%s\n",SNTPserver,BTname);
  S->print(tx);
  sprintf(tx,"MQTT server=%s, DEVname=%s, GLOBname=%s, user=%s, pass=%s\n",mqtt_server,mqtt_DEVname,mqtt_GLOBname,mqtt_user,mqtt_pasw);
  S->print(tx);
  sprintf(tx,"MQTT refrSTATUS=%d, refrSENSOR=%d\n",mqtt_refrSTATUS,mqtt_refrSENSOR);
  S->print(tx);
  sprintf(tx,"WEB user1=%s:%s, user2=%s:%s\n",WEBuser1,WEBpass1,WEBuser2,WEBpass2);
  S->print(tx);
}
//######################################################################
//
//    RESET
//
void aval_reset(void *mem, size_t size, const struct value_pop *pv) {
  aval_default(mem,size,pv);
  if(!aval_EEload(pv,NULL,true)) {
      //Serial.println("new value");
      aval_default(mem,size,pv);
      aval_EEsave(pv,NULL);
  }
}
void aval_resetALL(void) {
  aval_reset(NULL,0,&TABVP_SET1[0]);
  aval_reset(NULL,0,&TABVP_EML1[0]);
  aval_reset(NULL,0,&TABVP_MGSMS[0]);
  aval_reset((void *)&EVTDEF,sizeof(struct evt_defin),&TABVP_EVT[0]);
}
//###################################################################
String procGetGLOBAL(const String& var) {
  char tx[100];
  //Serial.println(var);
  if(var == "MDNS"){
    return String(WIFImdns);
  }
  if(var == "MAINIP"){
    return String(WiFi.localIP().toString().c_str());
  }
  if(var == "TIME"){
    #if isNTP
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      //Serial.println("Failed to obtain time");
      sprintf(tx,"Failed to obtain time");
    } else {
      //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      strftime(tx, sizeof(tx), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    }
    #else
    sprintf(tx,"20:12:03 08/02/2020");
    #endif
    return String(tx);
  }
  //----------- BAT -----------------------
  if(var.startsWith(String("BAT"))) {
    if(var == "BAT1"){
      sprintf(tx,"%02X,%02X,%02X",batReg[0],batReg[1],batReg[2]);
      return String(tx);
    }
    if(var == "BAT2"){
      sprintf(tx,"%02X,%02X,%02X,%02X",batReg[3],batReg[4],batReg[5],batReg[6]);
      return String(tx);
    }
    if(var == "BAT3"){
      sprintf(tx,"%02X",batReg[7]);
      return String(tx);
    }
    if(var == "BAT4"){
      sprintf(tx,"%02X,%02X,%02X,%02X,%02X",batReg[8],batReg[9],batReg[10],batReg[11],batReg[12]);
      return String(tx);
    }
    if(var == "BAT5"){
      sprintf(tx,"%02X",batReg[15]);
      return String(tx);
    }
    if(var == "BATLEVEL"){
      sprintf(tx,"%d",batLevel);
      return String(tx);
    }
    if(var == "BATSTAT"){
      sprintf(tx,"%d",batState);
      return String(tx);
    }
    if(var == "BATLOW"){
      if(batLow2) return String("<3.0V");
      if(batLow1) return String("<3.2V");
      return String();
    }
  }
  //--------------------------------------------------
  if(var == "HEAP_SIZE"){
    sprintf(tx,"%ld",ESP.getFreeHeap());
    return String(tx);
  }
  if(var == "POWER"){
    if(statePower) return String("ON");
    return String("OFF");
  }
  if(var == "CHARGER"){
    if(stateCharger) return String("ON");
    return String("OFF");
  }
  if(var == "PERC"){
    return String("\x25");
  }
  //----------- EVENT -----------------------
  if(var.startsWith(String("EVENT"))) {
    //------------ EVENT_ST ----------------------
    if(var.indexOf("EVENT_ST")>=0) {
      uint8_t n=var.c_str()[8];
      n--;
      n &= 3;
      struct evt_ram *pr=&EVTRAM[n];
      if(pr->old_stay <0) return String("disable");
      if(pr->old_stay ==1) return String("ON");
      return String("OFF");
    }
    //------------ EVENT_CC ----------------------
    if(var.indexOf("EVENT_CC")>=0) {
      uint8_t n=var.c_str()[8];
      n--;
      n &= 3;
      struct evt_ram *pr=&EVTRAM[n];
      sprintf(tx,"%d",pr->cnt_change);
      return String(tx);
    }
  }
  //--------------------------------------------
  return String();
}
//===================================================================

//###################################################################
//  
//      SYSTEM
//===================================================================
void syspar_print(Stream *S) {
  char tx[200];
  S->print("Free prefs = ");
  S->println(prefs.freeEntries());
  S->println(ESP.getSdkVersion());
  sprintf(tx,"Flash=%lX\n",ESP.getFlashChipSize());
  S->print(tx);
  sprintf(tx,"Efuse=%lX\n",ESP.getEfuseMac());
  S->print(tx);
}
//######################################################################
