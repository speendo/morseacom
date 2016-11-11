#include "Arduino.h"
#include "MorsePassword.h"

#include "EEPROM.h"

MorsePassword::MorsePassword(unsigned int maxPasswordLength, unsigned int resetPasswordTimeout) {
  MorsePassword::maxPasswordLength = maxPasswordLength;
  MorsePassword::resetPasswordTimeout = resetPasswordTimeout;
}

void MorsePassword::resetPassword() {
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
				String newPW = getNewPW();
				if (checkNewPW(newPW)) {
					pwStored = storePW(newPW);
				}
			}
		}
	}
}

String MorsePassword::getNewPW() {
	Serial.println("Please enter new password (\".\": dit, \"-\": dah):");
	if (Serial.available() > 0) {
		return Serial.readString();
	}
}

bool MorsePassword::checkNewPW(String newPW) {
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

bool MorsePassword::storePW(String newPW) {
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

String MorsePassword::getPassword() {
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

unsigned int MorsePassword::getPasswordLength() {
	for (unsigned int i = 0; i < maxPasswordLength; i++) {
		if (EEPROM.read(i) == empty || EEPROM.read(i) == 255) {
			return i;
		}
	}
	return maxPasswordLength;
}
