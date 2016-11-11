#ifndef MorsePassword_h
#define MorsePassword_h

#include "Arduino.h"

class MorsePassword {
  public:
    MorsePassword(int pin);
  private:
    enum MorseSignal {
      empty,
      dit,
      dah
    };
};

#endif
