#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <MySensor.h> 
#include <SPI.h>

#define SENSOR_NAME "MyLightLED"
#define SENSOR_VERSION "1.0"
#define NUMPIXELS	10
#define CHILD_ID	55
#define PIN            6


MySensor gw;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Parameters
//Type
MyMessage msgV1(CHILD_ID, V_VAR1);
MyMessage msgV2(CHILD_ID, V_VAR2);
MyMessage msgV3(CHILD_ID, V_VAR3);
MyMessage msgV4(CHILD_ID, V_VAR4);

int effet = 9;
int delayval = 200;
int brig=100;
uint8_t len=40;
uint32_t col=255;

//Bleu
String colorhex="04B4AE";


/***
 * Dimmable LED initialization method
 */
void setup()  
{ 
  Serial.println( SENSOR_NAME ); 
  gw.begin(incomingMessage, AUTO, true);
  
  // Register the LED Dimmable Light with the gateway
  gw.present( CHILD_ID, S_LIGHT_LEVEL );
  
  gw.sendSketchInfo(SENSOR_NAME, SENSOR_VERSION);
  // Pull the gateway's current dim level - restore light level upon sendor node power-up
  //gw.request( 0, V_DIMMER );

  pixels.begin(); // This initializes the NeoPixel library.
}

/***
 *  Dimmable LED main processing loop 
 */
void loop() 
{
  gw.process();

  //colorize
  col = setColor(colorhex);
  if (effet == 1) { color(col); };
  if (effet == 2) { colorMiddle(col,len); };
  if (effet == 3) { colorBlink(col,delayval); };
  if (effet == 4) { colorElevator(col,delayval); };
  if (effet == 5) { colorDash(col,delayval); };
  if (effet == 6) { colorPingPong(col,delayval); };
  if (effet == 7) { colorDegrad(col); };
  if (effet == 8) { rainbow(30); };
  if (effet == 9) { rainbowCycle(20); };
}



void incomingMessage(const MyMessage &message) {

  //Type d'effet
  if (message.type == V_VAR1) {
     effet = message.getInt();
  }

  //Délai
  if (message.type == V_VAR2) {
     delayval = message.getInt();
  }

  //Brightness
  if (message.type == V_VAR4) {
     brig = message.getInt();
     pixels.setBrightness(brig);
  }

  //Couleur
  if (message.type == V_VAR3) {
     colorhex = message.getString();
   }

 Serial.print("EFFET=");
 Serial.println(effet);
 Serial.print("DELAI=");
 Serial.println(delayval);
 Serial.print("BRIGHTNESS=");
 Serial.println(brig);
 Serial.print("COULEUR=");
 Serial.println(colorhex);
 col = setColor(colorhex);
 
  // Inform the gateway of the current DimmableLED's SwitchPower1 and LoadLevelStatus value...
  //gw.send(lightMsg.set(currentLevel > 0 ? 1 : 0));

  // hek comment: Is this really nessesary?
  //gw.send( dimmerMsg.set(currentLevel) );
  
}

/***
 *  Set unique Color
 */
void color(uint32_t col) {
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, col);
    }
    pixels.show();
}

/***
 *  Set unique Color in the middle for a length in pourcentage
 */
void colorMiddle(uint32_t col, uint8_t len) {
    int nbpix=NUMPIXELS*len/100;
    int start=(NUMPIXELS-nbpix)/2;
    for(int i=0;i<NUMPIXELS;i++){
      if (i>=start && i<=nbpix+start) {
         pixels.setPixelColor(i, col);
      } else {
         pixels.setPixelColor(i, 0);
      }
    }
    pixels.show();
}


/***
 *  Set unique Color in the middle for a length in pourcentage
 */
void colorDegrad(uint32_t col) {
  
    long number = (long) strtol( &colorhex[0],NULL,16);
    long r = number >> 16;
    long g = number >> 8 & 0xFF;
    long b = number & 0xFF;
    
    int nbpix = NUMPIXELS / 2;
    Serial.println(nbpix);
    pixels.setPixelColor(nbpix-1, r, g, b);
    if ((NUMPIXELS % 2) == 0) {
       pixels.setPixelColor(nbpix, r, g, b);
       nbpix=nbpix-1;
    } 

    long pas_r = (255-r)/(nbpix+1);
    long pas_g = (255-g)/(nbpix+1);
    long pas_b = (255-b)/(nbpix+1);

    for(int i=0;i<nbpix;i++){
       int indice = i+1;
       pixels.setPixelColor(i, 255-indice*pas_r, 255-indice*pas_g, 255-indice*pas_b);
       pixels.setPixelColor(NUMPIXELS-1-i, 255-indice*pas_r, 255-indice*pas_g, 255-indice*pas_b);
    }
    pixels.show();
}


/***
 *  Set unique Color in blink mode with blink delay in ms
 */
void colorBlink(uint32_t col, int delayval) {
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, col);
    }
    pixels.show();
    delay(delayval);
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
    pixels.show();
    delay(delayval);
}

/***
 *  Rolling Chenillard mode (bottom to top to bottom...)
 */
void colorElevator(uint32_t col, int delayval) {
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, col);
      pixels.show();
      delay(delayval);
    }
    for(int i=NUMPIXELS-1;i>=0;i--){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
      pixels.show();
      delay(delayval);
    }
}

/***
 *  Dash mode with delay in ms...
 */
void colorDash(uint32_t col, int delayval) {
    for(int i=0;i<NUMPIXELS;i++){
      if (i==0) pixels.setPixelColor(NUMPIXELS-1, pixels.Color(0,0,0));
      if (i>0) pixels.setPixelColor(i-1, pixels.Color(0,0,0));
      pixels.setPixelColor(i, col);
      pixels.show();
      delay(delayval);
      
    }
}

/***
 *  Same as Chenillard mode but bottom to top then top to bottom
 */
void colorPingPong(uint32_t col, int delayval) {
    for(int i=0;i<NUMPIXELS;i++){
      if (i==0) pixels.setPixelColor(NUMPIXELS-1, pixels.Color(0,0,0));
      if (i>0) pixels.setPixelColor(i-1, pixels.Color(0,0,0));
      pixels.setPixelColor(i, col);
      pixels.show();
      delay(delayval);
    }
    for(int i=NUMPIXELS-1;i>=0;i--){
      if (i>=0) pixels.setPixelColor(i+1, pixels.Color(0,0,0));
      pixels.setPixelColor(i, col);
      pixels.show();
      delay(delayval);
    }
}


/***
 *  Rainbow Cycle
 */
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


/***
 *  Rainbow
 */
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


uint32_t setColor(String hex) {
     long number = (long) strtol( &hex[0],NULL,16);
     return pixels.Color(number >> 16,number >> 8 & 0xFF,number & 0xFF);        
}

