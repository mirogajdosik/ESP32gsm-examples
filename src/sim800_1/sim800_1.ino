
#define isWIFI    1
#define isPOP3  1
#define isSMTP  1
#define isIMAP  0
#define isNTP     1
#define isFTP     1
#define isGSM     1
#define isMQTT    1
#define isVERB    0
#define isTEMPOW  0
#define isBTSER   0
#define isSPIFFS  1
#define isWEBSRV  2 //0,1,2
#define isMDNS    1
#define isMENU    2   //0-off,1-normal,2-task
#define isLED1    1   //0-off,1-normal,2-pwm



#if isTEMPOW
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 17
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperatureOW = 0;
//float humidity = 0;
#endif

#include <ArduinoJson.h>

#include <Preferences.h>
Preferences prefs;

#if isBTSER
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
#endif

#if isSPIFFS
#include "SPIFFS.h"
#endif

#if isWIFI
#include <WiFi.h>
#endif

#if isNTP
#include "time.h"
//const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
#endif

#if isWEBSRV ==1
//#include <WiFiClient.h>
//#include <WebServer.h>
#elif isWEBSRV ==2
//#include <Arduino.h>
//#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#endif

#if isMDNS
#include <ESPmDNS.h>
#endif

#include "aam.h"

bool WIFIenabled=true;
bool WIFIconnected=false;
bool WIFIstatic=false;
char WIFIssid[XNAME_SIZE];
char WIFIpasw[XPASS_SIZE];
char WIFImdns[XNAME_SIZE];
char WIFIip[XIP_SIZE];
char WIFImask[XIP_SIZE];
char WIFIgate[XIP_SIZE];
char WIFIdns[XIP_SIZE];
char SNTPserver[XNAME_SIZE];

char WEBuser1[XUSER_SIZE];
char WEBuser2[XUSER_SIZE];
char WEBpass1[XPASS_SIZE];
char WEBpass2[XPASS_SIZE];
bool mqtt_enabled;
char mqtt_server[XIP_SIZE];
char mqtt_DEVname[XNAME_SIZE];
char mqtt_GLOBname[XNAME_SIZE];
char mqtt_user[XUSER_SIZE];
char mqtt_pasw[XPASS_SIZE];
uint32_t mqtt_refrSTATUS = 60;
uint32_t mqtt_refrSENSOR = 20;
uint8_t wifi_log = 0;
uint8_t mqtt_log = 1;
char BTname[XNAME_SIZE];

bool SMTPenabled;
uint8_t SMTPdebug;
char SMTPserverName[XNAME_SIZE];
uint16_t SMTPserverPort;
char SMTPuserName[XNAME_SIZE];
char SMTPpassword[XPASS_SIZE];
char SMTPsenderName[XNAME_SIZE];
char SMTPsubject[XSUBJECT_SIZE];
char SMTPrecipient[XRECIPIENT_SIZE];

bool IMAPenabled;
uint8_t IMAPdebug;
char IMAPserverName[XNAME_SIZE];
uint16_t IMAPserverPort;
char IMAPuserName[XNAME_SIZE];
char IMAPpassword[XPASS_SIZE];
char IMAPsubject[XSUBJECT_SIZE];

bool POP3enabled;
char POP3serverName[XNAME_SIZE];
uint16_t POP3serverPort;
char POP3userName[XUSER_SIZE];
char POP3password[XPASS_SIZE];
char POP3subject[XSUBJECT_SIZE];
uint16_t POP3reread;
bool POP3toFile;
bool POP3toSMS;
bool POP3toMQTT;
uint8_t POP3debug;


bool toReset=false;
int delayReset=3000;
bool statePower = true;
bool stateCharger = false;
bool batLow1 = false;
bool batLow2 = false;
uint8_t batLevel =0;
uint8_t batState =0;

#if isLED1
int led1Pin = 13;
bool stateLed1 = false;
#endif


#if isSMTP || isIMAP
#include "ESP32_MailClient.h"
#if isSMTP
void sendCallback(SendStatus info);
#endif
#if isIMAP
void readCallback(ReadStatus info);
void readEmail();
#endif

#endif

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";


// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

#include <Wire.h>
#define TINY_GSM_NO_GPRS
#include <TinyGsmClient.h>

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT  Serial1

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

//###########################################################################
bool setPowerBoostKeepOn(int en){
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) {
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    Wire.write(0x35); // 0x37 is default reg value
  }
  return Wire.endTransmission() == 0;
}
//=============================================================
#if isNTP
void printLocalTime(Stream *S) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    S->println("Failed to obtain time");
    return;
  }
  S->println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
