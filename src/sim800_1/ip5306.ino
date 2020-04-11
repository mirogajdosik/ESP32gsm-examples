
byte batReg[20];
long IP5306_lastTestPower = 0;
int IP5306_refrTESTPOWER = 10;

byte IP5306readOne(byte adr){
  byte x;
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(adr);
  Wire.endTransmission();
  Wire.requestFrom(IP5306_ADDR,1);
  x=Wire.read();
  return x;
}

void IP5306testPower(boolean force) {
  long now = millis();
  if( (now - IP5306_lastTestPower > (IP5306_refrTESTPOWER * 1000)) || (force) ) {
    IP5306_lastTestPower = now;
    batReg[8]=IP5306readOne(0x70);   //REG_READ0
    batReg[9]=IP5306readOne(0x71);   //REG_READ1
    batReg[10]=IP5306readOne(0x72);   //REG_READ2
    if(batReg[8] & 8) stateCharger=true;
    else stateCharger=false;
    if(batReg[9] & 0xE0) statePower=true;
    else statePower=false;  
    batReg[12]=IP5306readOne(0x78);   //REG_READ4
    switch(batReg[12] & 0xF0) {
    case 0xE0:
      batLevel=25;
      batState=2;
      break;
    case 0xC0:
      batLevel=50;
      batState=1;
      break;
    case 0x80:
      batLevel=75;
      batState=0;
      break;
    case 0x00:
      batLevel= 100;
      batState=0;
      break;
    default:
      batLevel=0;
      batState=3;
    }
    if(batReg[12] & 0x10) batLow1=true;
    else batLow1 = false;
    if(batReg[12] & 0x02) batLow2=true;
    else batLow2 = false;
  }
}

void IP5306readALL(void) {
  batReg[15]++;  

  batReg[0]=IP5306readOne(0);    //SYS_CTL0
  batReg[1]=IP5306readOne(1);    //SYS_CTL1
  batReg[2]=IP5306readOne(2);    //SYS_CTL2
  batReg[3]=IP5306readOne(0x20);   //Charger_CTL0
  batReg[4]=IP5306readOne(0x21);   //Charger_CTL1
  batReg[5]=IP5306readOne(0x22);   //Charger_CTL1
  batReg[6]=IP5306readOne(0x23);   //Charger_CTL1
  batReg[7]=IP5306readOne(0x24);   //Charger_DIG_CTL0
  batReg[8]=IP5306readOne(0x70);   //REG_READ0
  batReg[9]=IP5306readOne(0x71);   //REG_READ1
  batReg[10]=IP5306readOne(0x72);   //REG_READ2
  batReg[11]=IP5306readOne(0x77);   //REG_READ3
  batReg[12]=IP5306readOne(0x78);   //REG_READ4
}

void IP5306printALL(void) {
  char tx[100];
  sprintf(tx,"SYS_CTL= %02X,%02X,%02X\n",batReg[0],batReg[1],batReg[2]);
  Serial.print(tx);
  sprintf(tx,"Charder_CTL= %02X,%02X,%02X,%02X\n",batReg[3],batReg[4],batReg[5],batReg[6]);
  Serial.print(tx);
  sprintf(tx,"Charder_DIG_CTL= %02X\n",batReg[7]);
  Serial.print(tx);
  sprintf(tx,"REG_READ= %02X,%02X,%02X,%02X,%02X\n",batReg[8],batReg[9],batReg[10],batReg[11],batReg[12]);
  Serial.print(tx);
}

void IP5306readPrintAll(void) {
  IP5306readALL();
  IP5306printALL();
}
