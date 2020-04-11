
struct file_def FILEDEF[] ={
  { "/smsrecv.txt", 20000 },
  { "/smssend.txt", 20000 },
  { "/msg.txt", 100000 },
  { "/event.txt", 100000 },
  { "", 0 }
};
//==============================================================
size_t mgfile_size(const char *path) {
  size_t sz;
  File f = SPIFFS.open(path);
  if(!f) {
    Serial.print("Error open file: ");
    Serial.println(path);
    return -1;
  }
  sz=f.size();
  f.close();
  return sz;
}
//==============================================================
bool mgfile_save_tx(uint8_t n, const char *tx) {
  size_t rdsz;
  if(n>3) return false;
  struct file_def *pfd=&FILEDEF[n];
  
  rdsz=mgfile_size(pfd->fname);
  if(rdsz > pfd->maxSize) {
    Serial.print("Error file size: ");
    Serial.println(pfd->fname);
    return false;
  }
  File f = SPIFFS.open(pfd->fname, FILE_APPEND);
  if(!f) {
    Serial.print("Error open file: ");
    Serial.println(pfd->fname);
    return false;
  }
  if(!f.print(tx)) {
    Serial.print("Error append to file: ");
    Serial.println(pfd->fname);
    f.close();  
    return false;
  }
  f.close();  
  return true;
}
//==============================================================
bool mgfile_save_time(uint8_t n) {
  char tx[100];
  #if isNTP
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
      sprintf(tx,"<no time> ");
  } else {
      sprintf(tx,"<%02d/%02d/%02d %02d:%02d:%02d> ",
        timeinfo.tm_mday, timeinfo.tm_mon,(timeinfo.tm_year % 100),
        timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec
      );
  }
  #else
  sprintf(tx,"<no NTP> ");
  #endif
  return mgfile_save_tx(n, tx);
}
//==============================================================
void mgfile_save_msg(uint8_t n, const char *msg) {
  mgfile_save_time(n);
  mgfile_save_tx(n, msg);
  mgfile_save_tx(n, "\r\n");
}
//==============================================================
void mgfile_deleted(const char *fname) {
  uint8_t i;
  struct file_def *pfd;
  for(i=0;i<FILEDEFmax;i++) {
    pfd=&FILEDEF[i];
    if(strcmp(fname,pfd->fname)==0) {
        SPIFFS.remove(fname);
        mgfile_save_time(i);
        mgfile_save_tx(i, "Deleted\r\n");
        return;      
    }
  }
}
//==============================================================
void mgfile_reset(void) {
  uint8_t i;
  for(i=0;i<FILEDEFmax;i++) {
     mgfile_save_tx(i,"\r\n");
     mgfile_save_msg(i,"RESET");
  }
  //SPIFFS.remove("/smsread.txt");
}
//==============================================================




//####################################################################
//List all files in SD card
void printDirectory(File &dir, int depth)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
      break;

    for (uint8_t i = 0; i < depth; i++)
      Serial.print("|    ");

    std::string name = entry.name();
    if (entry.isDirectory())
    {
      Serial.print("+----" + String(name.substr(name.find_last_of("/\\") + 1).c_str()) + "\r\n");
      printDirectory(entry, depth + 1);
    }
    else
    {
      Serial.print("+--" + String(name.substr(name.find_last_of("/\\") + 1).c_str()));
      Serial.print("\t\t\t(");
      Serial.print(entry.size(), DEC);
      Serial.println(")");
    }
    entry.close();
  }
}
//=====================================================================================
void printSPIFFS_1(void) {
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();
 
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());
 
      file = root.openNextFile();
  }
}
//=====================================================================
void printSPIFFS_2(void) {
  File root = SPIFFS.open("/");
  printDirectory(root, 0);
}
//#####################################################################
