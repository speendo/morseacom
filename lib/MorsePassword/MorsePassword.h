#ifndef MorsePassword_h
#define MorsePassword_h

#include "Arduino.h"

class MorsePassword {
  public:
    MorsePassword(unsigned int maxPasswordLength, unsigned int resetPasswordTimeout);
    void resetPassword();
    String getPassword();
    unsigned int getPasswordLength();
    String getNewPW();
    bool checkNewPW(String newPW);
    bool storePW(String newPW);
  private:
    enum MorseSignal {
      empty,
      dit,
      dah
    };
    unsigned int maxPasswordLength;
    unsigned int resetPasswordTimeout;
};

#endif
