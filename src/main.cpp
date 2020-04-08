// BLIND Control
//
// This is an example of a controller to send 433Mhz messages 
// to control blinds in your home.em Image" task of the PlatformIO IDE.
//
// Guy Turcotte
// 2020/04/08

#include <Arduino.h>

#include <Maison.h>
#include <string.h>

#include "utils.h"

#define BLIND_TOPIC "blinds"

// ---- Globals ----

bool start;
char msg[200];
int  msg_len;

// ---- Maison Framework ----

Maison maison(Maison::WATCHDOG_24H);

void blind_callback(const char * topic, byte * payload, unsigned int length) 
{
  PRINTLN("Callback...");

  if (length > 200) length = 199;

  // The rest of the payload is put in the msg buffer.
  // msq_len != 0 will trigger the display on the matrix cells

  memcpy(msg, payload, length);
  msg[length] = 0;
  msg_len = length;

  PRINT(F("Received the following message: ["));
  PRINT(msg);
  PRINTLN("]");

  if (decode_cmd(msg)) emit_cmd();
}

void setup() 
{
  delay(100);

  SERIAL_SETUP;

  PRINTLN("Setup...");

  maison.setup();
  maison.set_msg_callback(blind_callback, BLIND_TOPIC, 0);
}

void loop() 
{
  delay(10);
  
  maison.loop();
}

