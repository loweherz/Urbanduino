#include <Fat16.h>
#include <Fat16util.h>

SdCard card;
Fat16 file;

void storeOnSD(char* filename){ 
 DateTime now = RTC.now();

 // initialize the SD card
    if (!card.init()){ 
      #ifdef SERIAL_DEBUG
        showString(PSTR("card.init"));
      #endif      
    }
  
    // initialize a FAT16 volume
    if (!Fat16::init(&card)){ 
      #ifdef SERIAL_DEBUG 
        showString(PSTR("Fat16::init"));
      #endif      
    }
  
    // clear write error
    file.writeError = false;
  
    // O_CREAT - create the file if it does not exist
    // O_APPEND - seek to the end of the file prior to each write
    // O_WRITE - open for write
    if (!file.open(filename, O_CREAT | O_APPEND | O_WRITE)){
        #ifdef SERIAL_DEBUG
          showString(PSTR("error opening file"));
        #endif      
    }
  
    file.print("{\"timestamp\":");
    file.print(String(now.day(), DEC));
    file.print("\\");
    file.print(String(now.month(), DEC));
    file.print("\\");
    file.print(String(now.year(), DEC));
    file.print("-");
    file.print(String(now.hour(), DEC));
    file.print(":");
    file.print(String(now.minute(), DEC));
    file.print(",\"device\":T9");
    file.print(",\"cozir\":");
    file.print(cozir_sum);
    file.print(",\"temp\":");
    file.print(temp_sum);
    file.print(",\"hum\":");
    file.print(hum_sum);
    file.print(",\"mic\":");
    file.print(mic_sum);
    file.print(",\"gps\":");
    file.print(gprmc);
    file.print("}\n");
    
    if (!file.close()){ 
       #ifdef SERIAL_DEBUG
         showString(PSTR("error closing file"));
       #endif
    }
 
}
