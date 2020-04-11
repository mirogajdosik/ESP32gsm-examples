
bool SMSforceRead=false;
bool SMSforceDelAll=false;
bool SMSdelete1=false;
//bool SMSdebugRD1=true;
//bool SMSdebugWR1=true;
//long SMSdebTime1=0;
//long SMSdebTime2=0;
struct mgsms MGSMS;
#define SMSdebugRD1 (MGSMS.cfg & MGSMSdebRD1)
#define SMSdebugWR1 (MGSMS.cfg & MGSMSdebWR1)
#define SMStoMAIL (MGSMS.cfg & MGSMStoMAIL)
#define SMStoMQTT (MGSMS.cfg & MGSMStoMQTT)
#define SMStoFILE (MGSMS.cfg & MGSMStoFILE)
//##############################################################
//
//    UTIL
//
String msms_typ2str(uint8_t st) {
  switch(st) {
  case 0:  
    return "UNREAD";
  case 1:
    return "READ";
  case 2:
    return "UNSENT";
  case 3:
    return "SENT";
  case 4:
    return "ALL";
  }  
  return "UNDEF";
}
//##############################################################
//
//    DELETE
//
//--------------------------------------------------------------
bool sms_delete(uint8_t n, long *etime) {
  long t1;
  bool ok;

  t1=millis();
  ok=modem.deleteSmsMessage(n);
  if(etime != NULL) *etime=millis()-t1;
  return ok;   
}
//--------------------------------------------------------------
bool sms_deleteAll(long *etime) {
  long t1;
  bool ok;

  t1=millis();
  ok=modem.deleteAllSmsMessages(DeleteAllSmsMethod::All);
  if(etime != NULL) *etime=millis()-t1;
  return ok;   
}
//--------------------------------------------------------------
void sms_deleteAllproc(void) {
  char tx[80];
  uint8_t sz;
  bool ok;
  long t1;
  if(SMSforceDelAll) {
    SMSforceDelAll=false;
    ok=sms_deleteAll(&t1);
    sz=sprintf(tx,"SMS delete all, ");
    if(ok) sz+=sprintf(tx+sz,"OK");
    else sz+=sprintf(tx+sz,"ERROR");
    sz+=(tx+sz,", elapsed time: %ldms",t1);
    Serial.println(tx);
  }
}
//##############################################################
//
//    READ
//
#define SMSRD_MAX 10
struct msms_read {
  byte cnt;
  byte tot;
  MessageStorage ms; 
  Sms  xsms[SMSRD_MAX];
};
struct msms_read MSMS_R;
//==================================================================
/*
bool msms_rdok(struct msms_read *pr) {
  if(pr->
}
*/
void SMSprint(Sms *sms) {
  Serial.print("time: ");
  Serial.println(sms->serviceCentreTimeStamp);
  Serial.print("addr: ");
  Serial.println(sms->originatingAddress);    
  Serial.print("msg: ");
  Serial.println(sms->message);
  Serial.print("type: ");
  Serial.println(msms_typ2str((uint8_t)sms->status));
}
//-----------------------------------------------------------------
void msms_read(void) {
  char tx[200];
  long t1,t2,t3;
  bool ok;
  uint16_t sz;
  uint8_t i,m,stat;
  bool recvmsg;
  struct amsg *pam;

  MSMS_R.ms.type[0]=MessageStorageType::Either_SIMPreferred;
  MSMS_R.ms.type[1]=MessageStorageType::Either_SIMPreferred;
  MSMS_R.ms.type[2]=MessageStorageType::Either_SIMPreferred;
  MSMS_R.ms.used[0]=0;
  MSMS_R.ms.used[1]=0;
  MSMS_R.ms.used[2]=0;
  t1=millis();
  MSMS_R.ms=modem.getPreferredMessageStorage();
  if(SMSdebugRD1) {
      Serial.print("======== ReadSMS: ");
      printLocalTime(&Serial);
      sprintf(tx,"ReadMS elapsed time=%ldms",millis()-t1);
      Serial.println(tx);
      sprintf(tx,"MStype> %u,%u,%u",MSMS_R.ms.type[0],MSMS_R.ms.type[1],MSMS_R.ms.type[2]);
      Serial.println(tx);
      sprintf(tx,"MSused> %u,%u,%u",MSMS_R.ms.used[0],MSMS_R.ms.used[1],MSMS_R.ms.used[2]);
      Serial.println(tx);
      sprintf(tx,"MStotal> %u,%u,%u",MSMS_R.ms.total[0],MSMS_R.ms.total[1],MSMS_R.ms.total[2]);
      Serial.println(tx);
  }



  
  MSMS_R.tot=MSMS_R.ms.total[0];
  MSMS_R.cnt=MSMS_R.ms.used[0];
  if(MSMS_R.cnt>SMSRD_MAX) MSMS_R.cnt=SMSRD_MAX;
  m=MSMS_R.cnt;
  t1=millis();
  for(i=1;i<=m;i++) {
      pam=amsg_get_free();
      recvmsg=true;
      if(pam==NULL) recvmsg=false;
      t2=millis();
      MSMS_R.xsms[i-1]=modem.readSmsMessage(i,recvmsg);     //false);
      t2=millis()-t2;
      if(SMSdebugRD1) {
        if(recvmsg) sprintf(tx,"ReadSMS[%d,%d] change state, elapsed time = %ldms", i,m, t2);
        else sprintf(tx,"ReadSMS[%d,%d] elapsed time = %ldms", i,m, millis()-t2);
        Serial.println(tx);
        SMSprint(&MSMS_R.xsms[i-1]);
      }
      if(!MSMS_R.xsms[i-1].ok) {
        if(SMSdebugRD1) {
          Serial.println("!!!!!....skip null msg...........");
        }
        if(pam!=NULL) pam->alloc=false;
        continue;
      }
      stat=uint8_t(MSMS_R.xsms[i-1].status);
      if(stat > 1) {
        if(pam!=NULL) pam->alloc=false;
        if(SMSdebugRD1) sz=sprintf(tx,"bad typ, no store, delete...");
        ok=sms_delete(i, &t3);        
        if(ok) {
          if(SMSdebugRD1) sz+=sprintf(tx+sz,"OK");
        } else {
          if(SMSdebugRD1) sz+=sprintf(tx+sz,"ERROR");
        }
        if(SMSdebugRD1) {
            sz+=sprintf(tx+sz,", elapsed time= %ldms",t3);
            Serial.println(tx);
        }
      } else {
        if(pam!=NULL) {
          pam->typFROM=AMSG_TYP_FROM_SMS;
          if(SMStoMAIL) pam->typTO |= AMSG_TYP_TO_MAIL;
          if(SMStoMQTT) pam->typTO |= AMSG_TYP_TO_MQTT;
          if(SMStoFILE) pam->typTO |= AMSG_TYP_TO_FILE;
          snprintf(pam->to,XTO_SIZE,"%s",MGSMS.recipient);
          snprintf(pam->subject,XSUBJECT_SIZE,"%s",MGSMS.subject);
          snprintf(pam->from,XFROM_SIZE,"%s",MSMS_R.xsms[i-1].originatingAddress.c_str());
          snprintf(pam->msg,AMSG_MSG_SIZE,"%s",MSMS_R.xsms[i-1].message.c_str());
          snprintf(pam->smsTime,XSMSTIME_SIZE,"%s",MSMS_R.xsms[i-1].serviceCentreTimeStamp.c_str());
          pam->enabled=true;
          pam->alloc=false;
          ASTATE.sms_read_cnt_total++;
          ASTATE.sms_read_cnt_day++;
          if(SMSdebugRD1) sz=sprintf(tx,"store, delete...");
          ok=sms_delete(i, &t3);
          if(SMSdebugRD1) {
            if(ok) sz+=sprintf(tx+sz,"OK");
            else sz+=sprintf(tx+sz,"ERROR");
            sz+=sprintf(tx+sz,", elapsed time= %ldms",t3);
            Serial.println(tx);
          }
        } else {
          if(SMSdebugRD1) Serial.println("no free mem");
        }
      }
  }
  if(SMSdebugRD1) {
    sprintf(tx,"end ReadSMS elapsed time = %ldms", millis()-t1);
    Serial.println(tx);
    Serial.println("================================");
  }
}
//-----------------------------------------------------------------
uint8_t msms_getRDcnt(void) {
  return MSMS_R.cnt;
}
//-----------------------------------------------------------------
Sms msms_getRDindx(uint8_t i) {
  return MSMS_R.xsms[i];
}
//-----------------------------------------------------------------
String msms_readV1(void) {
  uint8_t i;
  char tx[20];
  String out;
  Sms sms;
  out+="'";
  for(i=1;i<=MSMS_R.cnt;i++) {
    sms=MSMS_R.xsms[i-1];
    out+="<tr><td colspan=\"";
    out+="3\">";
    out+=sms.serviceCentreTimeStamp;
    out+="</td></tr>";
    out+="<tr><td>";
    out+=sms.originatingAddress;    
    out+="</td><td>";
    out+=sms.message;
    out+="</td><td>";
    out+=msms_typ2str(uint8_t(sms.status));
    out+="</td></tr>";
  }
  out+="';";
  return out;
}
//-----------------------------------------------------------------
//##############################################################
//
//    SEND
//
#define SMSWR_MAX 10
//#define SMSWR_ID  12345
struct msms_sendOne {
  bool enabled;
  bool sended;
  bool cnt1;
  char telef[24];
  char mess[100];
};
struct msms_send {
  struct msms_sendOne xsms[SMSWR_MAX];
};
struct msms_send MSMS_S;
//-----------------------------------------------------------------
void msms_printSend(void) {
  char tx[100];
  uint8_t i;
  struct msms_sendOne *ps;
  Serial.println("MSMS send bufer----------------");
  for(i=0;i<SMSWR_MAX;i++) {
    ps=&MSMS_S.xsms[i];
    if(ps->enabled) {
      sprintf(tx,"%u[%u,%u]> %s, %s",
        i+1,ps->sended,ps->cnt1,
        ps->telef,
        ps->mess
      );
      Serial.println(tx);
    }
  }
  Serial.println("MSMS send bufer............");
}
//-----------------------------------------------------------------
String msms_send_toJSON(uint8_t n) {
  char tx[10];
  struct msms_sendOne *ps;

  if(n>=SMSWR_MAX) return String();
  ps=&MSMS_S.xsms[n];
  if(!ps->enabled) return String();

  DynamicJsonDocument doc(300);

  sprintf(tx,"%d",n);
  doc["indx"] = tx;
  doc["telef"] = ps->telef;
  doc["msg"] = ps->mess;
  String out;
  serializeJson(doc, out);
  return out;
}
//-----------------------------------------------------------------
String msms_send_toHTMLone(uint8_t n) {
  char tx[310];
  const char *tx1="<tr><td><div align=\"left\">";
  const char *tx1b="<tr><td><div align=\"center\">";
  const char *tx2="</div></td></tr>";
  struct msms_sendOne *ps;
  String s;

  if(n>=SMSWR_MAX) return String();
  ps=&MSMS_S.xsms[n];
  if(!ps->enabled) return String();

  sprintf(tx,"%s%d:&nbsp&nbsptelef:&nbsp&nbsp%s%s",tx1,n,ps->telef,tx2);
  s=tx;
  //s+="<tr><td><textarea>";
  s+="<tr><td><div id=\"textBox\">";
  s+=TextToHTML(ps->mess);
  //s+="</textarea></td></tr>";
  s+="</div></td></tr>";
  //Serial.println("SMStohtml:");
  //Serial.println(s);
  return s;
}
String msms_send_toHTML(void) {
  String s;
  uint8_t i;
  for(i=0;i<SMSWR_MAX;i++) {
    s+=msms_send_toHTMLone(i);
  } 
  return s; 
}
//-----------------------------------------------------------------
struct msms_sendOne *msms_getFree_S(void) {
  uint8_t i;
  struct msms_sendOne *ps;
  for(i=0;i<SMSWR_MAX;i++) {
    ps=&MSMS_S.xsms[i];
    if(!ps->enabled) {
      return ps;
    }
  }
  return NULL;
}
//-----------------------------------------------------------------
bool msms_addS(String telef, String mess) {
  struct msms_sendOne *ps;
  ps=msms_getFree_S();
  if(ps==NULL) return false;
  memset(ps,0,sizeof(struct msms_sendOne));
  sprintf(ps->telef,"%s",telef.c_str());
  sprintf(ps->mess,"%s",mess.c_str());
  ps->enabled=true;
  Serial.println("SMS add --------------");
  msms_printSend();
  return true;
}
//#####################################################################





