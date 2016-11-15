#ifndef MorseManager_h
#define MorseManager_h

#include "Arduino.h"
#include "AbstractMorsePassword.h"
#include "StoredMorsePassword.h"
#include "InputMorsePassword.h"

#include "BasicSettings.h"
#include "AdvancedSettings.h"

class MorseManager {
  public:
    MorseManager();
    void resetPassword();
    void awaitSignal();
  private:
    enum LoopState {
      noOperation,
      waitForSignal,
      waitForSignalEnd
    };
    StoredMorsePassword _storedPassword;
    InputMorsePassword _inputPassword;
    bool _checkSignal();
    void _receiveInput();
};

#endif
