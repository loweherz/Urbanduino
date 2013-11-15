// Seeeduino Urban  -  Stazioni su BUS e Bicicletta
// creato da Paolo Calvani - Terrasystem
// modificato da Officina Ibimet: Alessandro Matese, Jacopo Primicerio, Alessandro Zaldei, Alfonso Crisci, Filippo Di Gennaro 
// 15/02/2013: urban_4_plus_GPS: stazione B1: urban bike Ibimet

/*
  
  Last update: 28/10/2013 - 16:30
  Last updater: Mirko Mancin (UNIPR - WASNLAB)
  Version: urban_bike_ibimet_verB.0.3

*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include <String.h>


unsigned int time = millis();

/*************************
  DEFINE PER IMPOSTAZIONI
  
  * TOGLIERE COMMENTI 
    PER DEFINIRE LE VARIABILI    
*************************/
#define SERIAL_DEBUG 0
//#define INIT_TIME 0



/************
  SLEEP
*************/
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

volatile int f_timer=0;

#define DEFINE_SLEEP 200
int SECONDS_TO_SLEEP = DEFINE_SLEEP;

/**********
  RTC
***********/
#include <RTClib.h>
RTC_DS1307 RTC;



/**********
  GPS
**********/
SoftwareSerial nss(4,12);
#define GPSRATE 9600
#define GPSSIZ 90 // plenty big

char gprmc[GPSSIZ];
char status;
char gprmcdx;
char *parseptr;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;



/**********
  SENSORS
***********/
#include <dht.h>

#define CZ_RX_PIN 2
#define CZ_TX_PIN 3
#define MIC A3           // MIC collegato al pin analogico 3
#define DHT22_PIN A0      // DHT collegato al pin analogico 0
dht DHT; //dichiaro una variabile globale per la classe del sensore

SoftwareSerial COZIR(CZ_RX_PIN,CZ_TX_PIN); //inizializza seriale COZIR 



/**************
  GLOBAL
**************/
int cozir_sum = 0; //COZIR
float mic_sum=0;//media calcoata del rumore
float temp_sum=0;
float hum_sum=0;//media calcoata della temperatura ed umidit� anche in caso di errore



/**************
  CHECK
**************/
#define N_CHECK 3
char check_vect[N_CHECK] = {0,0,0};
#define TMP   0
#define HUM   1
#define COZ   2



//Dichiarazione di funzione che punta all'indirizzo zero
void(* Reset)(void) = 0;



void setup()
{  
  
  
  Wire.begin();
  //#ifdef SERIAL_DEBUG
    Serial.begin(19200);// inizializza seriale
  //#endif

  //turnOFFDevices();
  
  // RTC Setup
  RTC.begin();
  
  #ifdef INIT_TIME
    RTC.adjust(DateTime(__DATE__, __TIME__));  // definisci il realtime
  #endif
  
  
  
  // GSM Setup
  initGSM_Serial();
  delay(1000); 
    
    
    
  #ifdef SERIAL_DEBUG
    showString(PSTR("\n*********\nInitialising...\n\n"));
    delay(100); //Allow for serial print to complete.
  #endif  
  
  
  configureTimerSleep();         
} 



void loop(){
  
  if(f_timer==1)
  {    
    /* Don't forget to clear the flag. */
    f_timer = 0;
    
    
    //turnONDevices();
    //delay(30000);
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading DHT...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA DHT */
    readDHT();
    
    
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading MIC...\n"));
      delay(10); //Allow for serial print to complete.
    #endif  
    /* Sensors_reading - LETTURA MIC */  
    //readMIC();
  
  
  
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading COZIR...\n"));
      delay(10); //Allow for serial print to complete.
    #endif
    /* Sensors_reading - LETTURA COZIR*/  
    readCOZIR();
  
  
  
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading GPS...\n"));
      delay(10); //Allow for serial print to complete.
    #endif
    /* GPS */
    GPS_data();
  
  
    
    if(!checkData()){
      #ifdef SERIAL_DEBUG
        showString(PSTR("ERROR SENSING!\n"));
        showString(PSTR("\nCOZIR: "));
        Serial.println(cozir_sum);
        showString(PSTR("\nTEMP: "));
        Serial.println(temp_sum);
        showString(PSTR("\nHUM: "));
        Serial.println(hum_sum);
        showString(PSTR("\nMIC: "));
        Serial.println(mic_sum);
        showString(PSTR("\nGPS: "));
        Serial.println(gprmc);        
      #endif
      delay(500);
      Reset();  
    }
  
  
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nSensing complete!\n"));
      delay(100); //Allow for serial print to complete.
    #endif
  
    //turnOFFDevices();
    delay(500);
    
    // Accendo e imposto il modulo GSM per funzionare con l'APN corretto
    //GSM
    turnONGSM();
    initialSetup();   
  
  
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nInitialising complete!\n"));
      delay(100); //Allow for serial print to complete.
    #endif
    
    
    //INVIO AL SERVER - arg1 = Server, arg2 = File dove inviare i dati
    //sendGSMData("149.139.16.96:8080/sensorwebhub/j_set_data?dato_in=");
    sendGSMData("http://149.139.16.93/test/logger.php?msg=");
    
    //Dopo che ho inviato i dati spengo il modem, salvo su SD e mi metto in sleep
    //SPENGO DEVICES
    //turnOFFDevices();
    delay(500);
    //GSM - SPENGO
    turnOFFGSM();       
    
    //SD
    storeOnSD("data.txt");
    //SleepCommand       
    sleepForSeconds(SECONDS_TO_SLEEP);

    //WAKE UP DA SLEEP          
    delay(500);    
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nWAKE UP!!! "));
      showString(PSTR("\nMILLIS: "));
      Serial.println(millis());
      delay(100);
          
      showString(PSTR("\nMemory test results: "));
      Serial.print(memoryTest(),DEC);
      showString(PSTR(" bytes free\n"));
      
      delay(500);
    #endif
  
        
    // RESET VARIABILI 
    mic_sum=0;
    temp_sum=0;
    hum_sum=0;
    cozir_sum=0;
    
    delay(50);      
  }
}



// Controllo la bontà dei dati dai sensori
boolean checkData(){
  boolean response = true;
  
  for(int i=0; i<N_CHECK; i++){
    if(check_vect[i]==0) response = false;
  }
  
  return response; 
}



void sleepForSeconds(int time){
  /* IL TIMER HA UN PRESCALER DI 4 SECONDI*/
  for(int i=0; i<time/4; i++)
    enterSleep();
}


#ifdef SERIAL_DEBUG
    // this function will return the number of bytes currently free in RAM
    int memoryTest() {
      int byteCounter = 0; // initialize a counter
      byte *byteArray; // create a pointer to a byte array
      // More on pointers here: http://en.wikipedia.org/wiki/Pointer#C_pointers
    
      // use the malloc function to repeatedly attempt allocating a certain number of bytes to memory
      while ( (byteArray = (byte*) malloc (byteCounter * sizeof(byte))) != NULL ) {
        byteCounter++; // if allocation was successful, then up the count for the next try
        free(byteArray); // free memory after allocating it
      }
      
      free(byteArray); // also free memory after the function finishes
      return byteCounter; // send back the highest number of bytes successfully allocated
    }
#endif
