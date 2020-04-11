
struct evt_defin EVTDEF;
struct evt_ram EVTRAM[EVT_MAX];
//===============================================================
const char EVT_EEPLB[] = "Events1";
const uint32_t EVT_EEPID =0x28441101;
const struct value_pop TABVP_EVT[] = {
  {"EEPLB",VPTYP_STR,20,0,0,(void *)&EVT_EEPLB,NULL },
  {"EEPID",VPTYP_UI32,4,0,0,(void *)&EVT_EEPID,NULL },
  {"DST01",VPTYP_STR,XTELEF_SIZE,1,0,(void *)&EVTDEF.dst[0].telef,NULL },
  {"DST02",VPTYP_STR,XRECIPIENT_SIZE,1,0,(void *)&EVTDEF.dst[0].recipient,NULL },
  {"DST11",VPTYP_STR,XTELEF_SIZE,1,0,(void *)&EVTDEF.dst[1].telef,NULL },
  {"DST12",VPTYP_STR,XRECIPIENT_SIZE,1,0,(void *)&EVTDEF.dst[1].recipient,NULL },
  
  {"EVT01",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[0].par1,NULL },
  {"EVT02",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[0].func,NULL },
  {"EVT03",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[0].actDelay,NULL },
  {"EVT04",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[0].cosi,NULL },
  {"EVT05",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[0].vmin,NULL },
  {"EVT06",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[0].vmax,NULL },
  {"EVT07",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&EVTDEF.evt[0].subject,NULL },
  {"EVT08",VPTYP_STR,20,1,0,(void *)&EVTDEF.evt[0].msg,NULL },
  
  {"EVT11",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[1].par1,NULL },
  {"EVT12",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[1].func,NULL },
  {"EVT13",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[1].actDelay,NULL },
  {"EVT14",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[1].cosi,NULL },
  {"EVT15",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[1].vmin,NULL },
  {"EVT16",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[1].vmax,NULL },
  {"EVT17",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&EVTDEF.evt[1].subject,NULL },
  {"EVT18",VPTYP_STR,20,1,0,(void *)&EVTDEF.evt[1].msg,NULL },
  
  {"EVT21",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[2].par1,NULL },
  {"EVT22",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[2].func,NULL },
  {"EVT23",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[2].actDelay,NULL },
  {"EVT24",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[2].cosi,NULL },
  {"EVT25",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[2].vmin,NULL },
  {"EVT26",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[2].vmax,NULL },
  {"EVT27",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&EVTDEF.evt[2].subject,NULL },
  {"EVT28",VPTYP_STR,20,1,0,(void *)&EVTDEF.evt[2].msg,NULL },
  
  {"EVT31",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[3].par1,NULL },
  {"EVT32",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[3].func,NULL },
  {"EVT33",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[3].actDelay,NULL },
  {"EVT34",VPTYP_UI8,1,1,0,(void *)&EVTDEF.evt[3].cosi,NULL },
  {"EVT35",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[3].vmin,NULL },
  {"EVT36",VPTYP_UI16,1,1,0,(void *)&EVTDEF.evt[3].vmax,NULL },
  {"EVT37",VPTYP_STR,XSUBJECT_SIZE,1,0,(void *)&EVTDEF.evt[3].subject,NULL },
  {"EVT38",VPTYP_STR,20,1,0,(void *)&EVTDEF.evt[3].msg,NULL },
  
  {"end",VPTYP_NONE,0,0,0,NULL,NULL },
};

