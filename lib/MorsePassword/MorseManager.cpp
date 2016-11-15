#include "Arduino.h"
#include "MorseManager.h"
#include "AbstractMorsePassword.h"

MorseManager::MorseManager() {
  _storedPassword = StoredMorsePassword();
  _inputPassword = InputMorsePassword(_storedPassword.getPasswordLength());
}

void MorseManager::resetPassword() {
  _storedPassword.resetPassword();
}

void MorseManager::awaitSignal() {
  _inputPassword.processInput(_checkSignal());
  switch (_inputPassword.getInputStatus()) {

    case InputMorsePassword::shortCandidate:
    if (_checkSignal()) {
      // blink orange
      digitalWrite(red, true);
      digitalWrite(green, true);
    } else {
      // don't blink
      digitalWrite(red, false);
      digitalWrite(green, false);
    }

    case InputMorsePassword::candidate:
    if (_checkInput()) {
      grantEntry();
    } else {
      denyEntry();
    }
    break;

    case InputMorsePassword::preCheckFailed:
      denyEntry();
      break;

    case InputMorsePassword::noInput:
    // do nothing
    break;

    default:
    // should not happen, do nothing
    break;
  }
}

bool MorseManager::_checkSignal() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println("Function: bool checkSignal()");
#endif

	return analogRead(iPin) >= analogTreshold;
}

void MorseManager::_receiveInput() {
#ifdef DEBUG
	Serial.println("Function: void receiveInput()");
#endif

	getInput(input, maxPasswordLength, red, green, maxSigTime, minSigTime, waitTime);

	if (checkInput(input)) {
		grantEntry();
	} else {
		denyEntry();
	}
}

void MorseManager::getInput(MorseSignal *input, unsigned int maxPasswordLength, unsigned int red, unsigned int green, unsigned int maxSigTime, unsigned int minSigTime, unsigned int waitTime) {
#ifdef DEBUG
	Serial.println("Function: void MorseSignal* getInput()");
#endif

	unsigned long lastTimeStamp = millis();

	unsigned int inputCount = 0;

	unsigned int inputSignals[maxPasswordLength] = {};

	LoopState loopState = waitForSignalEnd;

#ifdef DEBUG
	Serial.println("loopState = waitForSignalEnd");
#endif

	while(true) {
		switch (loopState) {

		case waitForSignalEnd:

			// blink orange
			digitalWrite(red, true);
			digitalWrite(green, true);

			// if signal gets longer than waiting time
			if (millis() - lastTimeStamp > maxSigTime) {

#ifdef DEBUG
				Serial.println("Signal was too long (switching to Standby mode)");
#endif
				// return an empty array (which is later detected as invalid password
				return;
			}

			// if signal ends
			else if (checkSignal()) {
				// if the input gets longer than the password, the password is clearly wrong => invalid password
				if (inputCount >= maxPasswordLength) {

#ifdef DEBUG
					Serial.println("Provided key is longer than password (switching to Standby mode)");
					Serial.print("(currentInputValue: ");
					Serial.print(inputCount);
					Serial.print(", ArrayLength: ");
					Serial.print((getPasswordLength()));
					Serial.println(")");
#endif


					// return an empty array (which is later detected as invalid password
					return;
				}

				// else write signal to array (if it's longer than minSigTime)
				unsigned int sigTime = (millis() - lastTimeStamp);
#ifdef DEBUG
				Serial.print("Signal length:");
				Serial.println(sigTime);
#endif

				if (sigTime >= minSigTime) {
					inputSignals[inputCount] = sigTime;
#ifdef DEBUG
					Serial.print("inputSignal[");
					Serial.print(inputCount);
					Serial.print("] = ");
					Serial.println(sigTime);
#endif
					inputCount++;
				}

				// no matter how short the signal was, reset the following variables
				loopState = waitForSignal;

#ifdef DEBUG
				Serial.println("loopstate = waitForSignal");
#endif

				lastTimeStamp = millis();
				// stop blinking orange
				digitalWrite(red, false);
				digitalWrite(green, false);
			}
			break;

			// while Morse-A-Tron is not receiving a signal (but is waiting for one)

		case waitForSignal:

			if (millis() - lastTimeStamp > waitTime) {

#ifdef DEBUG
				Serial.println("Waiting time expired, now returning array");
#endif

				// check if there are enough signals
				if (inputCount == getPasswordLength()) {
					// translate to MorseSignal
					translateInput(inputSignals, input);
					return;
				} else {
					// return an empty array (which is later detected as invalid password
					return;
				}
			}

			// else, if a new signal comes in: switch to waitForSignalEnd
			else if (checkSignal()) {
				lastTimeStamp = millis();
				loopState = waitForSignalEnd;

#ifdef DEBUG
				Serial.println("loopstate = waitForSignalEnd");
#endif

			}
			break;

			// should not happen ...
		default:
			// return an empty array (which is later detected as invalid password
			return;
		}
	}
}

bool InputMorsePassword::checkInput(MorseSignal *input) {
#ifdef DEBUG
	Serial.println("Function: bool checkInput(MorseSignal *input)");
#endif
	for (unsigned i = 0; i < getPasswordLength(); i++) {
#ifdef DEBUG
		Serial.print("password[");
		Serial.print(i);
		Serial.print("] = ");
		Serial.print(password[i]);
		Serial.print(", ");
		Serial.print("input[");
		Serial.print(i);
		Serial.print("] = ");
		Serial.println(input[i]);
#endif
		if (password[i] != input[i]) {
			return false;
		}
	}
	return true;
}
