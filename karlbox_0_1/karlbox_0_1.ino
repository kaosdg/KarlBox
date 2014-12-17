// Mike Prevette
// v0.2
// 12/13/14
// Dependencies v_v_v_v_v_v_v_v_v_v_v_v_v_v_v_v
//
// Arduino 1.0.6 IDE http://arduino.cc/en/Main/Software#toc2
// -------I also recomend the great Stino plugin for sublime: http://robot-will.github.io/Stino/ 
// --------------- Needs some config but its worth it.
//
// -------The below go in the ~/documents/Arduino/Library folder
// LED LIBRARY https://github.com/adafruit/Adafruit_NeoPixel
//
// WAVE PLAYBACK  https://github.com/TMRh20/TMRpcm  
// --------------- Needs modification to pcmConfig.h 
// ---------------- Uncomment the buffer line and Disable speaker 2 line
//
//
// HARDWARE 
// Arduino UNO http://arduino.cc/en/Main/arduinoBoardUno
// Adafruit MONO AMP https://www.adafruit.com/product/2130
// SEED card shield http://www.seeedstudio.com/wiki/SD_Card_shield_V4.0
// 
// Audio files need to be WAV files, 8-bit, 8-32khz Sample Rate, mono. 



// these are compiler MACROS not VARS!
#include <Adafruit_NeoPixel.h> //WS2812B library
#include <SD.h>               // need to include the SD library
#include <TMRpcm.h>           //  also need to include this library...
#define SD_ChipSelectPin 4    //using digital pin 4 on arduino nano 328, can use other pins
#define sinTablelength 96 // length of my handy sin table
#define pixelPin 5 // NeoPixel Pin
#define bigButtpin  2 //giant button pin
#define modeButtpin 3 // mode selector pin - HAS TO BE 3 FOR INTERUPT
#define striplength 4 // length of leds
#define buttonDelay 250 //debounce yourself



//////////////// define some Vars


const uint8_t sinTable[] = {0,0,0,0, 4, 4, 4 , 4 , 8 , 8, 13 , 17 , 22 , 26 , 31 , 35 , 40 , 44 , 48 , 53 , 57 , 61 , 66 , 70 , 74 , 79 , 83 , 87 , 91 , 95 , 100 , 104 , 108 , 112 , 116 , 120 , 124 , 127 , 131 , 135 , 139 , 143 , 146 , 150 , 154 , 157 , 161 , 164 , 167 , 171 , 174 , 177 , 181 , 184 , 187 , 190 , 193 , 196 , 198 , 201 , 204 , 207 , 209 , 212 , 214 , 217 , 219 , 221 , 223 , 226 , 228 , 230 , 232 , 233 , 235 , 237 , 238 , 240 , 242 , 243 , 244 , 246 , 247 , 248 , 249 , 250 , 251 , 252 , 252 , 253 , 254 , 254 , 255 , 255 , 255 , 255 , 255};
// const uint8_t  sinTable[sinTablelength] = { 0 , 1 , 3 , 4 , 6 , 7 , 9 , 11 , 12 , 14 , 15 , 17 , 18 , 20 , 22 , 23 , 25 , 26 , 28 , 29 , 31 , 33 , 34 , 36 , 37 , 39 , 40 , 42 , 43 , 45 , 47 , 48 , 50 , 51 , 53 , 54 , 56 , 57 , 59 , 60 , 62 , 63 , 65 , 67 , 68 , 70 , 71 , 73 , 74 , 76 , 77 , 79 , 80 , 82 , 83 , 85 , 86 , 88 , 89 , 91 , 92 , 93 , 95 , 96 , 98 , 99 , 101 , 102 , 104 , 105 , 106 , 108 , 109 , 111 , 112 , 114 , 115 , 116 , 118 , 119 , 121 , 122 , 123 , 125 , 126 , 127 , 129 , 130 , 132 , 133 , 134 , 136 , 137 , 138 , 140 , 141 , 142 , 144 , 145 , 146 , 147 , 149 , 150 , 151 , 153 , 154 , 155 , 156 , 158 , 159 , 160 , 161 , 162 , 164 , 165 , 166 , 167 , 168 , 170 , 171 , 172 , 173 , 174 , 175 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 187 , 188 , 189 , 190 , 191 , 192 , 193 , 194 , 195 , 196 , 197 , 198 , 199 , 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 , 213 , 214 , 215 , 215 , 216 , 217 , 218 , 219 , 220 , 220 , 221 , 222 , 223 , 224 , 224 , 225 , 226 , 227 , 227 , 228 , 229 , 229 , 230 , 231 , 231 , 232 , 233 , 233 , 234 , 235 , 235 , 236 , 236 , 237 , 238 , 238 , 239 , 239 , 240 , 240 , 241 , 241 , 242 , 242 , 243 , 243 , 244 , 244 , 245 , 245 , 246 , 246 , 247 , 247 , 247 , 248 , 248 , 249 , 249 , 249 , 250 , 250 , 250 , 251 , 251 , 251 , 251 , 252 , 252 , 252 , 252 , 253 , 253 , 253 , 253 , 254 , 254 , 254 , 254 , 254 , 254 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 };
boolean fadeDir = true; // DEFINES up or down fade dir
volatile boolean makeNoise = false; // set in the hardware interupt to decide to play sound or not
uint8_t modeSpace = 1; // set the current mode
uint8_t pulsePos = 0;  // just 8bit vars for counters
uint8_t cyclePos = 0; // just 8bit vars for counters
unsigned long pulseLast = 0; // timers for software debounce of the buttons
unsigned long modeLast = 0;  // timers for software debounce of the buttons
unsigned long rainbowLast = 0;  // timers for software debounce of the buttons
volatile unsigned long last_micros;  // timers for software debounce of the interrupt
long debouncing_time = 15; //Debouncing Time in Milliseconds


