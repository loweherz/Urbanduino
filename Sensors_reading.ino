void readDHT(){
  //DHT.read22(DHT22_PIN);//leggo dal sensore e ritorna un valore corrispondente all'andamento della//leggo dal sensore e ritorna un valore corrispondente all'andamento della lettura

  if(DHT.read22(DHT22_PIN)==0){  //valore di ritorno : 0 -> Lettura andata a buon fine
    temp_sum = DHT.temperature;
    hum_sum = DHT.humidity;
    
    check_vect[HUM] = 1;
    check_vect[TMP] = 1;
  }
  else{    
    check_vect[HUM] = 0;
    check_vect[TMP] = 0;
  }
}

/*void readMIC(){
  int micsum2=0;
  int micsum1=0;
  for(int l=1;l<21;l++)
  {
    micsum1=analogRead(3);
    if(micsum1>micsum2){
      micsum2=micsum1;
    }
    delay(100);
  }
  mic_sum=mic_sum+(micsum2/20);
  analogRead(MIC);
  delay(100);
  micsum2=0;
}*/


void readCOZIR(){
  int m = 1;

  delay(100);  
    COZIR.begin(9600);//apro la connessione COZIR
    COZIR.flush();  
  delay(100);
  
  char inChar[16]; //costruzione di record da scrivere ogni 5 min
  //estrai txt da stringa cozir 
  
  uint8_t i =0;
  time = millis();
  while(inChar[i-1] != 0x0A){ 
     if(COZIR.available()){   
       inChar[i] = COZIR.read(); 
       i++; 
     } 
  
     if(millis()-time>10000) //ERRORE
       Reset();
  }
  
  //se ho letto correttamente
  if(i>7){
    for (int l=7;l>2;l--)
    {
      cozir_sum=cozir_sum+(inChar[l]-48)*m;
      m=m*10;
    } 
  }
  
    COZIR.flush();
    COZIR.end();
    //CHECK
  
    if(cozir_sum!=0){ 
      check_vect[COZ] = 1;
    }
    else{ 
      check_vect[COZ] = 0;
    }

}

