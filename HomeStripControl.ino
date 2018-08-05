#include "SoftwareSerial.h"
#include <FastLED.h>

const int PIN = 14;
const int NUMPIX = 243; //Strip is 243 total
//Adafruit_NeoPixel strip;
CRGB leds[NUMPIX];

int fillUp = NUMPIX; //Fill up color[] with different colors before lighting up the LED's

const int DEBUG = 0; //Only print to serial if debug is 1 to reduce serail overhead

int enabled = 0; //Turn lights off and on while preserving last setting
int colorMode = 0; //Mode 0 = nothing, use enabled to turn lights off while preserving color mode. 
                   //Mode 1 = rainbow, 2 = bounce, 3 = static color, 4 = stars 
int brightness = 255; 
int RRR = 150; //Global color settings. (Deafult is white)
int GGG = 255; 
int BBB = 255;
int PCMode = 1; //Turn off the first 50 LEDS to keep the wall behind my monitors dark



SoftwareSerial softSerial(13, 15); // RX, TX

void setup() {
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMPIX);
  digitalWrite(3, LOW);
  pinMode(3, INPUT);
  softSerial.begin(9600);
  while(!softSerial){}

  if(DEBUG == 1)
    Serial.begin(9600);
}

void loop() {
  //Serial.println("Running");
  checkPin();
  if (enabled == 0){ //Turn off the lights and prevent other affects from running. 
    staticColor(0, 0, 0);
    fillUp = NUMPIX;
    return;
  }
  if (colorMode == 1) {
    rainbowCycle(100, 20);
  } else {fillUp = NUMPIX;}
  
  if (colorMode == 2)
    bounce(30); 
  if (colorMode == 3)
    staticColor(RRR, GGG, BBB);
  if (colorMode == 4)
    stars(7, 255, 15, 0);

}
int checkPin() { //Returns 1 if pin says to change mode. Returns 0 otherwise
  
  while (softSerial.available() > 0) {
    //delay(50);
    String op = softSerial.readStringUntil(' '); //Read the op command
    char com = op.charAt(0);
    if(DEBUG == 1){
      Serial.print("Op: ");
      Serial.println(op);
    }


    if(com == '0'){ //On/ off
      String peram = softSerial.readStringUntil(' ');
      if(DEBUG == 1){
        Serial.print("Enable: ");
        Serial.println(peram);
      }
      if(peram.charAt(0) == '0'){
        enabled = 0;
      }
      if(peram.charAt(0) == '1'){
        enabled = 1;
      }
      return 1;
    }
    
    if(com == '1'){ //Rainbow
      colorMode = 1;
      enabled = 1;
      return 1;
    }

    if(com == '2'){ //Bounce
      colorMode = 2;
      enabled = 1;
      return 1;
    }
    
    if(com == '3'){ //Static 
      colorMode = 3;
      enabled = 1;
      return 1;
    }

    if(com == '4'){ //Stars
      colorMode = 4;
      enabled = 1;
      return 1;
    }
    
    if(com == 'c'){ //Set color
      getSerialRGB();
      enabled = 1;
      if(colorMode == 1 || colorMode == 4){ //If the current mode doesn't support changing the color
        colorMode = 3;
        return 1;
      }
    }

    if(com == 'b'){ //Set brightness
      brightness = softSerial.readStringUntil(' ').toInt();
      if(DEBUG == 1){
        Serial.print("Brightness: ");
        Serial.println(brightness);
      }
      FastLED.setBrightness(brightness);
      FastLED.show();
    }
    enabled = 1;
    
  }
  
  return 0;
}
void getSerialRGB(){ //If the op command from serial has RGB peramiters, this function will put them into int RRR,GGG,BBB
  String red = softSerial.readStringUntil(' '); //Read the red value
  RRR = red.toInt();
  if(DEBUG == 1){
    Serial.print("Red: ");
    Serial.println(RRR);
  }
  String green = softSerial.readStringUntil(' '); //Read the red value
  GGG = green.toInt();
  if(DEBUG == 1){
    Serial.print("Green: ");
    Serial.println(GGG);
  }

  String blue = softSerial.readStringUntil(' '); //Read the red value
  BBB = blue.toInt();
  if(DEBUG == 1){
    Serial.print("Blue: ");
    Serial.println(BBB);
  }
  
}

