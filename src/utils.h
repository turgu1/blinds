#ifndef _UTILS_H
#define _UTILS_H

#ifndef DEBUGGING
  #define DEBUGGING 0
#endif

#if DEBUGGING
  #define PRINT(msg)   Serial.print(msg)
  #define PRINTLN(msg) Serial.println(msg)
#else
  #define PRINT(msg)
  #define PRINTLN(msg)
#endif

#if DEBUGGING || SERIAL_NEEDED
  #define SERIAL_SETUP Serial.begin(74880)
#else
  #define SERIAL_SETUP
#endif

/*
 * ESP8266 pins need wired are below:
 * DIN (data in) on Matrix ---> 13 or MOSI on ESP8266
 * Clock(CLK) on Matrix --> 14 or SCK on ESP8266
 * CS pin on Matrix define below  --->( pick 15 on esp8266)
 */
    
const uint8_t TRANSMITTER_PIN = 5;

void init_controller();
int  decode_cmd(char * msg);
void emit_cmd();

#undef PUBLIC
#endif