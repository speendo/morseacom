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
