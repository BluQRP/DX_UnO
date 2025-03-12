/***********************************************************************************
* Use serial console Output for debugging and development
*  
* Change if needed
************************************************************************************/
// Comment out if you do not want Serial console output
// Comment out DEVMODE in a production environment as it will degrade performance!
//#define DEVMODE

//--------------------------------------------------------------------
// Your own HAM call. Change it
#define MYCALL "XXXXXX" // REPLACE XXXXX with your CALL SIGN
//--------------------------------------------------------------------
// The power of your transmission in dBm. 
// for the DX UnO this should be set to 24 (=400 milliwatts).
#define DBMPOWER 24


// If you have a valid GPS lock, the software will make this HIGH twice for 20ms
// You could connect a LED to it.
// Set LED_PIN to 0 if you do not want to use a pin
#define LED_PIN 11  

// If you have an Arduino on 16MHz, set this to 10672
// If you have an Arduino on 8MHz, set this to 5336
#define WSPR_CTC             10672         

// Frequency of the SI5351 Oscillator in Hertz
// for example #define SI5351FREQ 26000000 if you have a 26MHz XO
// Use 0 if you have a 25MHz Oscillator
#define SI5351FREQ 26000000


/***********************************************************************************
* All Si5351 chips have an accuracy error
* Use the example calibration program in the etherkit si5351 library to get that correction
* Or use a frequency counter or reliable VFO to check frequency accuracy.
*  
* Correction is in hundreths of Herz.
* It won't work if you don't calibrate your si5351
************************************************************************************/
// Change this value!
#define SI5351_CORRECTION 0

/***********************************************************************************
* wspr SETTINGS
*  
* Change if needed
************************************************************************************/
// WSPR will transmit only on the even minutes for two minutes. So there are 30 possible slots in one hour
// Fill in your favourite frequencies below. Use 0 is you do not want to transmit in that slot.
//
// If you leave this unchanged, you will get WSPR transmissions on 10, 20 and 40 meter band
// every 12 minutes. 
static const unsigned long long freqArray[6] = {
  14097100ULL, // minute   0,12,24,36,48 => 10 meter band WSPR transmission
  18106100ULL,        // minute   2,14,26,38,50 => no transmission 
  21096100ULL, // minute   4,16,28,40,52 => 20 meter band WSPR transmission 
  24926100ULL,        // minute   6,18,30,42,54 => no transmission
  28126100ULL,  // minute   8,20,32,44,56 => 40 meter band WSPR transmission 
  0ULL         // minute  10,22,34,46,58 => no transmission 
};


/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// I recommend using the ATMGM336H GPS, which is well available, cheap and has proven to be reliable
// We use SoftwareSerial so these pin numbers are basically free to choose
static const int Rx = 10, Tx = 9; // Arduino pins to connect to Tx and Rx of the GPS
static const uint32_t GPSBaud = 9600;  // GPS BAUD rate. Modern GPS usually 9600, older ones might use 4800.
                         
