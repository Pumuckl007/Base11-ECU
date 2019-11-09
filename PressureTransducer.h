#ifndef PRESSURE_TRANSDUCER_H
#define PRESSURE_TRANSDUCER_H

#include <Arduino.h>

namespace RPL{
  class PressureTransducer {
  public:
    PressureTransducer(int pin = 0, int id = 0);
    int writeValueToBuffer(char buffer[]);
    void aquire();
    void resetAccumulator();

  private:
    int pin;
    int id;
    unsigned int sum;
    int accumulatorCount;
  };
}

#endif
