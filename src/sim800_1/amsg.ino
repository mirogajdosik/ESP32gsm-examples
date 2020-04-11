
struct amsg AMSG[AMSG_MAX];


struct amsg *amsg_get_free(void) {
  uint8_t i;
  for(i=0;i<AMSG_MAX;i++) {
    if(!AMSG[i].enabled && !AMSG[i].alloc) {
      AMSG[i].alloc=true;
      return &AMSG[i];
    }
  }
  return NULL;
}
/*
struct amsg *amsg_add(struct amsg *pam) {
  struct amsg *p=amsg_get_free();
  if(p==NULL) return NULL;
  if(!pam->enabled && !pam->alloc) return NULL;
  memcpy(p,pam,sizeof(struct amsg));
  return p;
}
*/
bool amsg_testTELEF(char *bf, char *tel) {
  char *p;
  byte i;
  tel[0]=0;
  if(strncmp(bf,"telef<",6) != 0) return false;
  bf=&bf[6];
  p=strchr(bf,'>');
  if(p==NULL) return false;
  for(i=0;i<(XTELEF_SIZE-1);i++) {
    if(*bf=='>') {
      break;
    } else {
      tel[i]=*bf;
      tel[i+1]=0;
    }
    bf++;
  }
  return true; 
}
bool amsg_verifyTelef(const char *t) {
  if(t[0]!='+') return false;
  if(strlen(t) < 7) return false;
  return true;
}
bool amsg_verifyMLaddr(const char *t) {
  char *p;
  if(t[0]==0) return false;
  p=strchr(t,'@');
  if(p==NULL) return false;
  p=strchr(p,'.');
  if(p==NULL) return false;
  if(p[1]==0) return false;
  return true;      
}
void amsg_print(struct amsg *pm) {
  Serial.println("AMSG ------");
  Serial.print("from: ");
  Serial.println(pm->from);
  if(pm->to[0]) {
    Serial.print("to: ");
    Serial.println(pm->to);
  }
  Serial.print("subject: ");
  Serial.println(pm->subject);
  Serial.print("telef: ");
  Serial.println(pm->telef);
  Serial.print("subject forMe=");
  Serial.print(pm->subjForMe);
  Serial.print(", okTelef=");
  Serial.println(pm->okTelef);
  Serial.print("msg: ");
  Serial.println(pm->msg);
}
//========================================================================
String amsg_toHTML(uint8_t n) {
  String s;
  const char *tx1="<tr><td><div align=\x22left\x22>";
  const char *tx2="</div></td></tr>";
  char tx[250];
  uint16_t sz;
  struct amsg *pm;

  if(n>=AMSG_MAX) return String();
  pm=&AMSG[n];
  if(!pm->enabled) return String();
  sz=sprintf(tx,"%s%d:&nbsp&nbsp",tx1,n);
  if(pm->typFROM==AMSG_TYP_FROM_SMS) {
    sz+=sprintf(tx+sz,"fromSMS");
  } else if(pm->typFROM==AMSG_TYP_FROM_MAIL) {
    sz+=sprintf(tx+sz,"fromMAIL");
  } else if(pm->typFROM==AMSG_TYP_FROM_MQTT) {
    sz+=sprintf(tx+sz,"fromMQTT");
  } else if(pm->typFROM==AMSG_TYP_FROM_EVENT) {
    sz+=sprintf(tx+sz,"fromEVENT");
  } else {
    sz+=sprintf(tx+sz,"from???");
  }
  sz+=sprintf(tx+sz,", to[");
  if(pm->typTO & AMSG_TYP_TO_SMS) {
    sz+=sprintf(tx+sz,"SMS,");
  }
  if(pm->typTO & AMSG_TYP_TO_MAIL) {
    sz+=sprintf(tx+sz,"MAIL,");
  }
  if(pm->typTO & AMSG_TYP_TO_MQTT) {
    sz+=sprintf(tx+sz,"MQTT,");
  }
  if(pm->typTO & AMSG_TYP_TO_FILE) {
    sz+=sprintf(tx+sz,"FILE,");
  }
  sz+=sprintf(tx+sz,"]%s",tx2);
  s+=tx;
  if(pm->from[0]) {
    sprintf(tx,"%sfrom:&nbsp&nbsp%s%s",tx1,pm->from,tx2);
    s+=tx;
  }
  if(pm->to[0]) {
    sprintf(tx,"%sto:&nbsp&nbsp%s%s",tx1,pm->to,tx2);
    s+=tx;
  }
  if(pm->subject[0]) {
    sprintf(tx,"%ssubject:&nbsp&nbsp%s%s",tx1,pm->subject,tx2);
    s+=tx;
  }
  if(pm->telef[0]) {
    sprintf(tx,"%stelef:&nbsp&nbsp%s%s",tx1,pm->telef,tx2);
    s+=tx;
  }
  //Serial.print("subject forMe=");
  //Serial.print(pm->subjForMe);
  //Serial.print(", okTelef=");
  //Serial.println(pm->okTelef);
  //sprintf(tx,"<tr><td><textarea>");
  //s+=tx;
  s+="<tr><td><textarea>";
  s+=TextToHTML(pm->msg);
  //sprintf(tx,"</textarea></td></tr>");
  //s+=tx;
  s+="</textarea></td></tr>";
  
  return s;
}
//========================================================================
//char *amsg_toJSON(uint8_t n) {
String amsg_toJSON(uint8_t n) {
  String s;

  struct amsg *pm;

  if(n>=AMSG_MAX) return String();  //NULL;
  pm=&AMSG[n];
  if(!pm->enabled) return String(); //NULL;

  DynamicJsonDocument doc(800);


  //JsonObject obj = doc.as<JsonObject>();
  if(pm->typFROM==AMSG_TYP_FROM_SMS) {
    doc["typfrom"] = "SMS";
  } else if(pm->typFROM==AMSG_TYP_FROM_MAIL) {
    doc["typfrom"] = "MAIL";
  } else if(pm->typFROM==AMSG_TYP_FROM_MQTT) {
    doc["typfrom"] = "MQTT";
  } else if(pm->typFROM==AMSG_TYP_FROM_EVENT) {
    doc["typfrom"] = "EVENT";
  } else {
    doc["typfrom"] = "???";
  }
  doc["typto"] = pm->typTO;
  doc["from"] = pm->from;
  doc["to"] = pm->to;
  doc["subject"] = pm->subject;
  doc["telef"] = pm->telef;
  doc["smstime"] = pm->smsTime;
  doc["msg"] = pm->msg;
  String out;
  serializeJson(doc, out);
  return out;
}
//########################################################################
extern bool mqtt_forceSendSENSOR;

