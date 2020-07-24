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
 
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE(); // MIDI library init

#include "Motherboard6.h"

// Motherboard
Motherboard6 * device = Motherboard6::getInstance();
   
void setup() {
  Serial.begin(115200);
  
  while (!Serial && millis() < 2500); // wait for serial monitor

  // Starting sequence
  Serial.println("Ready!");
  // 0 = empty, 1 = button, 2 = potentiometer, 3 = encoder
  byte controls[6] = {1,1, 1,1, 3,3};
  device->init(controls);
  
  MIDI.setHandleNoteOn(onNoteOn);
  MIDI.setHandleNoteOff(onNoteOff);
  MIDI.begin(device->getMidiChannel());
  
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  
  device->setHandlePress(0, onButton1Press);
  device->setHandleLongPress(0, onButton1LongPress);
  device->setHandlePress(1, onButton2Press);
  device->setHandlePress(4, onRotary4Press);
  device->setHandleLongPress(4, onRotary4LongPress);
  device->setHandlePress(5, onRotary5Press);
  device->setHandleRotaryChange(5, onRotary5Change);
}

void loop() {
  device->update();
  
  MIDI.read(device->getMidiChannel());
  usbMIDI.read(device->getMidiChannel());
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


void onButton1Press(){
  Serial.println("onButton1Press");
}
void onButton2Press(){
  Serial.println("onButton2Press");
}
void onButton1LongPress(){
  Serial.println("onButton1LongPress");
}
void onRotary4Press(){
  Serial.println("onRotary4Press");
}
void onRotary4LongPress(){
  Serial.println("onRotary4LongPress");
}
void onRotary5Press(){
  Serial.println("onRotary5Press");
}

void onRotary5Change(bool value){
  if(value){
    Serial.println("onRotary5Change increment");
  }else{
    Serial.println("onRotary5Change decrement");
  }
}
