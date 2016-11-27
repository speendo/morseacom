#include "Arduino.h"
#include "EEPROM.h"

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

	Serial.println("Setup complete. Now waiting for morse signals.");
}

void loop() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println(F("Function: void loop()"));
#endif

	morseManager.awaitSignal();
}
