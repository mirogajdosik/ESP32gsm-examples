#if isFTP


#include <ESP8266FtpServer.h>
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

void ftp_setup() {
  Serial.println("FTP setup");
  ftpSrv.begin("esp32","root");    //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
}
void ftp_loop() {
  ftpSrv.handleFTP(); 
}
#endif //isFTP
