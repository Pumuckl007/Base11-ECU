#ifndef SETTINGS_H
#define SETTINGS_H
#include "Stream.h"
#include "Arduino.h"

namespace RPL{
  namespace Settings{
    const int VALVE_MAP[] = {13, 23, 25, 27, 29, 31, 33, 35};
    const size_t VALVE_MAP_LEN = sizeof(VALVE_MAP)/sizeof(VALVE_MAP[0]);
    HardwareSerial * const FCB_STREAM = &Serial;
    HardwareSerial * const COMMAND_BOX_STREAM = &Serial1;
    const long SERIAL_RATE = 115200;

    const int PT_PIN_MAP[] = {0, 7, 2, 5, 4, 3, 6, 1};
    const size_t PT_PIN_MAP_LEN = sizeof(PT_PIN_MAP)/sizeof(PT_PIN_MAP[0]);
    const int PT_UPDATE_RATE = 1000;
    const int PT_OVERSAMPLE_MULTIPLIER = 10;

    const int TC_PIN_MAP[] = {48, 46, 44, 47, 36, 38, 40, 42};
    const size_t TC_PIN_MAP_LEN = sizeof(TC_PIN_MAP)/sizeof(TC_PIN_MAP[0]);
    const int TC_UPDATE_RATE = 2000;
    const int TC_OVERSAMPLE_MULTIPLIER = 5;
    const int TC_DO_PIN = 50;
    const int TC_CLK_PIN = 52;
  }
}

#endif
