#ifndef InputMorsePassword_h
#define InputMorsePassword_h

#include "Arduino.h"
#include "AbstractMorsePassword.h"

#include "BasicSettings.h"
#include "AdvancedSettings.h"

class InputMorsePassword {
  public:
    enum InputStatus {
      noInput,
      shortCandidate,
      candidate,
      preCheckFailed
    };

    InputMorsePassword(unsigned int expectedPasswordLength);
    void processInput(bool signal);
    InputStatus getInputStatus();
    MorseSignal getValueAt(unsigned int i);
    void translateInput(unsigned int ditCount);
    void reset();
  private:
    enum LoopState {
      noOperation,
      waitForSignal,
      waitForSignalEnd
    };
    void beginSignal(unsigned long newTimeStamp);
    void endSignal(unsigned long newTimeStamp);

    LoopState _loopState;
    unsigned long _lastTimeStamp;
    unsigned int _position;
    void _quicksort(unsigned int *begin, unsigned int *end);
    void _swap(unsigned int *a, unsigned int *b);

    unsigned int _expectedPasswordLength;

    InputStatus _inputStatus;

    unsigned int _durations[maxPasswordLength];
    MorseSignal _enteredPassword[maxPasswordLength];
};

#endif
