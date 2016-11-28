#include "Arduino.h"
#include "StoredMorsePassword.h"

#include "EEPROM.h"

StoredMorsePassword::StoredMorsePassword() {
// breaks serial connection in the beginning
//#ifdef DEBUG
//	Serial.println(F("Function: StoredMorsePassword()"));
//#endif
}

void StoredMorsePassword::resetPassword() {
#ifdef DEBUG
	Serial.println(F("Function: void resetPassword()"));
#endif

	if (Serial) {
		if (getPasswordLength() > 0) {
			Serial.println("Current password: " + getPassword());
		} else {
			Serial.println("No password set.");
		}
		Serial.println(String("Press \"Y <Enter>\" within ") + resetPasswordTimeout + " ms to change the password.");
		unsigned long lastTimeStamp = millis();

		bool changePW = false;
		// Timeout, because if(Serial) is not reliable on most boards
		while (millis() - lastTimeStamp <= resetPasswordTimeout) {
			if (Serial.available()) {
				char inChar = (char)Serial.read();
				if (inChar == '\n' || inChar == 'y' || inChar == 'Y') {
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
#ifdef DEBUG
	Serial.println(F("Function: String _getNewPW()"));
#endif

	String newPassword = "";

	// flush the serial buffer
	while (Serial.available() > 0) {
		Serial.read();
	}

	// now receive the password
	Serial.println("Please enter new password (\".\": dit, \"-\": dah) and press <Enter> (\"N <Enter\" for no password):");
	while (true) {
		if (Serial.available()) {
			char inChar = (char)Serial.read();

			Serial.print((String)inChar);

			if (inChar == '\r' || inChar == '\n' || inChar == 'n' || inChar == 'N') {
				Serial.println();
				return newPassword;
			} else {
				newPassword += inChar;
			}
		}
	}
}

bool StoredMorsePassword::checkNewPW(String newPW) {
#ifdef DEBUG
	Serial.println(F("Function: bool checkNewPW(String newPW)"));

	Serial.println("Entered Password is: <" + newPW + ">");
#endif

  // check length
  if (newPW.length() > maxPasswordLength) {
		Serial.println("Password too long!");

    return false;
  }
  // check characters
  for (unsigned int i = 0; i < newPW.length(); i++) {
    if (newPW.charAt(i) != '.' && newPW.charAt(i) != '-') {
			Serial.println(String("Password contains invalid character: \'") + newPW.charAt(i) + "\'!");

      return false;
    }
  }
	return true;
}

bool StoredMorsePassword::_storePW(String newPW) {
#ifdef DEBUG
	Serial.println(F("Function: bool _storePW(String newPW)"));
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
	Serial.println(F("Function: String getPassword()"));
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
// breaks serial connection in the beginning
//#ifdef DEBUG
//	Serial.println(F("Function: unsigned int getPasswordLength()"));
//#endif

	for (unsigned int i = 0; i < EEPROM.length(); i++) {
		if (getValueAt(i) == empty || getValueAt(i) == 255) {
			return i;
		}
	}
}

unsigned int StoredMorsePassword::getDitCount() {
#ifdef DEBUG
	Serial.println(F("Function: unsigned int getDitCount()"));
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
// breaks serial connection in the beginning
//#ifdef DEBUG
//	Serial.println(F("Function: MorseSignal getValueAt(unsigned int i)"));
//#endif

	return MorseSignal(EEPROM.read(i));
}
