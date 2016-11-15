#include "Arduino.h"
#include "InputMorsePassword.h"
#include "AbstractMorsePassword.h"

InputMorsePassword::InputMorsePassword(unsigned int _expectedPasswordLength) {
	_expectedPasswordLength = _expectedPasswordLength;
	reset();
}

void InputMorsePassword::processInput(bool signal) {
	unsigned long newTimeStamp = millis();
	if (_loopState == waitForSignal) {
		if (signal) {
			beginSignal(newTimeStamp);
		} else {
			// check if waiting time expired
			if (newTimeStamp - _lastTimeStamp > waitTime) {

#ifdef DEBUG
				Serial.println("Waiting time expired");
#endif

				// check if there are enough signals
				if (_position == _expectedPasswordLength) {
					_inputStatus = candidate;
				} else {
					_inputStatus = preCheckFailed;
				}
			}
		}
	} else if (_loopState == waitForSignalEnd) {
		if (signal) {
			// if signal gets longer than waiting time
			if (newTimeStamp - _lastTimeStamp > maxSigTime) {

#ifdef DEBUG
				Serial.println("Signal was too long");
#endif

				_inputStatus = preCheckFailed;
			}
		// else if signal ends
		} else {
			// if the input gets longer than the password, the password is clearly wrong => invalid password
			if (_position >= _expectedPasswordLength) {

#ifdef DEBUG
				Serial.println("Provided key is longer than password (switching to Standby mode)");
				Serial.print("(current length: " + inputCount + ", expected length: " + _expectedPasswordLength + ")");
#endif

				_inputStatus = preCheckFailed;
			}

			// everything seems alright
			return endSignal(newTimeStamp);
		}
	} else {
		// _loopState = noOperation
		// should not happen...
		_inputStatus = noInput;
	}
}

void InputMorsePassword::beginSignal(unsigned long newTimeStamp) {
	_lastTimeStamp = newTimeStamp;
	_loopState = waitForSignalEnd;
	_inputStatus = shortCandidate;
}

void InputMorsePassword::endSignal(unsigned long newTimeStamp) {
	unsigned int sigTime = newTimeStamp - _lastTimeStamp;

#ifdef DEBUG
	Serial.print("Signal length:"	+ sigTime);
#endif

	// check if sigTime is at least minSigTime
	if (sigTime >= minSigTime) {

#ifdef DEBUG
	Serial.print("inputSignal[" + inputCount + "] = " + sigTime);
#endif

		_durations[_position] =
		_lastTimeStamp = newTimeStamp;
		_position++;
		_loopState = waitForSignal;
		_inputStatus = shortCandidate;

#ifdef DEBUG
		Serial.println("loopstate = waitForSignal");
#endif

	}
}

InputMorsePassword::InputStatus InputMorsePassword::getInputStatus() {
	return _inputStatus;
}

void InputMorsePassword::reset() {
	_loopState = noOperation;
	_lastTimeStamp = 0; // this is maybe not necessary
	_position = 0;
	_inputStatus = noInput;
	for (int i = 0; i < maxPasswordLength; i++) {
		_durations[i] = 0;
		_enteredPassword[i] = empty;
	}
}
