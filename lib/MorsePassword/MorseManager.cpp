#include "Arduino.h"
#include "MorseManager.h"
#include "AbstractMorsePassword.h"

MorseManager::MorseManager() : _storedPassword(), _inputPassword(_storedPassword.getPasswordLength()) {
#ifdef DEBUG
	Serial.println(F("Function: MorseManager()"));
#endif
}

void MorseManager::resetPassword() {
#ifdef DEBUG
	Serial.println(F("Function: void resetPassword()"));
#endif

  _storedPassword.resetPassword();

#ifdef DEBUG
	Serial.println(F("End void resetPassword()"));
#endif
}

void MorseManager::awaitSignal() {
#ifdef DEBUG
  Serial.println(F("Function: void awaitSignal()"));
#endif

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
    _inputPassword.translateInput(_storedPassword.getDitCount());
    if (_checkInput()) {
      // blink green
      digitalWrite(red, false);
      digitalWrite(green, true);

      _grantEntry();

      digitalWrite(green, false);
    } else {
      // blink red
      digitalWrite(green, false);
      digitalWrite(red, true);

      _denyEntry();

      digitalWrite(red, false);
    }
    break;

    case InputMorsePassword::preCheckFailed:
      _denyEntry();
      break;

    case InputMorsePassword::noInput:
    // do nothing
    break;

    default:
    // should not happen, do nothing
    break;
  }
}

bool MorseManager::_checkInput() {
#ifdef DEBUG
	Serial.println(F("Function: bool checkInput()"));
#endif

  for (unsigned int i = 0; i < _storedPassword.getPasswordLength(); i++) {

#ifdef DEBUG
    		Serial.println(String("password[") + i + "] = " + _storedPassword.getValueAt(i) + ", input[" + i + "] = " + _inputPassword.getValueAt(i));
#endif

    if (_inputPassword.getValueAt(i) != _storedPassword.getValueAt(i)) {
      return false;
    }
  }
  return true;
}

void MorseManager::_grantEntry() {
#ifdef DEBUG
	Serial.println(F("Function: void grantEntry()"));
#endif

	// wait humanize stealth time
	delay(minHStealthTime + random(rndHStealthTime));
	// open door
	digitalWrite(oPin, true);
	delay(openTime);
	digitalWrite(oPin, false);

	// reset
	reset();
}

void MorseManager::_denyEntry() {
#ifdef DEBUG
	Serial.println(F("Function: void denyEntry()"));
#endif

	delay(denyTime);

	// reset
	reset();
}

bool MorseManager::_checkSignal() {
#ifdef DEBUG
	// This makes the output somehow unreadable
//	Serial.println("Function: bool checkSignal()");
#endif

	return analogRead(iPin) >= analogTreshold;
}

void MorseManager::reset() {
#ifdef DEBUG
	Serial.println(F("Function: void reset()"));
#endif

  _inputPassword.reset();

	// blink orange
	digitalWrite(red, true);
	digitalWrite(green, true);
	delay(resetTime);

	// to begin, signal button has to be in released state
	if (_checkSignal()) {
#ifdef DEBUG
		Serial.println(F("iPin is pressed (has to be released to go on)"));
#endif
		// blink red
		digitalWrite(green, false);
		// do nothing until button is released
		while(_checkSignal()) {
			// do nothing
		}
		// blink orange again
		digitalWrite(green, true);
	}

	// end blink orange
	digitalWrite(red, false);
	digitalWrite(green, false);

#ifdef DEBUG
	Serial.println(F("Resetting now"));
#endif
}
