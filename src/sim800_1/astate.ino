//##############################################################
struct me_date {
  byte den;
  byte mes;
  byte rok;
  byte hod;
  byte min;
  byte sec;
  byte den7;
  byte free;
};
//##############################################################
//
//    STATE
#define ASTATE_ID       0xDEAD3272
struct astate {
    uint32_t id;
    uint16_t reset_cnt;
    struct me_date actual;
    bool rdTime;
    uint16_t sms_read_cnt_total;
    uint16_t sms_read_cnt_day;
    uint16_t sms_send_cnt_total;
    uint16_t sms_send_cnt_day;
    uint16_t mail_read_cnt_total;
    uint16_t mail_read_cnt_day;
    uint16_t mail_send_cnt_total;
    uint16_t mail_send_cnt_day;
    uint16_t mqtt_read_cnt_total;
    uint16_t mqtt_read_cnt_day;
    uint16_t mqtt_send_cnt_total;
    uint16_t mqtt_send_cnt_day;
    uint16_t pop3_mailCount;
};
struct astate ASTATE __attribute__ ((section (".noinit")));

void astate_ini_time(void) {
  struct tm timeinfo;
  if(!ASTATE.rdTime) {
    if(getLocalTime(&timeinfo)) {
      ASTATE.actual.den=timeinfo.tm_mday;
      ASTATE.actual.mes=timeinfo.tm_mon+1;
      ASTATE.actual.rok=(timeinfo.tm_year % 100);
      ASTATE.actual.hod=timeinfo.tm_hour;
      ASTATE.actual.min=timeinfo.tm_min;
      ASTATE.actual.sec=timeinfo.tm_sec;
      ASTATE.rdTime=true;
    }
  }
  
}

void astate_init(void) {
  if(ASTATE.id != ASTATE_ID) {
    memset(&ASTATE,0,sizeof(struct astate));
    //MSMS_ST.reset_cnt = 0;
    ASTATE.id = ASTATE_ID;
    Serial.println("Device first run");
    astate_ini_time();
  }
  ASTATE.reset_cnt++;
  Serial.print("Device reset cnt: ");
  Serial.println(ASTATE.reset_cnt);
}


String astate_procGet(const String& var) {
  String s;
  char tx[200];

    
  if(var == "AST10"){
    sprintf(tx,"%02d/%02d/%02d %02d:%02d:%02d",
        ASTATE.actual.den,ASTATE.actual.mes,ASTATE.actual.rok,
        ASTATE.actual.hod,ASTATE.actual.min,ASTATE.actual.sec
    );
    return String(tx);
  }
  if(var == "AST11"){
    sprintf(tx,"%d",ASTATE.reset_cnt);
    return String(tx);
  }

  if(var == "AST20"){
    sprintf(tx,"<tr><td>SMS</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>",
      ASTATE.sms_read_cnt_total, ASTATE.sms_read_cnt_day,
      ASTATE.sms_send_cnt_total, ASTATE.sms_send_cnt_day
    );
    s=tx;
    sprintf(tx,"<tr><td>MAIL</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>",
      ASTATE.mail_read_cnt_total, ASTATE.mail_read_cnt_day,
      ASTATE.mail_send_cnt_total, ASTATE.mail_send_cnt_day
    );
    s+=tx;
    sprintf(tx,"<tr><td>MQTT</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>",
      ASTATE.mqtt_read_cnt_total, ASTATE.mqtt_read_cnt_day,
      ASTATE.mqtt_send_cnt_total, ASTATE.mqtt_send_cnt_day
    );
    s+=tx;
    sprintf(tx,"<tr><td>POP3</td><td colspan=\"4\">%d</td></tr>",
      ASTATE.pop3_mailCount
    );
    s+=tx;
    return s;
  }
  if(var == "AST40") return amsg_toHTML(0);
  if(var == "AST41") return amsg_toHTML(1);
  if(var == "AST42") return amsg_toHTML(2);
  if(var == "AST43") return amsg_toHTML(3);
  if(var == "AST44") return amsg_toHTML(4);
  if(var == "AST45") return amsg_toHTML(5);
  if(var == "AST46") return amsg_toHTML(6);
  if(var == "AST47") return amsg_toHTML(7);
  if(var == "AST48") return amsg_toHTML(8);
  if(var == "AST49") return amsg_toHTML(9);
  
  if(var == "AST50"){
    return msms_readV1();
  }
  if(var == "AST51"){
    return msms_send_toHTML();
  }
  
  if(var == "AST60") return msms_send_toJSON(0);
  if(var == "AST61") return msms_send_toJSON(1);
  if(var == "AST62") return msms_send_toJSON(2);
  if(var == "AST63") return msms_send_toJSON(3);
  if(var == "AST64") return msms_send_toJSON(4);
  if(var == "AST65") return msms_send_toJSON(5);
  if(var == "AST66") return msms_send_toJSON(6);
  if(var == "AST67") return msms_send_toJSON(7);
  if(var == "AST68") return msms_send_toJSON(8);
  if(var == "AST69") return msms_send_toJSON(9);
  
  /*
  if(var == "AST60") return msms_send_toHTML(0);
  if(var == "AST61") return msms_send_toHTML(1);
  if(var == "AST62") return msms_send_toHTML(2);
  if(var == "AST63") return msms_send_toHTML(3);
  if(var == "AST64") return msms_send_toHTML(4);
  if(var == "AST65") return msms_send_toHTML(5);
  if(var == "AST66") return msms_send_toHTML(6);
  if(var == "AST67") return msms_send_toHTML(7);
  if(var == "AST68") return msms_send_toHTML(8);
  if(var == "AST69") return msms_send_toHTML(9);
  */
  return String();

}
