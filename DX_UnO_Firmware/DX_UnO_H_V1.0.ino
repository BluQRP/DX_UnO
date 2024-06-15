//*********************************************************************************************************
//**********  DX UnO  - QRPp ARDUINO DIGITAL MODES MULTIBAND HF TRANSCEIVER WITH CAT CONTROL  *************
//********************************* Write up start: 06/01/2024 ********************************************
// FW VERSION: DX_UnO_H_V1.0 - ( FW for DX UnO High Bands QRPp Portable HF Digital Modes Transceiver)
// Barb(Barbaros ASUROGLU) - WB2CBA - 2024
// HIGH BANDS DX UnO operates on 20m/17m/15m/12m/10m
//*********************************************************************************************************
// Required Libraries
// ----------------------------------------------------------------------------------------------------------------------
// Etherkit Si5351 (Needs to be installed via Library Manager to arduino ide) - SI5351 Library by Jason Mildrum (NT7S) - https://github.com/etherkit/Si5351Arduino
//*****************************************************************************************************
//* IMPORTANT NOTE: Use V2.1.3 of NT7S SI5351 Library. This is the only version compatible with DX UnO!!!*
//*****************************************************************************************************
// Arduino "Wire.h" I2C library(built-into arduino ide)
// Arduino "EEPROM.h" EEPROM Library(built-into arduino ide)
//*************************************[ LICENCE and CREDITS ]*********************************************
//  Initial FSK TX Signal Generation code by: Burkhard Kainka(DK7JD) - http://elektronik-labor.de/HF/SDRtxFSK2.html
//  SI5351 Library by Jason Mildrum (NT7S) - https://github.com/etherkit/Si5351Arduino
//  DX CAT code inspired from Lajos Höss, HA8HL TS2000 CAT implementation for Arduino.
//  Improved FSK TX  signal generation code is from JE1RAV https://github.com/je1rav/QP-7C
//
// License
// -------
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


//********************************[ CAT CONTROL SETTINGS and CAT Functionality ]********************
// CAT CONTROL RIG EMULATION: KENWOOD TS2000
// SERIAL PORT SETTINGS: 9600 baud,8 bit,1 stop bit
// When CAT is active Blue CAT led will be solid lit.
// DX can be controlled ONLY by CAT interface. Frequency and TX can be controlled via CAT.
// Once activated CAT mode stays active as rig control Until power recycle. 

//*******************************[ LIBRARIES ]*************************************************
#include <si5351.h>
#include "Wire.h"
#include <EEPROM.h>
//*******************************[ VARIABLE DECLERATIONS ]*************************************
uint32_t val;
int addr;
unsigned int Band = 0;
//unsigned long freq; 
unsigned long int freq1;
unsigned long int freq = 14074000;
unsigned long int freq4;
unsigned long int freqdiv;
int32_t cal_factor = 0;

int TX_State = 0;
int TX_inh;
int TXSW_State;
int index = 0;

uint32_t ReferenceFrequency = 26000000; //TCXO Frequency used as a reference for the Si5351PLL

//------------ CAT Variables
boolean cat_stat = 0;
boolean TxStatus = 0; //0 =  RX 1=TX

int cat_active;
int CAT_mode = 2;   
String received;
String receivedPart1;
String receivedPart2;    
String command;
String command2;  
String parameter;
String parameter2; 
String sent;
String sent2;



// **********************************[ DEFINE's ]***********************************************

#define TX 12 //TX LED
#define STAT 11 //CAT LED
#define RX  8 // RX SWITCH

Si5351 si5351;

//*************************************[ SETUP FUNCTION ]************************************** 
void setup()
{
  // delay(2000);
      
  Wire.begin(); // wake up I2C bus 
       
pinMode(TX, OUTPUT);

     pinMode(STAT, OUTPUT);

        pinMode(RX, OUTPUT);

pinMode(7, INPUT); //PD7 = AN1 = HiZ, PD6 = AN0 = 0

//FLASH BLUE CAT LED FOR POWER ON INDICATION
 
   for (int i = 0; i < 3; i++) {
    digitalWrite(STAT, HIGH);  
    delay(200);
    digitalWrite(STAT, LOW);
    delay(200);
  }
   
//SET UP SERIAL FOR CAT CONTROL

Serial.begin(9600); 
Serial.setTimeout(4); 
   
                              
//------------------------------- SET SI5351 VFO -----------------------------------  
   // The crystal load value needs to match in order to have an accurate calibration
 si5351.init(SI5351_CRYSTAL_LOAD_8PF, ReferenceFrequency, 0); //initialize the VFO
  si5351.set_correction(cal_factor, SI5351_PLL_INPUT_XO);
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);// SET For Max Power
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA); // Set for reduced power for RX 
  
 
  TCCR1A = 0x00;
  TCCR1B = 0x01; // Timer1 Timer 16 MHz
  TCCR1B = 0x81; // Timer1 Input Capture Noise Canceller
  ACSR |= (1<<ACIC);  // Analog Comparator Capture Input
  
  pinMode(7, INPUT); //PD7 = AN1 = HiZ, PD6 = AN0 = 0
          digitalWrite(RX,LOW);   
          
}
 
