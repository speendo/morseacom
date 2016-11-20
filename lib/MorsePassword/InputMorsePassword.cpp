#include "Arduino.h"
#include "InputMorsePassword.h"
#include "AbstractMorsePassword.h"

InputMorsePassword::InputMorsePassword(unsigned int expectedPasswordLength) {
#ifdef DEBUG
	Serial.println("Function: InputMorsePassword(unsigned int expectedPasswordLength)");
#endif

	_expectedPasswordLength = expectedPasswordLength;
	reset();
}

void InputMorsePassword::processInput(bool signal) {
#ifdef DEBUG
	Serial.println("Function: void processInput(bool signal)");
#endif

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
#ifdef DEBUG
	Serial.println("Function: void beginSignal(unsigned long newTimeStamp)");
#endif

	_lastTimeStamp = newTimeStamp;
	_loopState = waitForSignalEnd;
	_inputStatus = shortCandidate;
}

void InputMorsePassword::endSignal(unsigned long newTimeStamp) {
#ifdef DEBUG
	Serial.println("Function: endSignal(unsigned long newTimeStamp)");
#endif

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
#ifdef DEBUG
	Serial.println("Function: InputStatus getInputStatus()");
#endif

	return _inputStatus;
}

void InputMorsePassword::translateInput(unsigned int ditCount) {
#ifdef DEBUG
	Serial.println("Function: void translateInput()");
#endif

	unsigned int durationsCopy[_expectedPasswordLength];
	memcpy(durationsCopy, _durations, sizeof(durationsCopy));

#ifdef DEBUG
	Serial.println("Unordered keyLengths are:");
	for (unsigned int i = 0; i < (_expectedPasswordLength); i++) {
		Serial.print("input: " + _durations[i]);
	}
#endif

	_quicksort(&durationsCopy[0], &durationsCopy[(sizeof(durationsCopy) / sizeof(unsigned int))]);

	// generate a border between dits and dahs in user input
	unsigned int avInputLength;

#ifdef DEBUG
	Serial.println("Ordered keyLengths are:");
	for (unsigned int i = 0; i < (_expectedPasswordLength); i++) {
		Serial.print("durationsCopy: ");
		Serial.println(durationsCopy[i]);
	}
#endif

	if (ditCount <= 0) {
		avInputLength = 0;
	}
	else if (ditCount >= _expectedPasswordLength) {
		avInputLength = maxSigTime;
	}
	else {
		// in fact a password should always consist of dits AND dahs, therefore the 2 possibilities above should never be true

		// get average dit length
		unsigned int avDit = 0;
		for (unsigned int i = 0; i < ditCount; i++) {
			avDit = avDit + durationsCopy[i];
		}
		avDit = avDit / ditCount;

		// get average dah length
		unsigned int avDah = 0;
		for (unsigned int i = ditCount; i < (sizeof(durationsCopy) / sizeof(unsigned int)); i++) {
			avDah = avDah + durationsCopy[i];
		}
		avDah = avDah / ((sizeof(durationsCopy) / sizeof(unsigned int)) - ditCount);

		// calculate border between dit and dah in user input
		avInputLength = (avDit + avDah) / 2;

#ifdef DEBUG
		Serial.print("avDit: " + avDit);
		Serial.print("avDah: " + avDah);
#endif
	}

#ifdef DEBUG
	Serial.print("avInputLength: " + avInputLength);
#endif

#ifdef DEBUG
	Serial.print("input: ");
#endif
	// now generate dits and dahs out of user input
	for (unsigned int i = 0; i < (sizeof(durationsCopy) / sizeof(unsigned int)); i++) {
		if (_durations[i] < (avInputLength - (ditDahTreshold * avInputLength))) {
			_enteredPassword[i] = dit;
#ifdef DEBUG
			Serial.print(".");
#endif
		}
		else if (_durations[i] > (avInputLength + (ditDahTreshold * avInputLength))) {
			_enteredPassword[i] = dah;
#ifdef DEBUG
			Serial.print("-");
#endif
		}
		else {
			// else there is a signal that is in the interval
			// (avInputLength - (ditDahTreshold * avInputLength)) <= signal <= (avInputLength + (ditDahTreshold * avInputLength))
			// write an empty value in inputMorse (later this will be recognized as invalid input
			_enteredPassword[i] = empty;
#ifdef DEBUG
			Serial.print("?");
#endif
		}
	}
#ifdef DEBUG
	Serial.println();
#endif
}

MorseSignal InputMorsePassword::getValueAt(unsigned int i) {
// remember to always run translateInput() first - this cannot be checked here.

#ifdef DEBUG
	Serial.println("Function: MorseSignal getValueAt(unsigned int i)");
#endif

	return _enteredPassword[i];
}

void InputMorsePassword::reset() {
#ifdef DEBUG
	Serial.println("Function: void reset()");
#endif

	_loopState = noOperation;
	_lastTimeStamp = 0; // this is maybe not necessary
	_position = 0;
	_inputStatus = noInput;
	for (int i = 0; i < maxPasswordLength; i++) {
		_durations[i] = 0;
		_enteredPassword[i] = empty;
	}
}

// Quicksort
void InputMorsePassword::_quicksort(unsigned int *begin, unsigned int *end) {
#ifdef DEBUG
	// Makes the output somehow unreadable
//	Serial.println("Function: void quicksort(unsigned int *begin, unsigned int *end)");
#endif
	unsigned int *ptr;
	unsigned int *split;
	if (end - begin <= 1)
		return;
	ptr = begin;
	split = begin + 1;
	while (++ptr != end) {
		if (*ptr < *begin) {
			_swap(ptr, split);
			++split;
		}
	}
	_swap(begin, split - 1);
	_quicksort(begin, split - 1);
	_quicksort(split, end);
}

void InputMorsePassword::_swap(unsigned int *a, unsigned int *b) {
#ifdef DEBUG
	// Makes the output somehow unreadable
//	Serial.println("Function: void swap(unsigned int *a, unsigned int *b)");
#endif
	unsigned int tmp = *a;
	*a = *b;
	*b = tmp;
}
