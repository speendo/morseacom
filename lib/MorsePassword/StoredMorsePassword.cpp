#include "Arduino.h"
#include "StoredMorsePassword.h"

#include "EEPROM.h"

StoredMorsePassword::StoredMorsePassword() {
}

void StoredMorsePassword::resetPassword() {
	if (Serial) {
		if (getPasswordLength() > 0) {
			Serial.println("Current password: " + getPassword());
		} else {
			Serial.println("No password set.");
		}
		Serial.println("Change password? (Y/N)");
		unsigned long lastTimeStamp = millis();

		bool changePW = false;
		// Timeout, because if(Serial) is not reliable on most boards
		while (millis() - lastTimeStamp <= resetPasswordTimeout) {
			if (Serial.available() > 0) {
				char answer = char(Serial.read());
				if (answer == 'N' || answer == 'n') {
					break;
				} else if (answer == 'Y' || answer == 'y') {
					changePW = true;
					break;
				}
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
	Serial.println("Please enter new password (\".\": dit, \"-\": dah):");
	if (Serial.available() > 0) {
		return Serial.readString();
	}
}

bool StoredMorsePassword::checkNewPW(String newPW) {
  // check length
  if (newPW.length() > maxPasswordLength) {
    return false;
  }
  // check characters
  for (unsigned int i = 0; i < newPW.length(); i++) {
    if (newPW.charAt(i) != '.' && newPW.charAt(i) != '-') {
      return false;
    }
  }
	return true;
}

bool StoredMorsePassword::_storePW(String newPW) {
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
	for (unsigned int i = 0; i < EEPROM.length(); i++) {
		if (getValueAt(i) == empty || getValueAt(i) == 255) {
			return i;
		}
	}
}

unsigned int StoredMorsePassword::getDitCount() {
	unsigned int ditCount = 0;
	for (unsigned int i = 0; i < getPasswordLength(); i++) {
		if (getValueAt(i) == dit) {
			ditCount++;
		}
	}
	return ditCount;
}

MorseSignal StoredMorsePassword::getValueAt(unsigned int i) {
	return MorseSignal(EEPROM.read(i));
}