void EVT_store(void) {
  prefs.begin(EVT_EEPLB, false);
  prefs.putBytes("EVTDATA",(void *)&EVTDEF, sizeof(struct evt_defin));
  prefs.putUInt("EEPID", EVT_EEPID);
  prefs.end();
}
String procGetEVT(const String& var) {
  return procGet(var, &TABVP_EVT[0] );
}
void procPostEVT(AsyncWebServerRequest *request) {
   procPost(request, &TABVP_EVT[0] );
}
//#####################################################################
String EVT_funcToSTR(uint8_t x) {
  switch(x) {
  case EVT_PIN_TEST:
    return String("test");
  case EVT_PIN_POWER:
    return String("power");
  case EVT_PIN_75P:
    return String("bl75");
  case EVT_PIN_50P:
    return String("bl50");
  case EVT_PIN_25P:
    return String("bl25");
  case EVT_PIN_32:
    return String("pin32");
  case EVT_PIN_33:
    return String("pin33");
  case EVT_PIN_34:
    return String("pin34");
  case EVT_PIN_35:
    return String("pin35");
  }
  return String("off");
}
//=====================================================================
String EVT_toJSON(uint8_t n) {
  String s1,s2;
  char tx[10];
  if(n>=EVT_MAX) return String();
  struct evt_evt *pe=&EVTDEF.evt[n];
  //struct evt_ram *pr=&EVTRAM[n];
  int st = EVT_getST(pe);

  DynamicJsonDocument doc(800);

  s1=MQTTgetTIME();
  if(s1 != "") {
    doc["Time"] = s1.c_str();
  }
  sprintf(tx,"%d",n+1);
  doc["Event"]=tx;
  s2=EVT_funcToSTR(pe->func);
  if(s2 != "") {
    doc["Func"]=s2.c_str();
  } else {
    doc["Func"]="BAD";
  }
  if(st<0) {
    doc["State"]="DIS";
  } else if(st<1) {
    doc["State"]="OFF";
  } else {
    doc["State"]="ON";
  }
  doc["Subject"] = pe->subject;
  doc["Msg"] = pe->msg;
  String out;
  serializeJson(doc, out);
  return out;
}
//=====================================================================
int EVT_getST(struct evt_evt *pe) {
  int ri=0;
  if((pe->par1 & (EVT_PAR_SMS | EVT_PAR_MAIL | EVT_PAR_MQTT)) ==0) return -1;
  if((pe->par1 & (EVT_PAR_DST1 | EVT_PAR_DST2)) ==0) return -1;
  switch(pe->func) {
  case EVT_PIN_TEST:
    if(pe->par1 & EVT_PAR_TST) return 1;
    return 0;
  case EVT_PIN_POWER:
    if(statePower) return 1;
    return 0;
  case EVT_PIN_75P:
    if(batLevel==75) return 1;
    return 0;
  case EVT_PIN_50P:
    if(batLevel==50) return 1;
    return 0;
  case EVT_PIN_25P:
    if(batLevel==25) return 1;
    return 0;
  case EVT_PIN_32:
    pinMode(32, INPUT);
    return digitalRead(32);
  case EVT_PIN_33:
    pinMode(33, INPUT);
    return digitalRead(33);
  case EVT_PIN_34:
    pinMode(34, INPUT);
    return digitalRead(34);
  case EVT_PIN_35:
    pinMode(35, INPUT);
    return digitalRead(35);
  default:
    return -1;
  }
  return -1;
}
void EVT_state_init(void) {
  uint8_t i;
  for(i=0;i<EVT_MAX;i++) {
    //EVTRAM[i].stay=-10;
    EVTRAM[i].old_stay=-10;
    EVTRAM[i].last_act=0;
    EVTRAM[i].cnt_change=0;
  }
}
int EVT_action(uint8_t n) {
  if(n >= EVT_MAX) return -1;
  struct evt_evt *pe=&EVTDEF.evt[n];
  struct evt_ram *pr=&EVTRAM[n];
  int st = EVT_getST(pe);
  if(st < 0 ) return -1;
  if(st > 1 ) return -1;  //?????
  if(pe->par1 & EVT_PAR_INV) {
    if(st == 1 ) st=0;
    else st=1;
  }
  long now= millis();
  if( (now - pr->last_act > ((long)pe->actDelay * 1000)) || (pr->last_act==0) ) {
    if(pr->old_stay == -10) {
      pr->last_act=0;
      pr->old_stay = st;
      return -1;
    }
    pr->last_act=now;
    if(pr->old_stay == st) return 0;
    pr->cnt_change++;
    pr->old_stay = st;
    if(st==1) return 1;
    return 2;
  }
  return -1;
}
//===============================================
void EVT_state_proc(void) {
  uint8_t i;
  int st;
  const char *p1,*p2,*p3;
  String s;
  String msg;
  struct evt_evt *pe;

  for(i=0;i<EVT_MAX;i++) {
    st=EVT_action(i);
    if(st < 1) continue;
    pe=&EVTDEF.evt[i];

    msg="EVT";
    msg+=String(i+1);
    msg+=";";
    msg+=EVT_funcToSTR(pe->func);
    if(st==1) msg+=";ON;";
    else msg+=";OFF;";
    msg +=pe->msg;
        
    if(pe->par1 & EVT_PAR_SMS) {
        if(pe->par1 & EVT_PAR_DST1) {
            if(amsg_verifyTelef(EVTDEF.dst[0].telef)) msms_addS(String(EVTDEF.dst[0].telef),msg);
        }
        if(pe->par1 & EVT_PAR_DST2) {
            if(amsg_verifyTelef(EVTDEF.dst[1].telef)) msms_addS(String(EVTDEF.dst[1].telef),msg);
        }
    }
    if(pe->par1 & EVT_PAR_MAIL) {
        p2=pe->subject;
        p3=msg.c_str();
        if(pe->par1 & EVT_PAR_DST1) {
            p1=EVTDEF.dst[0].recipient;
            if(amsg_verifyMLaddr(p1)) {
                  email_send(p1,p2,p3);
                  delay(100);
            }
        }
        if(pe->par1 & EVT_PAR_DST2) {
            p1=EVTDEF.dst[1].recipient;
            if(amsg_verifyMLaddr(p1)) {
                  email_send(p1,p2,p3);
                  delay(100);
            }
        }
    }
    if(pe->par1 & EVT_PAR_MQTT) {
        String topic = "tele/" + String(mqtt_DEVname) + "/event";
        s=EVT_toJSON(i);
        MQTTpublish(topic, s);
    }
    if(pe->par1 & EVT_PAR_FILE) {
        s=EVT_toJSON(i);
        mgfile_save_msg(FILEDEF_EVENT, s.c_str());
    }
  }
}
//#####################################################################
