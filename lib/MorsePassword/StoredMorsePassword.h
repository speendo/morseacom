#ifndef StoredMorsePassword_h
#define StoredMorsePassword_h

#include "Arduino.h"
#include "AbstractMorsePassword.h"

#include "BasicSettings.h"
#include "AdvancedSettings.h"

class StoredMorsePassword {
  public:
    StoredMorsePassword();
    void resetPassword();
    String getPassword();
    bool checkNewPW(String newPW);
    unsigned int getPasswordLength();
    unsigned int getDitCount();
    MorseSignal getValueAt(unsigned int i);
  private:
    String _getNewPW();
    bool _storePW(String newPW);
};

#endif
