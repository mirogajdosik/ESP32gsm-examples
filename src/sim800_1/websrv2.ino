#if isWEBSRV ==2
#include <WebAuthentication.h> // otherwise the IDE wont find generateDigestHash()
#define HTTP_REALM    "realm"
//#define HTTP_ONLY_BASIC
//#define HTTP_ONLY_DIGEST
#define HTTP_ADD_USER2
AsyncWebServer server(80);
//######################################################################
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
//=================================================================
bool Authentify(AsyncWebServerRequest *request, uint8_t xpass) {
  #ifdef HTTP_ONLY_DIGEST
  xpass |= 0x80;
  #endif
  #ifdef HTTP_ONLY_BASIC
  xpass &= 0x7F;
  #endif
  #ifdef HTTP_ADD_USER2
  xpass |= 2;
  #endif
  if(xpass & 0x80) {
    if(xpass & 1) {
      if(request->authenticate(generateDigestHash(WEBuser1, WEBpass1, HTTP_REALM).c_str())) return true;
    } 
    if(xpass & 2) {   
      if(!request->authenticate(generateDigestHash(WEBuser1, WEBpass1, HTTP_REALM).c_str())) return true;
    }
    request->requestAuthentication();
    return false;
  }
  if(xpass & 1) {
     if(request->authenticate(WEBuser1, WEBpass1)) return true;
  } 
  if(xpass & 2) {   
     if(request->authenticate(WEBuser2, WEBpass2)) return true;
  }
  request->requestAuthentication();
  return false;
}
//=================================================================
void procPostKey(AsyncWebServerRequest *request,const char* KEY) {
   String keyVal;
   if (request->hasParam(KEY,true)) {
      keyVal= request->getParam(KEY,true)->value();
      if(KEY=="br1") {
        if(keyVal=="1") {
            IP5306readALL();
        }
      }
   }
}
//#########################################################
void websrv_setup(void) {
  #if isMDNS
  if (MDNS.begin(WIFImdns)) {
    Serial.println("MDNS responder started");
  }
  #endif
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",  "text/html");
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",  "text/html");
  });
  server.on("/batrd", HTTP_POST, [](AsyncWebServerRequest * request){
    Serial.println("---POST /batrd");
    procPostKey(request,"br1");
    request->send(SPIFFS, "/index.html",  "text/html");
  });
  server.on("/fajax.html", HTTP_GET, [](AsyncWebServerRequest *request){
    
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", fajax_html_gz, fajax_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    
    //request->send(SPIFFS, "/fajax.html",  "text/html");
  });
  server.on("/scr1.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/scr1.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/css", (const char *)(const __FlashStringHelper *)PGstyle, NULL);
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/status.xml", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/xml", (const char *)(const __FlashStringHelper *)PGstatus, processorSTAT);
    request->send(SPIFFS, "/status.xml", String(), false, procGetGLOBAL);
  });
  //----------------------------------------------------------------------
  server.on("/eml.html", HTTP_GET, [](AsyncWebServerRequest *request){
    if(Authentify(request,0x83))   
    request->send(SPIFFS, "/eml.html", String(), false, procGetEML1);
  });
  server.on("/eml.html", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!Authentify(request,0x03)) return;
    procPostEML1(request);
    request->send(SPIFFS, "/eml.html", String(), false, procGetEML1);
  });
  //----------------------------------------------------------------------
  server.on("/asetup.html", HTTP_GET, [](AsyncWebServerRequest *request){
    if(Authentify(request,0x03))   
    request->send(SPIFFS, "/asetup.html", String(), false, procGetSET1);
  });
  server.on("/asetup.html", HTTP_POST, [](AsyncWebServerRequest * request){
    if(!Authentify(request,0x03)) return;
    procPostSET1(request);
    request->send(SPIFFS, "/asetup.html", String(), false, procGetSET1);
  });
  //----------------------------------------------------------------------
  server.on("/system.html", HTTP_GET, [](AsyncWebServerRequest *request){
    //if(Authentify(request,0x03))   
    request->send(SPIFFS, "/system.html", String(), false, procGetSET1);
  });
  server.on("/sysRESET", HTTP_POST, [](AsyncWebServerRequest * request){
    toReset=true;
    request->send(SPIFFS, "/system.html", String(), false, procGetSET1);
  });
  server.on("/sysDELETE", HTTP_POST, [](AsyncWebServerRequest * request){
    if(!Authentify(request,0x03)) return;
    if (request->hasParam("Fname",true)) {
      String keyVal= request->getParam("Fname",true)->value();
      //SPIFFS.remove(keyVal.c_str());
      mgfile_deleted(keyVal.c_str());
    }
    request->send(SPIFFS, "/system.html", String(), false, procGetSET1);
  });

  //----------------------------------------------------------------------
  server.on("/event.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/event.html", String(), false, procGetEVT);
  });
  server.on("/event.html", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!Authentify(request,0x03)) return;
    procPostEVT(request);
    request->send(SPIFFS, "/event.html", String(), false, procGetEVT);
  });
  server.on("/state.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/state.html", String(), false, astate_procGet);
  });
  //----------------------------------------------------------------------
  server.on("/sms.html", HTTP_GET, [](AsyncWebServerRequest *request){
    if(Authentify(request,0x03))   
    request->send(SPIFFS, "/sms.html", String(), false, processorSMS1);
  });
  server.on("/setMGSMS", HTTP_POST, [](AsyncWebServerRequest * request){
    if(!Authentify(request,0x03)) return;
    procPostMGSMS(request);
    request->send(SPIFFS, "/sms.html", String(), false, processorSMS1);
  });
  server.on("/smsSend", HTTP_POST, [](AsyncWebServerRequest * request){
    if(!Authentify(request,0x03)) return;
    procPostSMSsend(request);
    request->send(SPIFFS, "/sms.html", String(), false, processorSMS1);
  });
  server.on("/smsDele", HTTP_POST, [](AsyncWebServerRequest * request){
    //if(!Authentify(request,0x03)) return;
    SMSforceDelAll=true;
    request->send(SPIFFS, "/sms.html", String(), false, processorSMS1);
  });
  server.on("/smsRead", HTTP_POST, [](AsyncWebServerRequest * request){
    //if(!Authentify(request,0x03)) return;
    SMSforceRead=true;
    request->send(SPIFFS, "/sms.html", String(), false, processorSMS1);
  });
  //----------------------------------------------------------------------
  server.on("/smsrecv.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/smsrecv.txt", String(), false, NULL);
  });
  server.on("/smssend.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/smssend.txt", String(), false, NULL);
  });
  server.on("/msg.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/msg.txt", String(), false, NULL);
  });
  server.on("/event.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SPIFFS, "/event.txt", String(), false, NULL);
    request->send(SPIFFS, "/event.txt", F("text/plain"));
  });
  //----------------------------------------------------------------------
  server.on("/esp32sim800.pdf", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/esp32sim800.pdf", F("application/pdf"));
  });
  //----------------------------------------------------------------------
  server.on("/normal.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/normal.png", F("image/png"));
  });
  server.on("/warning.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/warning.png", F("image/png"));
  });
  server.on("/critical.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/critical.png", F("image/png"));
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon_ico_gz, favicon_ico_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    //request->send(SPIFFS, "/favicon.ico", "image/icon");
  });

  // attach filesystem root at URL /fs
  //server.serveStatic("/fs", SPIFFS, "/");

  server.onNotFound(notFound);
  server.begin();
}
//#########################################################
void websrv_loop(void) {
    //server.handleClient();
}
//#########################################################

#endif
