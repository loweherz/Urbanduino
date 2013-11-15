#define COUNT 5

byte countdown = COUNT;

void GPS_data(){   
    nss.begin(9600);
    
    time = millis();
    
    while(1)
    {    
      readline(time);      
      
      if( (strncmp(gprmc, "$GPRMC",6) == 0) ){
        parseptr = gprmc+7;
        parseptr = strchr(parseptr, ',') + 1;
        status = parseptr[0];
        parseptr += 2;
        // grab latitude & long data
        // latitude
        latitude = parsedecimal(parseptr);
        if (latitude != 0) {
          latitude *= 10000;
          parseptr = strchr(parseptr, '.')+1;
          latitude += parsedecimal(parseptr);
        }
        parseptr = strchr(parseptr, ',') + 1;
        // read latitude N/S data
        if (parseptr[0] != ',') {
          latdir = parseptr[0];
        }
        //Serial.println(latdir);
        // longitude
        parseptr = strchr(parseptr, ',')+1;
        longitude = parsedecimal(parseptr);
        if (longitude != 0) {
          longitude *= 10000;
          parseptr = strchr(parseptr, '.')+1;
          longitude += parsedecimal(parseptr);
        }
        parseptr = strchr(parseptr, ',')+1;
        // read longitude E/W data
        if (parseptr[0] != ',') {
          longdir = parseptr[0];
        }
        // groundspeed
        parseptr = strchr(parseptr, ',')+1;
        groundspeed = parsedecimal(parseptr);
        // track angle
        parseptr = strchr(parseptr, ',')+1;
        trackangle = parsedecimal(parseptr);
        //Serial.println(gprmc);
        break;
      }
    }
    
    nss.flush();
    nss.end();   

    countdown = COUNT;    
}

uint32_t parsedecimal(char *str) {
  uint32_t d = 0;
  while (str[0] != 0) {
    if ((str[0] > '9') || (str[0] < '0'))
      return d;
    d *= 10;
    d += str[0] - '0';
    str++;
  }
  return d;
}

void readline(unsigned long time) {
  char c;
  gprmcdx = 0; // start at begninning
   
  while (1) {
    
    //Serial.println(time);
    if( (millis()-time) > 5000){
      countdown--;
      if(countdown==0){
        showString(PSTR("..RESET..\n"));
        Reset();
      }
      showString(PSTR("..\n"));
      nss.flush();
      nss.end();
      gprmc[0] = 0;
      GPS_data();
      return;
    }
    
    delay(10);
    c=nss.read();
    
    if (c == -1)
      continue;
  
    if (c == '\n')
      continue;

    if ((gprmcdx == GPSSIZ-1) || (c == '\r')) {
      gprmc[gprmcdx] = 0;      
         
      return;
    }

    gprmc[gprmcdx++]= c;
  }
}
