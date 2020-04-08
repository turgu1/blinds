#include <Arduino.h>

#include "utils.h"

#define CMDS_COUNT 6

enum CMD { DOWN = 0, UP, STOP, PAIR, LIMIT, CHGDIR };

static const char *cmds[CMDS_COUNT] = { 
  "DOWN", "UP", "STOP", "PAIR", "LIMIT", "CHGDIR" 
};

static const char cmd_bits[CMDS_COUNT] = {
  1, 12, 5, 4, 2, 8
};

static const int PULSE_SHORT = 340;
static const int PULSE_LONG  = 680;

static const int AGC1_PULSE  = 4885;
static const int AGC2_PULSE  = 2450;
static const int AGC3_PULSE  = 1700;

static const int RADIO_SILENCE = 5057;
static const int MODEL         = 0xC1;

static CMD cmd;
static int id;
static int channel_count;
static short channels[10];

void init_controller()
{
  pinMode(TRANSMITTER_PIN, OUTPUT);
  digitalWrite(TRANSMITTER_PIN, LOW);

  delay(10);
}

void transmitHigh(int delay_microseconds) 
{
  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(delay_microseconds);
}

void transmitLow(int delay_microseconds) {
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(delay_microseconds);
}

void transmitValue(int value, int count)
{
  int bit;
  while (count > 0) {
    bit = value & 1;
    value >>= 1;
    if (bit == 0) {
      transmitLow(PULSE_SHORT);
      transmitHigh(PULSE_SHORT);
      transmitLow(PULSE_SHORT);
    }
    else {
      transmitLow(PULSE_SHORT);
      transmitHigh(PULSE_LONG);
    } 
    count--;
  }
}

int decode_cmd(char * msg)
{
  char *ch;
  char cmd_str[10];
  int  i;

  // Retrieve command

  ch = msg;
  i = 0;
  while ((i < 9) && (*ch != 0) && (*ch != ',')) {
    if (*ch != ' ') cmd_str[i++] = *ch;
    ch++;
  }
  cmd_str[i] = 0;
  if (*ch != ',') {
    PRINT("ERROR: Command incomplete: "); PRINTLN(msg);
    return 0;
  }

  i = 0;
  while (i < CMDS_COUNT) {
    if (strcmp(cmds[i], cmd_str) == 0) break;
    i++;
  }

  if (i >= CMDS_COUNT) {
    PRINT("ERROR: Unknown command: "); PRINTLN(cmd_str);
    return 0;
  }
  cmd = (CMD)i;

  // Retrieve id

  ch++;
  while (*ch == ' ') ch++;
  id = 0;
  while ((*ch >= '0') && (*ch <= '9')) {
    id = (id * 10) + (*ch - '0');
    ch++;
  }
  while (*ch == ' ') ch++;
  if (*ch != ',') {
    PRINT("ERROR: No channel present: "); PRINTLN(msg);
    return 0;
  }

  // Retrieve channels

  channel_count = 0;
  while ((*ch == ',') && (channel_count < 10)) {
    ch++;
    while (*ch == ' ') ch++;
    channels[channel_count] = 0;
    while ((*ch >= '0') && (*ch <= '9')) {
      channels[channel_count] = (channels[channel_count] * 10) + (*ch - '0');
      ch++;
    }
    while (*ch == ' ') ch++;
    channel_count++;
  }

  if (channel_count == 0) {
    PRINT("ERROR: No channel present: "); PRINTLN(msg);
  }

  while (*ch == ' ') ch++;

  if (*ch != 0) {
    PRINT("ERROR: Some residual data not processed: "); PRINTLN(msg);
    return 0;
  }

  #if DEBUGGING
    PRINTLN("Parsing command successfull:");
    PRINT("Cmd: "); PRINTLN(cmd_str);
    PRINT("Id: "); PRINTLN(id);
    PRINT("Channels:");

    char c = ' ';
    for (i = 0; i < channel_count; i++) {
      PRINT(c); PRINT(' '); 
      PRINT(channels[i]); 
      c = ',';
    }
    PRINTLN(".");
  #endif

  return 1;
}

void emit_cmd()
{
  unsigned char checksum;

  pinMode(TRANSMITTER_PIN, OUTPUT); // Prepare the digital pin for output

  for (int i = 0; i < channel_count; i++) {

    for (int j = 0; j < 10; j++) {

      transmitHigh(AGC1_PULSE);
      transmitLow(AGC2_PULSE);
      transmitHigh(AGC3_PULSE);

      checksum = ((id >> 8) & 0xFF) + 
                 (id & 0xFF) + 
                 ((cmd << 4) + channels[i]) +
                 MODEL - 1;
      checksum = (checksum ^ 0xFF) + 1;

      transmitValue(id,          16);
      transmitValue(channels[i],  4);
      transmitValue(cmd,          4);
      transmitValue(MODEL,        8);
      transmitValue(checksum,     8);
      transmitValue(1,            1);
    }

    // Radio silence at the end of last command.
    // It's better to go a bit over than under minimum required length:
    transmitLow(RADIO_SILENCE);
  }

  digitalWrite(TRANSMITTER_PIN, LOW);
}
