/*
 __    _  _____   ___      ___ _  _     __    _ ______ _    
/__|_|/ \(_  |     | |\|    | |_)|_||\|(_ |  |_| |  | / \|\|
\_|| |\_/__) |    _|_| |    | | \| || |__)|__| | | _|_\_/| |

If you enjoy my work and music please consider donating.

https://www.ghostintranslation.com/
https://ghostintranslation.bandcamp.com/
https://www.instagram.com/ghostintranslation/
https://www.youtube.com/channel/UCcyUTGTM-hGLIz4194Inxyw
https://github.com/ghostintranslation
*/

/**
 * This is an example sketch
 * The first led switch on when a midi note on is received and 
 * switch off when a midi note off is received.
 */

#include "Motherboard6.h"

// Motherboard
Motherboard6 * device = Motherboard6::getInstance();
  
void setup() {
  Serial.begin(115200);
  
  while (!Serial && millis() < 2500); // wait for serial monitor

  Serial.println("000");
  // Init
  device->init(
    "Synth",
    {
      Potentiometer, Potentiometer,
      Potentiometer, Potentiometer,
      Potentiometer, Potentiometer
    }
  );
  Serial.println("Ready!");
  
  device->setHandleMidiNoteOn(onNoteOn);
  device->setHandleMidiNoteOff(onNoteOff);
//  device->setHandleMidiControlChange(onControlChange);// TODO: Make it specific to input ids
  device->setHandleMidiControlChange(2, 0,  "Mode",   onControlChangeMode);
  device->setHandleMidiControlChange(1,     "Param",  onControlChangeParam);
  device->setHandleMidiControlChange(2,     "Shape",  onControlChangeShape);
  device->setHandleMidiControlChange(3,     "FM",     onControlChangeFM);
  device->setHandleMidiControlChange(4,     "Attack", onControlChangeAttack);
  device->setHandleMidiControlChange(5,     "Release",onControlChangeRelease);
  device->setHandleMidiControlChange(55,    "Custom", onControlChangeCustom);
  
  device->setHandlePressDown(0, onButton1Press);
  device->setHandlePressUp(0, onButton1Release);
//  device->setHandleLongPressDown(0, onButton1LongPress);
  device->setHandlePressDown(1, onButton2Press);
  device->setHandlePressDown(4, onRotary4Press);
  device->setHandleLongPressDown(4, onRotary4LongPress);
  device->setHandlePressDown(5, onRotary5Press);
//  device->setHandlePotentiometerChange(2, onPotentiometer2Change);
  device->setHandleRotaryChange(5, onRotary5Change);
}

void loop() {
  device->update();
}


/**
 * Midi note on callback
 */
void onNoteOn(byte channel, byte note, byte velocity) {
  device->setLED(0, 1);
}

/**
 * Midi note off callback
 */
void onNoteOff(byte channel, byte note, byte velocity) {
  device->setLED(0, 0);
}

/**
 * Midi Control Change callback
 */
void onControlChange(byte channel, byte control, byte value) {
  Serial.println("onControlChange");
}

/**
 * Midi Control Change Mode callback
 */
void onControlChangeMode(byte value) {
  Serial.println("onControlChangeMode");
}
/**
 * Midi Control Change Param callback
 */
void onControlChangeParam(byte value) {
  Serial.println("onControlChangeParam");
}
/**
 * Midi Control Change Shape callback
 */
void onControlChangeShape(byte value) {
  Serial.println("onControlChangeShape");
}
/**
 * Midi Control Change FM callback
 */
void onControlChangeFM(byte value) {
  Serial.println("onControlChangeFM");
}
/**
 * Midi Control Change Attack callback
 */
void onControlChangeAttack(byte value) {
  Serial.println("onControlChangeAttack");
}
/**
 * Midi Control Change Release callback
 */
void onControlChangeRelease(byte value) {
  Serial.println("onControlChangeRelease");
}
/**
 * Midi Control Change Custom callback
 */
void onControlChangeCustom(byte value) {
  Serial.println("onControlChangeCustom");
}

void onButton1Press(byte inputIndex){
  Serial.println("onButton1Press");
    device->setLED(0, 1);
}
void onButton1Release(byte inputIndex){
  Serial.println("onButton1Release");
    device->setLED(0, 0);
}
void onButton2Press(byte inputIndex){
  Serial.println("onButton2Press");
}

void onButton1LongPress(byte inputIndex){
  Serial.println("onButton1LongPress");
}

void onRotary4Press(byte inputIndex){
  Serial.println("onRotary4Press");
}

void onRotary4LongPress(byte inputIndex){
  Serial.println("onRotary4LongPress");
}

void onRotary5Press(byte inputIndex){
  Serial.println("onRotary5Press");
}

void onPotentiometer2Change(byte inputIndex, unsigned int value, int diffToPrevious){
  Serial.print("onPotentiometer2Change: ");
  Serial.print(value);
  Serial.print(" ");
  Serial.print(diffToPrevious);
  Serial.println("");
}

void onRotary5Change(bool value){
  if(value){
    Serial.println("onRotary5Change increment");
  }else{
    Serial.println("onRotary5Change decrement");
  }
}