void amsgTask( void * pvParameters ) {
  uint8_t i=0;  
  struct amsg *pam;
  char *p1,*p2,*p3;
  String s;
  EVT_state_init();
  while(1) {
    astate_ini_time();
    //............... event ................................
    EVT_state_proc();
    //............... msg .................................
    pam=&AMSG[i];
    if(pam->enabled) {
      if(pam->typTO & AMSG_TYP_TO_MQTT) {
          pam->typTO &= ~AMSG_TYP_TO_MQTT;
          s=amsg_toJSON(i);
          String topic = "tele/" + String(mqtt_DEVname) + "/amsg";
          MQTTpublish(topic, s);
      } 
      if(pam->typTO & AMSG_TYP_TO_SMS) {
          if(amsg_verifyTelef(pam->telef)) {
            if( msms_addS(String(pam->telef),String(pam->msg))) {
              pam->typTO &= ~AMSG_TYP_TO_SMS;
            }
          } else {
            Serial.println(F("Error Telef!!"));
            pam->typTO &= ~AMSG_TYP_TO_SMS;
          }
      } 
      if(pam->typTO & AMSG_TYP_TO_MAIL) {
         // bool email_send(const char *recipient, const char *subject, const char *message);
         //if(pam->typFROM==AMSG_TYP_FROM_MQTT) {
         
         if(!amsg_verifyMLaddr(pam->to)) p1=SMTPrecipient;
         else p1=pam->to;
         if(pam->subject[0]==0) p2=SMTPsubject;
         else p2=pam->subject;
         p3=pam->msg;
         /*
         if(email_send(p1,p2,p3)) {
            pam->typTO &= ~AMSG_TYP_TO_MAIL;
         }
         */
         email_send(p1,p2,p3);
         pam->typTO &= ~AMSG_TYP_TO_MAIL;
      } 
      if(pam->typTO & AMSG_TYP_TO_FILE) {
          pam->typTO &= ~AMSG_TYP_TO_FILE;
          s=amsg_toJSON(i);
          mgfile_save_msg(FILEDEF_MSG, s.c_str());
      }
      if(!pam->typTO) {
        pam->enabled=false;
      }
      delay(1000);
    }
    i++;
    if(i>=AMSG_MAX) i=0;
    delay(200);
  }
}
//========================================================================
void amsg_setup(void) {
  memset(&AMSG,0,AMSG_MAX * sizeof(struct amsg));
  /*
  //.......... test ...................
  AMSG[0].typFROM=AMSG_TYP_FROM_MAIL;
  sprintf(AMSG[0].subject,"xxsubjectzz");
  sprintf(AMSG[0].telef,"+1234567");
  sprintf(AMSG[0].msg,"sprava");
  AMSG[0].enabled=true;
  String s;
  s=amsg_toJSON(0);
  Serial.println("Test JSON 1");
  Serial.println(s);
  //...................................
  */
  
  xTaskCreate(
      amsgTask,          // Task function.
      "amsgTask",        // String with name of task.
      10000,            // Stack size in bytes.
      NULL,             // Parameter passed as input of the task
      1,                // Priority of the task.
      NULL);            // Task handle.
}
//########################################################################