//**************************[ END OF SETUP FUNCTION ]************************

//***************************[ Main LOOP Function ]**************************
void loop()
{  

// CAT CHECK SERIAL FOR NEW DATA FROM WSJT-X AND RESPOND TO QUERIES

   if ((Serial.available() > 0)) {
            
             digitalWrite(STAT, HIGH); 
                  
       CAT_control();    
    }

 //--------------------------- LIMIT OUT OF OPERATION BANDS TO ONLY RX -----------------
freqdiv = freq / 1000000;

if (freqdiv < 11 || freqdiv > 30){

         goto RX1; // Skip TX Loop
         
}  

// --------------------------- FSK  TX LOOP -----------------------------------
 // The following code is from JE1RAV https://github.com/je1rav/QP-7C
  //(Using 3 cycles for timer sampling to improve the precision of frequency measurements)
  //(Against overflow in low frequency measurements)
  
  int FSK = 1;
  int FSKtx = 0;

  while (FSK>0){
    int Nsignal = 10;
    int Ncycle01 = 0;
    int Ncycle12 = 0;
    int Ncycle23 = 0;
    int Ncycle34 = 0;
    unsigned int d1=1,d2=2,d3=3,d4=4;
  
    TCNT1 = 0;  
    while (ACSR &(1<<ACO)){
      if (TIFR1&(1<<TOV1)) {
        Nsignal--;
        TIFR1 = _BV(TOV1);
        if (Nsignal <= 0) {break;}
      }
    }
    while ((ACSR &(1<<ACO))==0){
      if (TIFR1&(1<<TOV1)) {
        Nsignal--;
        TIFR1 = _BV(TOV1);
        if (Nsignal <= 0) {break;}
      }
    }
    if (Nsignal <= 0) {break;}
    TCNT1 = 0;
    while (ACSR &(1<<ACO)){
        if (TIFR1&(1<<TOV1)) {
        Ncycle01++;
        TIFR1 = _BV(TOV1);
        if (Ncycle01 >= 2) {break;}
      }
    }
    d1 = ICR1;  
    while ((ACSR &(1<<ACO))==0){
      if (TIFR1&(1<<TOV1)) {
        Ncycle12++;
        TIFR1 = _BV(TOV1);
        if (Ncycle12 >= 3) {break;}      
      }
    } 
    while (ACSR &(1<<ACO)){
      if (TIFR1&(1<<TOV1)) {
        Ncycle12++;
        TIFR1 = _BV(TOV1);
        if (Ncycle12 >= 6) {break;}
      }
    }
    d2 = ICR1;
    while ((ACSR &(1<<ACO))==0){
      if (TIFR1&(1<<TOV1)) {
        Ncycle23++;
        TIFR1 = _BV(TOV1);
        if (Ncycle23 >= 3) {break;}
      }
    } 
    while (ACSR &(1<<ACO)){
      if (TIFR1&(1<<TOV1)) {
      Ncycle23++;
      TIFR1 = _BV(TOV1);
      if (Ncycle23 >= 6) {break;}
      }
    } 
    d3 = ICR1;
    while ((ACSR &(1<<ACO))==0){
      if (TIFR1&(1<<TOV1)) {
        Ncycle34++;
        TIFR1 = _BV(TOV1);
        if (Ncycle34 >= 3) {break;}
      }
    } 
    while (ACSR &(1<<ACO)){
      if (TIFR1&(1<<TOV1)) {
        Ncycle34++;
        TIFR1 = _BV(TOV1);
        if (Ncycle34 >= 6) {break;}
      }
    } 
    d4 = ICR1;
    unsigned long codefreq1 = 1600000000/(65536*Ncycle12+d2-d1);
    unsigned long codefreq2 = 1600000000/(65536*Ncycle23+d3-d2);
    unsigned long codefreq3 = 1600000000/(65536*Ncycle34+d4-d3);
    unsigned long codefreq = (codefreq1 + codefreq2 + codefreq3)/3;
    if (d3==d4) codefreq = 5000;     
    if ((codefreq < 310000) and  (codefreq >= 10000)) {
       
        
      if (FSKtx == 0){
              
        TX_State = 1;
        digitalWrite(RX,LOW);
        digitalWrite(TX,HIGH);
        delay(10);
        si5351.output_enable(SI5351_CLK1, 0);   //RX off
        si5351.output_enable(SI5351_CLK0, 1);   // TX on
      }
    
      si5351.set_freq((freq * 100 + codefreq), SI5351_CLK0);   
      if(Serial.available() > 0) CAT_control(); 
        
      FSKtx = 1;
    }
    else{
      FSK--;
    }
  }

 RX1:
  digitalWrite(TX,0);
       si5351.output_enable(SI5351_CLK0, 0);   //TX off

       freq4 = freq * 4;
freqdiv = freq / 1000000;

if (freqdiv <= 25){
  //       Serial.print("   Freq4: ");
//  Serial.println(freq4); 
           si5351.set_freq(freq4*100ULL, SI5351_CLK1);
   si5351.output_enable(SI5351_CLK1, 1);   //RX on
}

if (freqdiv > 25 && freqdiv < 30){
            // Set CLK1 to output 112 MHz
  si5351.set_ms_source(SI5351_CLK1, SI5351_PLLB);
  si5351.set_freq_manual(freq4*100ULL, 70000000000ULL, SI5351_CLK1);
                si5351.output_enable(SI5351_CLK1, 1);   //RX on
}
    
    TX_State = 0;
    digitalWrite(RX,HIGH);
   
  FSKtx = 0;
    
}
//*********************[ END OF MAIN LOOP FUNCTION ]*************************