#endif
//=============================================================
#if isGSM
bool myModemInit(void) {
  digitalWrite(MODEM_RST, LOW);
  delay(500);
  digitalWrite(MODEM_RST, HIGH);
  delay(3000);
  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if(!modem.restart()) return false;
  // use modem.init() if you don't need the complete restart
  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
  return true;
}
#endif //isGSM
//###########################################################################
//
//    SETUP
//
void websrv_setup(void);
void websrv_loop(void);
void msms_init(void);

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  // Keep power when running from battery
  Wire.begin(I2C_SDA, I2C_SCL);
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  astate_init();
  aval_resetALL();
  amsg_setup();
  
  syspar_print(&Serial);
  #if isSPIFFS 
  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }
  
  Serial.print("SPIFFS  total=");
  Serial.print(SPIFFS.totalBytes());
  Serial.print(", used=");
  Serial.println(SPIFFS.usedBytes());
  
  #endif
  
  #if isLED1 ==1
  pinMode(led1Pin, OUTPUT);
  #endif
  
  #if isWIFI
  setup_wifi();
  #endif

  #if isWEBSRV
  websrv_setup();
  #endif

  #if isMQTT
  MQTTsetup();
  #endif

  #if isBTSER
  SerialBT.begin(BTname); //Bluetooth device name
  Serial.print("The device <");
  Serial.print(BTname);
  Serial.println("> started, now you can pair it with bluetooth!");
  #endif

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  #if isGSM
  msms_init();
  #endif //isGSM
  

  IP5306readPrintAll();
  
  #if isMENU ==2
    xTaskCreate(
      menuTask,          /* Task function. */
      "menuTask",        /* String with name of task. */
      10000,            /* Stack size in bytes. */
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */
  #endif

  #if isPOP3
  POP3setup();
  #endif
  
  #if isFTP
  ftp_setup();
  #endif
  
  mgfile_reset();
  
  Serial.print("Heap size: ");
  Serial.println(ESP.getFreeHeap());

}
//======================================================================
void setup_wifi() {
  IPAddress local_IP(10, 0, 0, 5);
  IPAddress gateway(10, 0, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8); //optional
  IPAddress secondaryDNS(8, 8, 4, 4); //optional
  if(WIFIstatic) {
    local_IP.fromString(WIFIip);
    gateway.fromString(WIFIgate);
    subnet.fromString(WIFImask);
    primaryDNS.fromString(WIFIdns);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }
  }
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFIssid);

  WiFi.begin(WIFIssid, WIFIpasw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected to ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  #if isNTP
    configTime(gmtOffset_sec, daylightOffset_sec, SNTPserver);
    printLocalTime(&Serial);
  #endif
}
//###########################################################################
//
//    LOOP
//
void loop() {
  //============ mqtt =====================
  #if isMQTT
  //if(mqtt_enabled)
  MQTTloop();
  #endif
  //============ ftp server =====================
  #if isFTP
  ftp_loop();
  #endif
  //============ web server =====================
  #if isWEBSRV
  websrv_loop();
  #endif
  //============ menu =====================
  #if isMENU ==1
  MENUrecv();
  #endif
  //========to reset===================================
  if(toReset) {
    delay(delayReset);
    toReset=false;
    ESP.restart();
  }
  //===========================================
  IP5306testPower(false);
  delay(1);
}
//####################################################################
String TextToHTML(const char *tx) {
  String s;
  int i=0;
  char last=0;
  char z[2];
  z[1]=0;
  s="";
  while(i<600) {
    z[0]=tx[i];
    if(z[0]<0x20) {
      if(z[0]==0) break;
      //z[0]='?';
      //s+=z;
      
      if(z[0]==0x0A) {
        z[0]='\\';
        s+=z;
        z[0]='n';
        s+=z;
      } 
      if(z[0]==0x0D) {
        //s+="\r";
        z[0]='\\';
        s+=z;
        z[0]='r';
        s+=z;
      }
      

    } else if(z[0]=='\"') {
      if(last!='\\') {
        //s+="\"";
        z[0]='\\';
        s+=z;
        z[0]='"';
        s+=z;
      } else s+=z;
    } else if(z[0]=='\'') {
      if(last!='\\') {
        //s+="\'";
        z[0]='\\';
        s+=z;
        z[0]='\'';
        s+=z;
      } else s+=z;

    } else {
      s+=z;
    }
    last=tx[i];
    i++;
  }
  /*
  Serial.println("TextToHTML");
  Serial.print("in:  ");
  Serial.println(tx);
  Serial.print("out: ");
  Serial.println(s);
  */
  return s;
}
//#####################################################################################
