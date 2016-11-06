// Do not remove the include below
#include "Morse-A-Com-noVal.h"

#define DEBUG

/*
 * Automatic Mores-A-Com
 * by Marcel Jira
 *
 * This work may be distributed and/or modified under
 * the conditions of the GNU Public License (GPL)
 */

// Password (as sequence: use "dit" for short inputs and "dah" for long inputs, like "{dit, dit, dit, dah, dah, dah, dit, dit, dit}" for "SOS")
MorseSignal password[] = {dit, dah};

// define input pin
#define iPin 2
// define output pin
#define oPin 3
// define red led pin
#define red 6
// define green led pin
#define green 7

// define doorbell treshold (analog value that is recognzed as signal)
#define analogTreshold 500

// define waiting time (how long will the Morse-A-Com wait for a new signal, after the previous signal is finished) (in ms)
// ATTENTION: this is also the (min) time, the Morse-A-Com will wait after the last signal before it opens the door
#define waitTime 1500

// define min signal length (if the signal is shorter than this it will be ignored - this is useful
// because of mechanical/electrical insufficiencies)
#define minSigTime 10

// define max signal length (max time the button should be pressed at once)
#define maxSigTime 3000

// define opening time (how long should the door stay open)
#define openTime 1000

// define deny time (how long should the Morse-A-Com blink red and wait before reset
#define denyTime 1000

// define reset time (how long should the Morse-A-Com be unresponsive after each input-attempt)
#define resetTime 1000

// define ditDahTreshold (the Morde-A-Com will calculate a border between dits and dahs depending on the user duration of the user inputs signals
// if a signal is shorter than this border, it is recognized as dit, if a signal is longer than this border, it is recognized as dah
// if a signal has the same duration as the border, the provided user input will be rejected
// with ditDahTreshold it is possible to broaden this border; for example if ditDahTreshold = 0.1 user input will be rejected iff
// <border> - (0.1 * <border>) <= signal <= <border> + (0.1 * <border>)
// ATTENTION: ditDahTreshold should be 0 <= ditDahTreshold < 1
#define ditDahTreshold 0.1

// define min humanize stealth time (how long should the Morse-A-Com wait on average after a correct code has been entered - this is useful
// if somebody watches you entering the door - nobody would believe a real person could open the door that fast after you
// entered the code; if this value is high enough people would probably think a real person would open the door for you
// ATTENTION: the Morse-A-Com will anyway wait waitTime after the last input, therefore overall waiting time is
//            waitTime + minHStelathTime (+ rndHStealthTime))
// ATTENTION: if you don't need humanize stealth time, simply set it to zero
#define minHStealthTime 5000

// define random humanize stealth time (to make human stealth time settings even more realistic, you can state another time here
// the resulting humanize stealth time will be: minHStealthTime + rand(0,rndHStealthTime) -- overall waiting time is
// waitTime + minHStelathTime (+ rndHStealthTime))
// ATTENTION: if you don't need a random humanize stealth time, simply set it to zero
#define rndHStealthTime 2500

// Setup serial communication (9600 should be right - leave it if you are unsure)
#define serialBPS 9600

void setup() {
#ifdef DEBUG
	Serial.begin(serialBPS);
	Serial.println("DEBUG: on");
	Serial.println("Function: void setup()");
#endif


	// basic setup
	pinMode(iPin, INPUT);
	pinMode(oPin, OUTPUT);
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);

#ifdef DEBUG
	Serial.print("passwordSize: ");
	Serial.println((sizeof(password) / sizeof(MorseSignal)));
	Serial.print("password: ");
	for (unsigned int i = 0; i < (sizeof(password) / sizeof(MorseSignal)); i++) {
		if (password[i] == dit) {
			Serial.print(".");
		} else {
			Serial.print("-");
		}
	}
	Serial.println();
#endif
	reset();
}

void loop() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println("Function: void loop()");
#endif

	if (checkSignal()) {
		receiveInput();
	}
}

bool checkSignal() {
#ifdef DEBUG
	Serial.println("Function: bool checkSignal()");
#endif

	return analogRead(iPin) >= analogTreshold;
}

void receiveInput() {
#ifdef DEBUG
	Serial.println("Function: void receiveInput()");
#endif

	MorseSignal input[sizeof(password) / sizeof(MorseSignal)] = {};
	getInput(input);

	if (checkInput(input)) {
		grantEntry();
	} else {
		denyEntry();
	}
}

