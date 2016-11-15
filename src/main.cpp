// Do not remove the include below
#include "Morse-A-Com-noVal.h"
#include "MorseManager.h"

#include "BasicSettings.h"
#include "AdvancedSettings.h"

#define DEBUG

/*
 * Automatic Mores-A-Com
 * by Marcel Jira
 *
 * This work may be distributed and/or modified under
 * the conditions of the GNU Public License (GPL)
 */

MorseManager morseManager;

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

	morseManager = MorseManager();
	morseManager.resetPassword();

	reset();
}

void loop() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println("Function: void loop()");
#endif

	morseManager.awaitSignal();
}

void translateInput(unsigned int *inputSig, MorseSignal *input) {
#ifdef DEBUG
	Serial.println("Function: void translateInput(unsigned int *inputSig, MorseSignal *input)");
#endif
	unsigned int inputCopy[(password.getPasswordLength())];
	memcpy(inputCopy, inputSig, sizeof(inputCopy));

#ifdef DEBUG
	Serial.println("Unordered keyLengths are:");
	for (unsigned int i = 0; i < (getPasswordLength()); i++) {
		Serial.print("input: ");
		Serial.println(inputSig[i]);
	}
#endif

	quicksort(&inputCopy[0], &inputCopy[(sizeof(inputCopy) / sizeof(unsigned int))]);

	// generate a border between dits and dahs in user input
	unsigned int avInputLength;

#ifdef DEBUG
	Serial.println("Ordered keyLengths are:");
	for (unsigned int i = 0; i < (getPasswordLength()); i++) {
		Serial.print("inputCopy: ");
		Serial.println(inputCopy[i]);
	}
#endif

	unsigned int ditCount = 0;
	for (unsigned int i = 0; i < (getPasswordLength()); i++) {
		if (password[i] == dit) {
			ditCount++;
		}
	}

	if (ditCount <= 0) {
		avInputLength = 0;
	}
	else if (ditCount >= (getPasswordLength())) {
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
