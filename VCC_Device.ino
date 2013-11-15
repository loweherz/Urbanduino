/**************
  ALIMENTAZIONI
**************/
#define VCC_RELE    10
#define VCC_GSM     10

void initVCCChannel(){
  pinMode(VCC_RELE, OUTPUT);
  pinMode(VCC_GSM, OUTPUT);
}

void turnONDevices(){
  #ifdef SERIAL_DEBUG
    showString(PSTR("TURN ON\n"));
  #endif
  digitalWrite(VCC_RELE, HIGH);  
  delay(2000);
}

void turnON_VCC_GSM(){
  digitalWrite(VCC_GSM, HIGH);
  delay(2000);
}

void turnOFFDevices(){
  #ifdef SERIAL_DEBUG
    showString(PSTR("TURN OFF\n"));
  #endif
  digitalWrite(VCC_RELE, LOW);
  delay(2000);
}

void turnOFF_VCC_GSM(){
  digitalWrite(VCC_GSM, LOW);
  delay(2000);
}