void translateInput(unsigned int *inputSig, MorseSignal *input) {
#ifdef DEBUG
	Serial.println("Function: void translateInput(unsigned int *inputSig, MorseSignal *input)");
#endif
	unsigned int inputCopy[(sizeof(password) / sizeof(MorseSignal))];
	memcpy(inputCopy, inputSig, sizeof(inputCopy));

#ifdef DEBUG
	Serial.println("Unordered keyLengths are:");
	for (unsigned int i = 0; i < (sizeof(password) / sizeof(MorseSignal)); i++) {
		Serial.print("input: ");
		Serial.println(inputSig[i]);
	}
#endif

	quicksort(&inputCopy[0], &inputCopy[(sizeof(inputCopy) / sizeof(unsigned int))]);

	// generate a border between dits and dahs in user input
	unsigned int avInputLength;

#ifdef DEBUG
	Serial.println("Ordered keyLengths are:");
	for (unsigned int i = 0; i < (sizeof(password) / sizeof(MorseSignal)); i++) {
		Serial.print("inputCopy: ");
		Serial.println(inputCopy[i]);
	}
#endif

	unsigned int ditCount = 0;
	for (unsigned int i = 0; i < (sizeof(password) / sizeof(MorseSignal)); i++) {
		if (password[i] == dit) {
			ditCount++;
		}
	}

	if (ditCount <= 0) {
		avInputLength = 0;
	}
	else if (ditCount >= (sizeof(password) / sizeof(MorseSignal))) {
		avInputLength = maxSigTime;
	}
	else {
		// in fact a password should always consist of dits AND dahs, therefore the 2 possibilities above should never be true

		// get average dit length
		unsigned int avDit = 0;
		for (unsigned int i = 0; i < ditCount; i++) {
			avDit = avDit + inputCopy[i];
		}
		avDit = avDit / ditCount;

		// get average dah length
		unsigned int avDah = 0;
		for (unsigned int i = ditCount; i < (sizeof(inputCopy) / sizeof(unsigned int)); i++) {
			avDah = avDah + inputCopy[i];
		}
		avDah = avDah / ((sizeof(inputCopy) / sizeof(unsigned int)) - ditCount);

		// calculate border between dit and dah in user input
		avInputLength = (avDit + avDah) / 2;

#ifdef DEBUG
		Serial.print("avDit: ");
		Serial.println(avDit);
		Serial.print("avDah: ");
		Serial.println(avDah);
#endif
	}

#ifdef DEBUG
	Serial.print("avInputLength: ");
	Serial.println(avInputLength);
#endif

#ifdef DEBUG
	Serial.print("input: ");
#endif
	// now generate dits and dahs out of user input
	for (unsigned int i = 0; i < (sizeof(inputCopy) / sizeof(unsigned int)); i++) {
		if (inputSig[i] < (avInputLength - (ditDahTreshold * avInputLength))) {
			input[i] = dit;
#ifdef DEBUG
			Serial.print(".");
#endif
		}
		else if (inputSig[i] > (avInputLength + (ditDahTreshold * avInputLength))) {
			input[i] = dah;
#ifdef DEBUG
			Serial.print("-");
#endif
		}
		else {
			// else there is a signal that is in the interval
			// (avInputLength - (ditDahTreshold * avInputLength)) <= signal <= (avInputLength + (ditDahTreshold * avInputLength))
			// write an empty value in inputMorse (later this will be recognized as invalid input
			input[i] = empty;
#ifdef DEBUG
			Serial.print("?");
#endif
		}
	}
#ifdef DEBUG
	Serial.println();
#endif
}

bool checkInput(MorseSignal *input) {
#ifdef DEBUG
	Serial.println("Function: bool checkInput(MorseSignal *input)");
#endif
	for (unsigned i = 0; i < sizeof(password) / sizeof(MorseSignal); i++) {
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

void getInput(MorseSignal *input) {
#ifdef DEBUG
	Serial.println("Function: void MorseSignal* getInput()");
#endif

	unsigned long lastTimeStamp = millis();

	unsigned int inputCount = 0;

	unsigned int inputSignals[sizeof(password) / sizeof(MorseSignal)] = {};

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
				if (inputCount >= (sizeof(password) / sizeof(MorseSignal))) {

#ifdef DEBUG
					Serial.println("Provided key is longer than password (switching to Standby mode)");
					Serial.print("(currentInputValue: ");
					Serial.print(inputCount);
					Serial.print(", ArrayLength: ");
					Serial.print((sizeof(password) / sizeof(MorseSignal)));
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
				if (inputCount == sizeof(password) / sizeof(MorseSignal)) {
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

void grantEntry() {
#ifdef DEBUG
	Serial.println("Function: void grantEntry()");
#endif
	// blink green
	digitalWrite(red, false);
	digitalWrite(green, true);
	// wait humanize stealth time
	delay(minHStealthTime + random(rndHStealthTime));
	// open door
	digitalWrite(oPin, true);
	delay(openTime);
	digitalWrite(oPin, false);
	digitalWrite(green, false);

	// reset
	reset();
}

void denyEntry() {
#ifdef DEBUG
	Serial.println("Function: void denyEntry()");
#endif
	// blink red
	digitalWrite(green, false);
	digitalWrite(red, true);
	delay(denyTime);
	digitalWrite(red, false);

	// reset
	reset();
}

void reset() {
#ifdef DEBUG
	Serial.println("Function: void reset()");
#endif
	// blink orange
	digitalWrite(red, true);
	digitalWrite(green, true);
	delay(resetTime);

	// to begin, signal button has to be in released state
	if (checkSignal()) {
#ifdef DEBUG
		Serial.println("iPin is pressed (has to be released to go on)");
#endif
		// blink red
		digitalWrite(green, false);
		// do nothing until button is released
		while(checkSignal()) {
			// do nothing
		}
		// blink orange again
		digitalWrite(green, true);
	}

	// end blink orange
	digitalWrite(red, false);
	digitalWrite(green, false);

#ifdef DEBUG
	Serial.println("Resetting now");
#endif
}

// Quicksort
void quicksort(unsigned int *begin, unsigned int *end) {
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
			swap(ptr, split);
			++split;
		}
	}
	swap(begin, split - 1);
	quicksort(begin, split - 1);
	quicksort(split, end);
}

void swap(unsigned int *a, unsigned int *b) {
#ifdef DEBUG
	// Makes the output somehow unreadable
//	Serial.println("Function: void swap(unsigned int *a, unsigned int *b)");
#endif
	unsigned int tmp = *a;
	*a = *b;
	*b = tmp;
}
