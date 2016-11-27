// Reset password timeout
#define resetPasswordTimeout 2000

// define max password length (max number of dits or dahs in a password sequence)
// ATTENTION: the size of this value must be reserved in the EEPROM as well as in
//            the RAM. Don't use a pointless long value. You wouldn't ring your
//            doorbell 25+ times anyway!
#define maxPasswordLength 25

// Setup serial communication (9600 should be right - leave it if you are unsure)
#define serialBPS 9600

// Debug on or off
#define debug true
