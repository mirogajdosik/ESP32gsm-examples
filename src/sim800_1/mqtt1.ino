#if isMQTT
#include <PubSubClient.h>
#define MQTTverbose   1
#define MQTTuseLWT    1
#define MQTTaddTime   1
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
long mqtt_lastMsgSTATUS = 0;
long mqtt_lastMsgSENSOR = 0;
bool mqtt_forceSendSTATUS = true;
bool mqtt_forceSendSENSOR = false;
bool mqtt_sendLWT = true;
bool mqtt_stopMQTT = false;
bool mqtt_forceSMSrd = false;
long lastReconnectAttempt = 0;
//######################################################################3
void MQTTsetup(void) {
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);
  lastReconnectAttempt = 0;
}
//========================================================================
void MQTTpublish(String topic, String msg) {
  if(!mqtt_enabled) return;  
  bool rb1=mqtt_client.publish(topic.c_str(), msg.c_str());
  if( ((mqtt_log > 0) && (!rb1)) || (mqtt_log>2) ) {
      if(rb1) Serial.print("Send Message to topic: ");
      else Serial.print("!!!Err Send Message to topic: ");
      Serial.print(topic.c_str());
      Serial.print(". Message: ");
      Serial.print(msg.c_str());
      Serial.println();
  }
  ASTATE.mqtt_send_cnt_total++;
  ASTATE.mqtt_send_cnt_day++;

}
//========================================================================
String MQTTgetTIME(void) {
  String s ="";
  char tx[40];
  #if isNTP
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return s;
  }
  // "2019-12-15T16:16:45";
  sprintf(tx,"%04d-%02d-%02dT%02d:%02d:%02d",
    timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday,
    timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec
  );
  s=tx;
  #endif
  return s;
}
//========================================================================
void MQTTsendSTATUS(void) {
  String o;  
  long now = millis();
  if( (now - mqtt_lastMsgSTATUS > ((long)mqtt_refrSTATUS * 1000)) || (mqtt_forceSendSTATUS) ) {
    mqtt_lastMsgSTATUS = now;
    mqtt_forceSendSTATUS=false;
    
    String topic = "stat/" + String(mqtt_DEVname) + "/STATUS5";
    DynamicJsonDocument doc(800);
    JsonObject stn  = doc.createNestedObject("StatusNET");
    stn["Hostname"] = WIFImdns;
    stn["IPAddress"] = WiFi.localIP().toString();
    stn["Gateway"] = WiFi.gatewayIP().toString();
    stn["Subnetmask"] = WiFi.subnetMask().toString();
    //stn["Mac"] =  WiFi.macAddress().toString();
    serializeJson(doc, o);
    MQTTpublish(topic, o);
  }
}
//========================================================================
void MQTTsendSENSOR(void) {
  String s;  
  String o;  
  char temp[8];
  char humi[8];
  sprintf(temp,"23.47");
  sprintf(humi,"44");

  long now = millis();
  if( (now - mqtt_lastMsgSENSOR > ((long)mqtt_refrSENSOR * 1000)) || (mqtt_forceSendSENSOR) ) {
    mqtt_lastMsgSENSOR = now;
    mqtt_forceSendSENSOR=false;
    String topic = "tele/" + String(mqtt_DEVname) + "/SENSOR";
    DynamicJsonDocument doc(800);
    
    #if MQTTaddTime
    s=MQTTgetTIME();
    if(s != "") {
        doc["Time"] = s.c_str();
    }
    #endif
    if(statePower) doc["Power"] = "ON";
    else doc["Power"] = "OFF";
    if(stateCharger) doc["Charger"] = "ON";
    else doc["Charger"] = "OFF";
    if(stateLed1) doc["Led1"] = "ON";
    else doc["Led1"] = "OFF";

    JsonObject sht  = doc.createNestedObject("SHT1");
    sht["Temperature"] = temp;
    sht["Humidity"] = humi;
    doc["TempUnit"] = "C";
    serializeJson(doc, o);
    MQTTpublish(topic, o);
  }
}
//######################################################################3
void mqtt_callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  if(mqtt_log > 3) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    Serial.print(messageTemp);
    Serial.println();
  }
  String mtopic;
  String prefix = "cmnd/" + String(mqtt_DEVname) + "/";
  //---------------------------------------------------------------
#if isLED1==1
  mtopic = prefix;
  mtopic += "led1";
  if (String(topic) == mtopic) {
    if(mqtt_log > 3) {
      Serial.print("Changing led to ");
    }
    if(messageTemp == "on"){
      if(mqtt_log > 3) {
        Serial.println("on");
      }
      digitalWrite(led1Pin, HIGH);
      stateLed1=true;
    } else if(messageTemp == "off"){
      if(mqtt_log > 3) {
        Serial.println("off");
      }
      digitalWrite(led1Pin, LOW);
      stateLed1=false;
    } else {
      return;  
    }
    mqtt_forceSendSENSOR=true;
    return;
  }  
