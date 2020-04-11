#if isPOP3

#define POP3cmdIO       0
#define POP3delMain     1
#define POP3viewMSGALL  1

#define POP3debugLev1   1
#define POP3debugLev2   2
#define POP3debugMain   4
#define POP3debugAll    8
WiFiClient pop3_client;
//uint8_t POP3debug=POP3debugMain;
bool POP3forceRead=false;
char rtnBuf[100];
struct amsg POP3MSG_0;

const char* bound1 = "--{BOUNDARY}"; 
//====================================================
byte POP3read(void) {
  byte thisByte = 0;
  byte respCode;
  char rtnVal[64];
  int mailCount,mailSize;
  char tBuf[64];

  if(POP3debug & POP3debugLev1) Serial.print(F("POP3 read "));
  if(!POP3enabled) {
    if(POP3debug & POP3debugLev1) Serial.println(F("disabled!!"));
    return 0;
  }
  if(POP3debug & POP3debugLev1) Serial.println(F("..."));
  
  if(pop3_client.connect(POP3serverName,POP3serverPort) == 1) {
    if(POP3debug & POP3debugLev2) Serial.println(F("connected"));
  } else {
    if(POP3debug & POP3debugLev2) Serial.println(F("connection failed"));
    return 0;
  }

  if(!POP3rcv()) return 0;
  if(POP3debug & POP3debugLev2) Serial.println(rtnBuf);

  if(POP3debug & POP3debugLev2) Serial.println(F("Sending User"));
  strcpy_P(tBuf,PSTR("USER "));  
  strcat(tBuf,POP3userName);
  strcat_P(tBuf,PSTR("\r\n"));  
  pop3_client.write(tBuf);
  if(!POP3rcv()) return 0;
  if(POP3debug & POP3debugLev2) {
    Serial.println(rtnBuf);
    Serial.println(F("Sending Password"));
  }
  strcpy_P(tBuf,PSTR("PASS "));  
  strcat(tBuf,POP3password);
  strcat_P(tBuf,PSTR("\r\n"));  
  pop3_client.write(tBuf);
  if(!POP3rcv()) return 0;
  if(POP3debug & POP3debugLev2) {
    Serial.println(rtnBuf);
    Serial.println(F("Sending STAT"));
  }
  strcpy_P(tBuf,PSTR("STAT\r\n"));  
  pop3_client.write(tBuf);
  if(!POP3rcv()) return 0;
  if(POP3debug & POP3debugLev2) Serial.println(rtnBuf);

  sscanf(rtnBuf,"%s %u %u",rtnVal,&mailCount,&mailSize);
  if(POP3debug & POP3debugLev1) {
    Serial.print(F("mail count: "));
    Serial.println(mailCount);
  }
  ASTATE.pop3_mailCount=mailCount;
  for(int i = 1;i<=mailCount;i++) {
    strcpy_P(tBuf,PSTR("RETR "));  
    itoa(i,rtnBuf,10);
    strcat(tBuf,rtnBuf);
    strcat(tBuf,"\r\n");
    pop3_client.write(tBuf);
    //read message
    if(POP3parseMail(&POP3MSG_0) == 2) {
      //------- forMe or telef ----------------------------------
      struct amsg *pam=amsg_get_free();
      if(pam!=NULL) {
          #if POP3delMain 
          strcpy(tBuf,"DELE ");       
          itoa(i,rtnBuf,10);
          strcat(tBuf,rtnBuf);
          strcat(tBuf,"\r\n");
          pop3_client.write(tBuf);
          if(POP3rcv()) {
              if(POP3debug & POP3debugLev1) Serial.println("Deleted");
          } else {
             if(POP3debug & POP3debugLev1) Serial.println("Not deleted");
          }
          #endif
              memcpy(pam,&POP3MSG_0,sizeof(struct amsg));
              pam->typFROM=AMSG_TYP_FROM_MAIL;
              
              if(POP3toSMS) pam->typTO |= AMSG_TYP_TO_SMS;
              if(POP3toMQTT) pam->typTO |= AMSG_TYP_TO_MQTT;
              if(POP3toFile) pam->typTO |= AMSG_TYP_TO_FILE;
              pam->enabled=true;
              ASTATE.mail_read_cnt_total++;
              ASTATE.mail_read_cnt_day++;

          pam->alloc=false;
          if(POP3debug & POP3debugMain) amsg_print(&POP3MSG_0);
      }
    } else {
      if(POP3debug & POP3debugAll) amsg_print(&POP3MSG_0);
    }
    if(POP3debug & POP3debugLev2) Serial.println(F("\r\nEND"));
  }

  if(POP3debug & POP3debugLev2) Serial.println(F("Sending QUIT"));
  strcpy_P(tBuf,PSTR("QUIT\r\n"));  
  pop3_client.write(tBuf);
  if(!POP3rcv()) return 0;
  if(POP3debug & POP3debugLev2) Serial.println(rtnBuf);

  pop3_client.stop();
  if(POP3debug & POP3debugLev2) Serial.println(F("disconnected"));
  return 1;
}
//==============================================================
byte POP3rcv(void) {
  byte respCode;
  byte thisByte;
  int loopCount = 0;
  byte rtnCount = 0;
  
  while(!pop3_client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      pop3_client.stop();
      if(POP3debug & POP3debugLev2) Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
  while(pop3_client.available()) {  
    thisByte = pop3_client.read();    
//    Serial.write(thisByte);
    if(rtnCount < 99) {
      rtnBuf[rtnCount]=thisByte;
      rtnCount++;
      rtnBuf[rtnCount]=0;
    }
  }
  if(rtnBuf[0] == '-') return 0;
  return 1;
}
//=======================================================
#define MLST_hdr            1
#define MLST_boundaryStart  2
#define MLST_msg            3
#define MLST_boundaryEnd    4
#define MLST_end            10

byte POP3parseMail(struct amsg *pm) {
  byte state = 0;
  byte thisByte;
  int loopCount = 0;
  byte rtnCount = 0;
  byte lineLength = 0;
  uint16_t msgLength = 0;
  bool isBoundary = false;
  char lineBuf[200];
  #if POP3cmdIO
  byte pinNo;
  char pinState[8];
  #endif
  
  while(!pop3_client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      pop3_client.stop();
      if(POP3debug & POP3debugLev2) Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
  //...........................................................
  memset(pm,0,sizeof(struct amsg));
  //while(!endMsg) {
  while(state < MLST_end) {
    while(pop3_client.available()) {  
      thisByte = pop3_client.read(); 
      if(state == MLST_msg) {
        if(msgLength < (AMSG_MSG_SIZE-1)) {
          pm->msg[msgLength] = thisByte;
          msgLength++;
          pm->msg[msgLength] = 0;
        }
        //Serial.write(thisByte);
      }

      if(thisByte == '\n') {
        //----------------------- end of line -------------------------------------------
        if(strlen(lineBuf) == 0) {
              //-------------- null line ------------------------
              if(state < MLST_hdr) {
                  //if(POP3debug & POP3debugLev2) {
                  //Serial.println("Message end hdr");
                  //}
                  state = MLST_hdr;
              } else if(state == MLST_boundaryStart) {
                  state = MLST_msg;
              } else if(state == MLST_msg) {
                  state = MLST_end;
              }
        } else {      
            //------???------------------
            if(strcmp(lineBuf,".") == 0) {
                  // end of message
                  state = MLST_end;
            }
            //.......... parse Header ..................
            if(state < MLST_hdr) {
                if(strncmp(lineBuf,"From:",5) == 0) {
                    // from
                    snprintf(pm->from,XFROM_SIZE,"%s",&lineBuf[6]);
                    if(POP3debug & POP3debugLev2) Serial.println(lineBuf);
                }
                if(strncmp(lineBuf,"Subject:",8) == 0) {
                    // subject
                    if(POP3debug & POP3debugLev2) Serial.println(lineBuf);
                    snprintf(pm->subject,XSUBJECT_SIZE,"%s",&lineBuf[9]);

                    if(strcmp(&lineBuf[9],POP3subject) == 0) {
                        if(POP3debug & POP3debugLev2) Serial.println("subject For me!");
                        //forMe= true;
                        pm->subjForMe=true;
                    }
                    //Serial.println("Test telef 1");
                    if(amsg_testTELEF(&lineBuf[9], pm->telef)) {
                        pm->okTelef=true;
                    }
                    //Serial.println("---");
                }
            } else if(state == MLST_hdr) {
                if(strcmp(lineBuf,bound1) == 0) {
                   state=MLST_boundaryStart;
                   isBoundary=true;
                } else {
                   msgLength=sprintf(pm->msg,"%s",lineBuf);
                   state=MLST_msg;
                }
            } else if(state == MLST_msg) {
                if(pm->subjForMe && pm->okTelef==false) {
                    //Serial.println("Test telef 2");
                    if(amsg_testTELEF(lineBuf, pm->telef)) {
                        pm->okTelef=true;
                        msgLength=0;
                    }
                    //Serial.println("---");
                }
        
                #if POP3cmdIO
                if(msgMsg && strncmp(lineBuf,"command ",8) == 0) {
                    if(POP3debug & POP3debugLev2) Serial.print("Command! D");          
                        sscanf(lineBuf,"%*s %u %s",&pinNo,pinState);
                        if(POP3debug & POP3debugLev2) {
                            Serial.print(pinNo);
                            Serial.print(" ");
                            Serial.println(pinState);
                    }
                    if(strcmp(pinState,"on") == 0) digitalWrite(pinNo,HIGH);          
                    if(strcmp(pinState,"off") == 0) digitalWrite(pinNo,LOW);          
                }
                #endif
            }
        }
        lineLength = 0;
        lineBuf[0] = 0;
      }
      else if(thisByte != '\r') {
        // another character
        if(lineLength < 199) {
          lineBuf[lineLength] = thisByte;
          lineLength++;
          lineBuf[lineLength] = 0;
        }
      }
    }
  }

  if(pm->okTelef || pm->subjForMe) return 2;
  return 1;
}
//=======================================================
void POP3efail(void) {
  byte thisByte = 0;
  int loopCount = 0;

  pop3_client.println("QUIT");

  while(!pop3_client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      pop3_client.stop();
      #if POP3debug
      Serial.println(F("\r\nTimeout"));
      #endif
      return;
    }
  }

  while(pop3_client.available())
  {  
    thisByte = pop3_client.read();    
    if(POP3debug & POP3debugLev2) Serial.write(thisByte);
  }
  pop3_client.stop();
  if(POP3debug & POP3debugLev2) Serial.println(F("disconnected"));
}
//##########################################################################
void pop3Task( void * pvParameters ) {
  long now=0;
  long last_read=0;
  POP3forceRead=false;
  
  while(1) {
    now = millis();
    if( (now - last_read > ((long)POP3reread * 1000)) || (POP3forceRead) ) {
      last_read = now;
      POP3forceRead=false;
      if(POP3enabled) {
        #if POP3debugViewML
        //Serial.println("POP3 start read");
        #endif
        POP3read();
      }
    }
    delay(1000);
  }
}
//===========================================================
void POP3setup(void) {
  //if(POP3enabled) {
    xTaskCreate(
      pop3Task,          // Task function.
      "pop3Task",        // String with name of task.
      10000,            // Stack size in bytes.
      NULL,             // Parameter passed as input of the task
      1,                // Priority of the task.
      NULL);            // Task handle.
  //}
}
//##########################################################################
#endif //isPOP3
