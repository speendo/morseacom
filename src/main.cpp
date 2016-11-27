// Do not remove the include below
#include "Morse-A-Com-noVal.h"
#include "MorseManager.h"

#include "BasicSettings.h"
#include "AdvancedSettings.h"

/*
 * Automatic Mores-A-Com
 * by Marcel Jira
 *
 * This work may be distributed and/or modified under
 * the conditions of the GNU Public License (GPL)
 */

MorseManager morseManager;

// this has to come after the definition of morseManager
#if (debug == true)
#define DEBUG
#endif

void setup() {
	Serial.begin(serialBPS);

#ifdef DEBUG
	Serial.println(F("DEBUG: on"));
	Serial.println(F("Function: void setup()"));
#endif


	// basic setup
	pinMode(iPin, INPUT);
	pinMode(oPin, OUTPUT);
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);

	morseManager = MorseManager();
	morseManager.resetPassword();

	morseManager.reset();
}

void loop() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println(F("Function: void loop()"));
#endif

	morseManager.awaitSignal();
}