#endif //isLED1==1
  //---------------------------------------------------------------
  mtopic = prefix + "sentMAIL";
  if (String(topic) == mtopic) {
      if(mqtt_log > 3) {
        Serial.print("sentMAIL: ");
        Serial.println(messageTemp);
      }
      DynamicJsonDocument doc(800);
      deserializeJson(doc, message, length);
      const char* recipient = doc["Recipient"];
      const char* subject = doc["Subject"];
      const char* msg = doc["Msg"];
      //if(mqtt_log > 3) {
        Serial.print("JSON recipient: ");
        Serial.print(recipient);
        Serial.print(", subject: ");
        Serial.print(subject);
        Serial.print(", msg: ");
        Serial.println(msg);
      //}      
      struct amsg *pam;
      pam=amsg_get_free();
      if(pam != NULL) {
          pam->typFROM=AMSG_TYP_FROM_MQTT;
          pam->typTO |= AMSG_TYP_TO_MAIL;
          snprintf(pam->msg,AMSG_MSG_SIZE,"%s",msg);
          snprintf(pam->subject,XSUBJECT_SIZE,"%s",subject);
          snprintf(pam->to,XTO_SIZE,"%s",recipient);
          pam->enabled=true;
          pam->alloc=false;
          ASTATE.mqtt_read_cnt_total++;
          ASTATE.mqtt_read_cnt_day++;
      }
      
      return;
  }
  //---------------------------------------------------------------
  mtopic = prefix + "sentSMS";
  if (String(topic) == mtopic) {
      if(mqtt_log > 3) {
        Serial.print("sentSMS: ");
        Serial.println(messageTemp);
      }
      DynamicJsonDocument doc(800);
      deserializeJson(doc, message, length);
      const char* telef = doc["Telef"];
      const char* msg = doc["Msg"];
      if(mqtt_log > 3) {
        Serial.print("JSON telef: ");
        Serial.print(telef);
        Serial.print(", msg: ");
        Serial.println(msg);
      }
      struct amsg *pam;
      pam=amsg_get_free();
      if(pam != NULL) {
          pam->typFROM=AMSG_TYP_FROM_MQTT;
          pam->typTO |= AMSG_TYP_TO_SMS;
          snprintf(pam->msg,AMSG_MSG_SIZE,"%s",msg);
          snprintf(pam->telef,XTELEF_SIZE,"%s",telef);
          pam->enabled=true;
          pam->alloc=false;
          ASTATE.mqtt_read_cnt_total++;
          ASTATE.mqtt_read_cnt_day++;
      }
      return;
  }  
  //---------------------------------------------------------------
  mtopic = prefix + "delaySTATUS";
  if (String(topic) == mtopic) {
      if(mqtt_log > 3) {
        Serial.print("Set DEVrefrSTATUS: ");
        Serial.println(messageTemp);
      }
      mqtt_refrSTATUS = atoi(messageTemp.c_str());
      if(mqtt_refrSTATUS < 2) mqtt_refrSTATUS=2;
      aval_EEsave(&TABVP_SET1[0], "MQT06");      
  }  
  //---------------------------------------------------------------
  mtopic = prefix + "delaySENSOR";
  if (String(topic) == mtopic) {
      if(mqtt_log > 3) {
        Serial.print("Set DEVrefrSENSOR: ");
        Serial.println(messageTemp);
      }
      mqtt_refrSENSOR = atoi(messageTemp.c_str());
      if(mqtt_refrSENSOR < 2) mqtt_refrSENSOR=2;
      aval_EEsave(&TABVP_SET1[0], "MQT07");      
      return;
  }  
  //---------------------------------------------------------------
  mtopic = "cmnd/" + String(mqtt_GLOBname) + "/STATUS";
  if (String(topic) == mtopic) {
      if(mqtt_log > 3) {
        Serial.print("RCV GLOBname, msg:");
        Serial.println(messageTemp);
      }
    mqtt_forceSendSTATUS = true;
    return;
  }
  //---------------------------------------------------------------
  if (String(topic) == "cmnd/sonoffs/STATUS") {
    if(mqtt_log > 3) {
      Serial.print("RCV cmnd/sonoffs/STATUS, msg:");
      Serial.println(messageTemp);
    }
    mqtt_forceSendSTATUS = true;
    return;
  }
  if (String(topic) == "cmnd/tasmotass/STATUS") {
    if(mqtt_log > 3) {
      Serial.print("RCV cmnd/tasmotass/STATUS, msg:");
      Serial.println(messageTemp);
    }
    mqtt_forceSendSTATUS = true;
    return;
  }
}
//############################################################################
#if MQTTuseLWT
byte willQoS = 1;
//const char* willTopic = "tele/ESP32-1/LWT";
char willTopic[80];
const char* willMessage = "Offline";
boolean willRetain = true;
#endif
boolean MQTTreconnect() {
    Serial.print("Attempting MQTT connection...");
   // Create a random client ID
    String clientId = "ESP32gsmClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    #if MQTTuseLWT
    sprintf(willTopic,"tele/%s/LWT",mqtt_DEVname);
    if (mqtt_client.connect(clientId.c_str(), willTopic, willQoS, willRetain, willMessage) ) {
    #else
    if (mqtt_client.connect(clientId.c_str())) {
    #endif  
      Serial.println("connected");
      #if MQTTuseLWT
      mqtt_client.publish(willTopic,"Online", true);
      #endif
      // Subscribe
      String topic;
      topic = "cmnd/" + String(mqtt_DEVname) + "/#";
      mqtt_client.subscribe(topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      return false;
    }
  return mqtt_client.connected();
}
//############################################################################
void MQTTloop(void) {
  if(!mqtt_enabled) return;
  if (!mqtt_client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (MQTTreconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqtt_client.loop();
    MQTTsendSTATUS();
    MQTTsendSENSOR();
  }
}
//############################################################################
#endif //isMQTT
