#include "Arduino.h"
#include "EEPROM.h"

enum MorseSignal {
  empty,
  dit,
  dah
};

enum LoopState {
  noOperation,
  waitForSignal,
  waitForSignalEnd
};

void setup();
void loop();
void resetPassword();
String getPassword();
unsigned int getPasswordLength();
String getNewPW();
bool checkNewPW(String newPW);
bool storePW(String newPW);
bool checkSignal();
void receiveInput();
void translateInput(unsigned int *inputSig, MorseSignal *input);
bool checkInput(MorseSignal *input);
void getInput(MorseSignal *input);
void grantEntry();
void denyEntry();
void reset();
void quicksort(unsigned int *begin, unsigned int *end);
void swap(unsigned int *a, unsigned int *b);