//********************************************************************************
//******************************** [ FUNCTIONS ] *********************************
//********************************************************************************


//*****************************[ CAT CONTROL FUNCTION ]***************************

void CAT_control(void)
{
                           
    received = Serial.readString();  
    received.toUpperCase();  
    received.replace("\n","");  

           String data = "";
           int bufferIndex = 0;

          for (int i = 0; i < received.length(); ++i)
               {
               char c = received[i];
    
                 if (c != ';')
                    {
                    data += c;
                    }
                 else
                    {
                        if (bufferIndex == 0)
                          {  
                              data += '\0';
                              receivedPart1 = data;
                              bufferIndex++;
                              data = "";
                          }
                         else
                          {  
                              data += '\0';
                              receivedPart2 = data;
                              bufferIndex++;
                              data = "";
                          }
                    }

               }
    
    command = receivedPart1.substring(0,2);
    command2 = receivedPart2.substring(0,2);    
    parameter = receivedPart1.substring(2,receivedPart1.length());
    parameter2 = receivedPart2.substring(2,receivedPart2.length());

    if (command == "FA")  
    {  
        
          if (parameter != "")  
              {  
              freq = parameter.toInt();
              //VfoRx = VfoTx;   
              }
          
          sent = "FA" // Return 11 digit frequency in Hz.  
          + String("00000000000").substring(0,11-(String(freq).length()))   
          + String(freq) + ";";     
    }

    else if (command == "PS")  
        {  
        sent = "PS1;";
        }

    else if (command == "TX")  
        {   
        sent = "TX0;";
        //VfoTx = freq;
        digitalWrite(TX,1);
        TxStatus = 1;
       
        }

    else if (command == "RX")  
        {  
        sent = "RX0;";
        //VfoRx = freq;
        digitalWrite(TX,0);
        TxStatus = 0;
        
        }

    else  if (command == "ID")  
        {  
        sent = "ID019;";
        }

    else if (command == "AI")  
        {
        sent = "AI0;"; 
        }

    else if (command == "IF")  
        {
          if (TxStatus == 1)
            {  
            sent = "IF" // Return 11 digit frequency in Hz.  
            + String("00000000000").substring(0,11-(String(freq).length()))   
            //+ String(freq) + String("     ") + "+" + "0000" + "0" + "0" + "0" + "00" + "0" + String(CAT_mode) + "0" + "0" + "0" + "0" + "00" + String(" ") + ";"; 
            + String(freq) + "00000" + "+" + "0000" + "0" + "0" + "0" + "00" + "1" + String(CAT_mode) + "0" + "0" + "0" + "0" + "000" + ";"; 
            } 
             else
            {  
            sent = "IF" // Return 11 digit frequency in Hz.  
            + String("00000000000").substring(0,11-(String(freq).length()))   
            //+ String(freq) + String("     ") + "+" + "0000" + "0" + "0" + "0" + "00" + "0" + String(CAT_mode) + "0" + "0" + "0" + "0" + "00" + String(" ") + ";"; 
            + String(freq) + "00000" + "+" + "0000" + "0" + "0" + "0" + "00" + "0" + String(CAT_mode) + "0" + "0" + "0" + "0" + "000" + ";"; 
            } 
       }
  
    else if (command == "MD")  
      {  
      sent = "MD2;";
      }

//------------------------------------------------------------------------------      

    if (command2 == "ID")  
        {  
        sent2 = "ID019;";
        }
            
    if (bufferIndex == 2)
        {
        Serial.print(sent2);
        }
        
    else
    {
        Serial.print(sent);
    }  

   if ((command == "RX") or (command = "TX")) delay(50);

    sent = String("");
    sent2 = String("");  

    
}
