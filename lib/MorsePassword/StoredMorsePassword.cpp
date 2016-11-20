#include "Arduino.h"
#include "StoredMorsePassword.h"

#include "EEPROM.h"

StoredMorsePassword::StoredMorsePassword() {
#ifdef DEBUG
	Serial.println("Function: StoredMorsePassword()");
#endif
}

void StoredMorsePassword::resetPassword() {
#ifdef DEBUG
	Serial.println("Function: void resetPassword()");
#endif

	if (Serial) {
		if (getPasswordLength() > 0) {
			Serial.println("Current password: " + getPassword());
		} else {
			Serial.println("No password set.");
		}
		Serial.println(String("Press <Enter> within ") + resetPasswordTimeout + " ms to change the password.");
		unsigned long lastTimeStamp = millis();

		bool changePW = false;
		// Timeout, because if(Serial) is not reliable on most boards
		while (millis() - lastTimeStamp <= resetPasswordTimeout) {
			if (Serial.available() > 0) {
				// it is assumed that the input contains an <Enter>
				changePW = true;
				break;
			}
		}
		if (changePW == true) {
			bool pwStored = false;
			while (!pwStored) {
				String newPW = _getNewPW();
				pwStored = _storePW(newPW);
			}
		}
	}
}

String StoredMorsePassword::_getNewPW() {
#ifdef DEBUG
	Serial.println("Function: String _getNewPW()");
#endif

	Serial.println("Please enter new password (\".\": dit, \"-\": dah):");
	if (Serial.available() > 0) {
		return Serial.readString();
	}
}

bool StoredMorsePassword::checkNewPW(String newPW) {
#ifdef DEBUG
	Serial.println("Function: bool checkNewPW(String newPW)");
#endif

  // check length
  if (newPW.length() > maxPasswordLength) {
		Serial.println("Password too long!");

    return false;
  }
  // check characters
  for (unsigned int i = 0; i < newPW.length(); i++) {
    if (newPW.charAt(i) != '.' && newPW.charAt(i) != '-') {
			Serial.println("Password contains invalid character!");

      return false;
    }
  }
	return true;
}

bool StoredMorsePassword::_storePW(String newPW) {
#ifdef DEBUG
	Serial.println("Function: bool _storePW(String newPW)");
#endif

	if (checkNewPW(newPW)) {
    for (unsigned int i = 0; i < newPW.length(); i++) {
      if (newPW.charAt(i) == '.') {
        EEPROM.update(i, dit);
      } else {
        EEPROM.update(i, dah);
      }
    }
    // finally make sure the position after the password is "empty"
    EEPROM.update(newPW.length(), empty);
	  return true;
  } else {
    return false;
  }
}

String StoredMorsePassword::getPassword() {
#ifdef DEBUG
	Serial.println("Function: String getPassword()");
#endif

	String retPW = "";
	for (unsigned int i = 0; i < (getPasswordLength()); i++) {
		if (EEPROM.read(i) == dit) {
			retPW += ".";
		} else {
			retPW += "-";
		}
	}
	return retPW;
}

unsigned int StoredMorsePassword::getPasswordLength() {
#ifdef DEBUG
	Serial.println("Function: unsigned int getPasswordLength()");
#endif

	for (unsigned int i = 0; i < EEPROM.length(); i++) {
		if (getValueAt(i) == empty || getValueAt(i) == 255) {
			return i;
		}
	}
}

unsigned int StoredMorsePassword::getDitCount() {
#ifdef DEBUG
	Serial.println("Function: unsigned int getDitCount()");
#endif

	unsigned int ditCount = 0;
	for (unsigned int i = 0; i < getPasswordLength(); i++) {
		if (getValueAt(i) == dit) {
			ditCount++;
		}
	}
	return ditCount;
}

MorseSignal StoredMorsePassword::getValueAt(unsigned int i) {
#ifdef DEBUG
	Serial.println("Function: MorseSignal getValueAt(unsigned int i)");
#endif

	return MorseSignal(EEPROM.read(i));
}