//#####################################################################
String processorSMS1(const String& var) {
  char tx[100];
  //Serial.println(var);
  if(var == "PLACEHOLDER_SMSRECV"){
    return msms_readV1();
  }
  return procGetMGSMS(var);
}
//=================================================================
void procPostSMSsend(AsyncWebServerRequest *request) {
   String keyVal;
   byte mam=0;
   String telefon;
   String message;
   if (request->hasParam("sms01",true)) {
      telefon= request->getParam("sms01",true)->value();
      mam++;
   }
   if (request->hasParam("sms02",true)) {
      message= request->getParam("sms02",true)->value();
      mam++;
   }
   if(mam==2) {
      msms_addS(telefon,message);
   }
} 
//############################################################################
//
//    TASK
//
void smsTask( void * pvParameters ) {
  uint8_t i;
  char tx[100];
  struct msms_sendOne *ps;
  bool old_statePower = statePower;
  long now=0;
  long t1;
  long last_read=0;
  long last_send=0;
  //delay(3000);
  while(!myModemInit()) {
    delay(2000);
  }
  SMSforceRead=true;
  while(1) {
    if(MGSMS.cfg & MGSMSenabled) {
      //------------- send -------------------
      if(ASTATE.sms_send_cnt_day < MGSMS.maxSMSday) {
        for(i=0;i<SMSWR_MAX;i++) {
          ps=&MSMS_S.xsms[i];
          if(ps->enabled) {
            if(ps->cnt1 > MGSMS.replys) {
              ps->enabled=false;
            } else {
              now = millis();
              if(now - last_send > ((long)MGSMS.resend * 1000)) {
                last_send = now;
                t1=millis();
                if(SMSdebugWR1) {
                  Serial.print("send sms tel<");
                  Serial.print(ps->telef);
                  Serial.print("> cnt:");
                  Serial.println(ps->cnt1);
                }
                if(modem.sendSMS(String(ps->telef), String(ps->mess))) {
                    last_send = now;
                    ps->enabled=false;
                    ASTATE.sms_send_cnt_total++;
                    ASTATE.sms_send_cnt_day++;
                    if(SMSdebugWR1) Serial.println("...sms send OK");
                    break;
                } else{
                    if(SMSdebugWR1) Serial.println("...sms send Error!!!");
                    ps->cnt1++;
                    if(ps->cnt1 > MGSMS.replys) ps->enabled=false;
                }
                if(SMSdebugWR1) {
                    sprintf(tx,"end SendSMS elapsed time = %ldms", millis()-t1);
                    Serial.println(tx);
                }
              }
            }
          }
        }
      }
      //------------- recv-------------------
      now = millis();
      if( (now - last_read > ((long)MGSMS.rerecv * 1000)) || SMSforceRead) {
          SMSforceRead=false;
          last_read = now;
          msms_read();
      }
      //------------- deleteAll-------------------
      sms_deleteAllproc();
      //-------------------------------------
    }
    delay(200);
  }
  vTaskDelete( NULL );
}
//############################################################################
//
//    INIT
//
void msms_init(void) {
  //.............. send SMS ................
  memset(&MSMS_S,0,sizeof(struct msms_send));
  //msms_printSend();
  //.............task ....................
  xTaskCreate(
      smsTask,          // Task function.
      "smsTask",        // String with name of task.
      15000,            // Stack size in bytes.
      NULL,             // Parameter passed as input of the task
      1,                // Priority of the task.
      NULL);            // Task handle.
}
//############################################################################