//////////////// define some libraries

TMRpcm tmrpcm;   // create an object for use in this sketch
Adafruit_NeoPixel strip = Adafruit_NeoPixel(striplength, pixelPin, NEO_RGB + NEO_KHZ800); //create led object


////////////// LET THE GAMES BEGIN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void setup() {

  pinMode(pixelPin, OUTPUT);  // set the LED pin as an outpuy
  pinMode(bigButtpin, INPUT); // set up pull ups on the interrupt pin
  pinMode(modeButtpin, INPUT); // set up pull ups on the mode pin
  digitalWrite(bigButtpin, HIGH); // set up pull ups on the interrupt pin
  digitalWrite(modeButtpin, HIGH); // set up pull ups on the mode pin
  attachInterrupt(0, checkbutton, FALLING); //int.0 which is pin 2 on the uno
  
  SD.begin(SD_ChipSelectPin); // turn on the sd card.
  
  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  tmrpcm.quality(1); //best audio quality
  tmrpcm.setVolume(4); 


  strip.begin(); // strt the neopixels
  strip.show(); // Initialize all pixels to 'off'
  //tmrpcm.play("YAK.WAV");
}


//-----------------------------DA LOOP-----------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void loop() {

unsigned long theTime = millis();


if (digitalRead(modeButtpin) == LOW && (theTime - modeLast) > buttonDelay) {
    tmrpcm.disable(); // disable the playing sound if a mode switch occurs
    if (modeSpace < 4) {
          modeSpace++;} else {
          modeSpace = 1;
    }
    modeLast = millis();
}
  

 
  if(!tmrpcm.isPlaying()) {   // if not playing
     tmrpcm.setVolume(0); 
     digitalWrite(tmrpcm.speakerPin, LOW); //removes hiss by dropping the speaker pin low
   }


if (makeNoise == true) {   //  sounds 
     tmrpcm.disable();
     tmrpcm.setVolume(4); 
     if(modeSpace == 1) {tmrpcm.play("YAK2.WAV");}
        else if (modeSpace == 2) {tmrpcm.play("YAK.WAV");}
          else if (modeSpace == 3) {tmrpcm.play("SAD.WAV");}
               else if (modeSpace == 4) {tmrpcm.play("HEYKARL.WAV");}
      makeNoise = false;
   }
  
if (modeSpace == 1) {pulse('red', 10);} // Lights 
  else if (modeSpace == 2) {pulse('blue', 10);}
    else if (modeSpace == 3) {pulse('green', 10);}
         else if (modeSpace == 4) {rainbow(10);}

} 
// ----------------------------------------THE END
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



//-------------------------------------------------------------  Led Pulse Function

void pulse(char color, byte pulseSpeed){
  if ((unsigned long)(millis() - pulseLast) > pulseSpeed) {
    if (pulsePos < sinTablelength && fadeDir == true) {pulsePos++;} else {fadeDir = false; pulsePos--;}
    if (pulsePos == 1 && fadeDir == false) {fadeDir = true;}
    switch(color) {
      case 'green' : allLed(strip.Color(0,sinTable[pulsePos],0)); break;
      case 'red' : allLed(strip.Color(sinTable[pulsePos],0,0)); break;
      case 'blue' : allLed(strip.Color(0,0,sinTable[pulsePos])); break;
      default : break; 
    }
    pulseLast = millis();
  }
}


//-------------------------------------------------------------  Cycle color 

void rainbow(byte Speed) {
  if ((unsigned long)(millis() - rainbowLast) > Speed) {
   cyclePos++;
   allLed(Wheel(cyclePos & 255));
   strip.show();
   rainbowLast = millis();
 }
}


//-------------------------------------------------------------  Cycle color tool
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

//-------------------------------------------------------------  Black out the matrix

void blackMem() {                              
  for (uint8_t i = 0; i< striplength; i++) {
        strip.setPixelColor(i, 0);
    }
  }

//-------------------------------------------------------------  set all the led's one color 

void allLed(uint32_t c) {                                
  for (uint8_t i = 0; i< striplength; i++) {
        strip.setPixelColor(i, c);
    }

  strip.show();
  }



//------------------------------------------------------------- ISR to check for a proper button press
void checkbutton(){
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
      makeNoise = true;
      last_micros = micros();
    }
};