void stars(int numStars, int maxBrightness, int delayMils, int color) { //delayMils is delay
  int change = 1;
  int minRGB = 0;
  int maxBrightDiv = 3; //Reduce initial color brightness to reduce POP in effect
  staticColor(0, 0, 0); //Initialize the strand as black
  int starR[numStars];//If stars are B/W (color == 0), only use starR for brightness
  int starG[numStars];
  int starB[numStars];
  byte starLoc[numStars];
  byte starInc[numStars]; //If starInc == 1, the brightness is increasing, 0 means is decreasing
  for (int x = 0; x < numStars; x++) { //Initial set star color brightnesses and locations
    starR[x] = random(minRGB, maxBrightness / maxBrightDiv);
    if (color) {
      starG[x] = random(minRGB, maxBrightness / maxBrightDiv);
      starB[x] = random(minRGB, maxBrightness / maxBrightDiv);
    } else {
      starG[x] = 0;
      starB[x] = 0;
    }
    starLoc[x] = random(0, NUMPIX);
    starInc[x] = 1;
  }
  while (1) {
    for (int x = 0; x < numStars; x++) { //Increase or decrease star brightness/ relocate stars if starR&&starG&&starB == 0
      /*
      softSerial.write("R: "); softSerial.print(starR[0]); 
      softSerial.write(" G: "); softSerial.print(starG[0]); 
      softSerial.write(" B: "); softSerial.print(starB[0]); softSerial.write("\n");
      */
      
      if (starInc[x] == 0) { //Decrease brightness
        starR[x] -= change;
        if (color) {
          starG[x] -= change;
          starB[x] -= change;
        }
      } else { //Increase brightness
        starR[x] += change;
        if (color) {
          starG[x] += change;
          starB[x] += change;
        }
      }
      for (int x = 0; x < numStars; x++) { //Show the stars on the strand
      if (color){
        if(starR[x] < 0)
          starR[x] = 0;
        if(starG[x] < 0)
          starG[x] = 0;
        if(starB[x] < 0)
          starB[x] = 0;
        if(starR[x] > maxBrightness)
          starR[x] = maxBrightness;
        if(starG[x] > maxBrightness)
          starG[x] = maxBrightness;
        if(starB[x] > maxBrightness)
          starB[x] = maxBrightness;
        leds[starLoc[x]].setRGB(starR[x], starG[x], starB[x]);
      }
      else
        leds[starLoc[x]].setRGB(starR[x], starR[x], starR[x]);
      }
      if (starR[x] <= 1 && starG[x] <= 1 && starB[x] <= 1) { //If the values are too low, reset the star
        leds[starLoc[x]].setRGB(0,0,0);
        //softSerial.print("star reset\n");
        starLoc[x] = random(0, NUMPIX);
        starR[x] = random(minRGB, maxBrightness / maxBrightDiv);
        starInc[x] = 1;
        if (color) {
          starG[x] = random(minRGB, maxBrightness / maxBrightDiv);
          starB[x] = random(minRGB, maxBrightness / maxBrightDiv);
        }
      }
      if (starR[x] >= maxBrightness || starG[x] >= maxBrightness || starB[x] >= maxBrightness) { //If the values are too high, flip starInc to decrease
        starInc[x] = 0;
      }
    }
    
    FastLED.show();
    if (checkPin()) {
      return;
    }
    delay(delayMils);
  }
}

void staticColor(int r, int g, int b) {

  for (int x = 0; x < NUMPIX; x++) {
    leds[x].setRGB(r, g, b);

  }
  
  if(PCMode == 1)
    staticColorSection(0,0,0,0,68);
    
  FastLED.show();
}
void staticColorSection(int r, int g, int b, int start, int endi) {

  for (int x = start; x < endi; x++) {
    leds[x].setRGB(r, g, b);

  }
  FastLED.show();
}

void bounce(int speed) {
  int backwards = 1;
  for (int x = 0; x < NUMPIX; x += backwards) {
    for (int y = 0; y < NUMPIX; y++) {
      leds[y].setRGB(0, 0, 0);
      //strip.setPixelColor(y, 0, 0, 0);
    }
    leds[x].setRGB(RRR, GGG, BBB);
    //strip.setPixelColor(x, color);
    FastLED.show();
    //strip.show();
    delay(speed);
    if (checkPin() == 1)
      return;

    if (x >= NUMPIX - 1) {
      backwards = -1;
    }
    if (x <= 0) {
      backwards = 1;
    }
  }
}

void rainbowCycle(int speed, int rate) { //Speed: delay for next color change, rate: how different the next color is
  byte colors[NUMPIX][3];
  double rgbColor[3];
  rgbColor[0] = 255;
  rgbColor[1] = 0;
  rgbColor[2] = 0;



  for (int decColor = 0; decColor < 3; decColor += 1) {
    int incColor = 0;
    if (decColor == 2) {
      incColor = 0;
    } else {
      incColor = decColor + 1;
    }

    for (int i = 0; i < (256 - rate); i += rate) { //i += 2 was used for smoother led color transition.
      rgbColor[decColor] -= rate;
      rgbColor[incColor] += rate;
      if (fillUp > 0) { //Fill colors with values before turning on LED's
        colors[fillUp - 1][0] = rgbColor[0];
        colors[fillUp - 1][1] = rgbColor[1];
        colors[fillUp - 1][2] = rgbColor[2];
        fillUp--;

      } else {
        for (int x = NUMPIX - 1; x > 0; x--) {
          colors[x][0] = colors [x - 1][0];
          colors[x][1] = colors [x - 1][1];
          colors[x][2] = colors [x - 1][2];
        }
        colors[0][0] = rgbColor[0];
        colors[0][1] = rgbColor[1];
        colors[0][2] = rgbColor[2];
        for (int x = 0; x < NUMPIX; x++) {
          //if(x < 16)//Since the board is 2 rows of 16
          //strip.setPixelColor(x, colors[x]);
          leds[x].setRGB(colors[x][0], colors[x][1], colors[x][2]);
        }

        //strip.show();
        if(PCMode == 1)
          staticColorSection(0,0,0,0,68);
         
        delay(speed);
        FastLED.show();
        if (checkPin() == 1)
          return;
      }
    }


  }
}
