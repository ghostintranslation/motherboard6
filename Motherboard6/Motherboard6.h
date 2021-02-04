#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE(); // MIDI library init

#ifndef Motherboard6_h
#define Motherboard6_h

/*
 * Motherboard6
 * v1.3.0
 */
class Motherboard6{
  public:
    struct Input{
      byte id = 0;
      byte type = 0;
      String name = "";
      byte midiCC = 0;
      byte midiChannel = 0;
    };
    
  private:
    static Motherboard6 *instance;
    Motherboard6();
    
    byte currentRow = 0;
    byte currentLed = 0;
    byte currentInput = 0;
    byte columnsNumber = 2;
    byte ioNumber;
    byte analogResolution = 10;
    byte midiChannel = 0;
    
    Input *inputs;

    // LEDs
    byte *leds;
    byte *ledsBrightness;
    unsigned int *ledsDuration;
    // Buttons
    bool *buttons;
    // Potentiometers
    unsigned int *potentiometers;
    unsigned int *potentiometersPrevious;
    // For smoothing purposes
    unsigned int *potentiometersTemp;
    byte *potentiometersReadings;

    // Encoders
    int *encoders;
    bool *encodersSwitch;
    byte *encodersState;
    byte currentEncPinA;
    byte currentEncPinB;
    #define R_START 0x0
    #define R_CW_FINAL 0x1
    #define R_CW_BEGIN 0x2
    #define R_CW_NEXT 0x3
    #define R_CCW_BEGIN 0x4
    #define R_CCW_FINAL 0x5
    #define R_CCW_NEXT 0x6
    // No complete step yet.
    #define DIR_NONE 0x0
    // Clockwise step.
    #define DIR_CW 0x10
    // Anti-clockwise step.
    #define DIR_CCW 0x20
    const byte ttable[7][4] = {
      // R_START
      {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
      // R_CW_FINAL
      {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
      // R_CW_BEGIN
      {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
      // R_CW_NEXT
      {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
      // R_CCW_BEGIN
      {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
      // R_CCW_FINAL
      {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
      // R_CCW_NEXT
      {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
    };
    
    // Debug clock
    elapsedMillis clockDebug;
    // Main clock
    elapsedMicros clockMain;
    const unsigned int intervalClockMain = 5000;
    // Leds clocks
    const unsigned int intervalDisplay = 10;
    elapsedMicros clockDisplay;
    const unsigned int intervalDisplayFlash = 400;
    elapsedMillis clockDisplayFlash;
    // Inputs clock
    const unsigned int intervalInputs = 100;
    elapsedMicros clockInputs;

    // Callbacks
    using PressDownCallback = void (*)(byte);
    PressDownCallback *inputsPressDownCallback;
    using LongPressDownCallback = void (*)(byte);
    LongPressDownCallback *inputsLongPressDownCallback;
    using PressUpCallback = void (*)(byte);
    PressUpCallback *inputsPressUpCallback;
    using LongPressUpCallback = void (*)(byte);
    LongPressUpCallback *inputsLongPressUpCallback;
    elapsedMillis *inputsPressTime;
    bool *inputsLongPressDownFired;
    using PotentiometerChangeCallback = void (*)(byte, unsigned int, int);
    PotentiometerChangeCallback *inputsPotentiometerChangeCallback;
    using RotaryChangeCallback = void (*)(bool);
    RotaryChangeCallback *inputsRotaryChangeCallback;

    // Callbacks triggers
    void triggerPressCallbacks(byte inputIndex, bool value);
    void triggerPotentiometerChangeCallback(byte inputIndex, unsigned int value, unsigned int diff);
    void triggerRotaryChangeCallback(byte inputIndex, bool value);

    // MIDI Callbacks
    using MidiNoteOnCallback = void (*)(byte, byte, byte);
    MidiNoteOnCallback midiNoteOnCallback;
    using MidiNoteOffCallback = void (*)(byte, byte, byte);
    MidiNoteOffCallback midiNoteOffCallback;
    using MidiControlChangeCallback = void (*)(byte, byte, byte);
    MidiControlChangeCallback midiControlChangeCallback;
    using MidiSysExCallback = void (*)(const uint8_t*, uint16_t, bool);
    MidiSysExCallback midiSysExCallback;
    
    
    void updateDisplay();
    void iterateDisplay();
    void iterateInputs();
    void readCurrentInput();
    void readButton(byte inputIndex);
    void readPotentiometer(byte inputIndex);
    void readEncoder(byte inputIndex);
    void readMidiChannel();
    void setMainMuxOnLeds();
    void setMainMuxOnPots();
    void setMainMuxOnEncoders1();
    void setMainMuxOnEncoders2();
    void setMainMuxOnChannel();
    void printInputs();
    void printLeds();
    static void handleMidiSysEx(const uint8_t* data, uint16_t length, bool last);
    static void handleMidiControlChange(byte channel, byte control, byte value);
    
  public:
    static Motherboard6 *getInstance();
    void init(Input *inputs);
    void update();
    void setLED(byte ledIndex, byte ledStatus, byte ledBrightness=255);
    void setAllLED(unsigned int binary, byte ledStatus);  
    void toggleLED(byte index);
    void resetAllLED();
    void writeLED(byte index);
    int getInput(byte index);
    bool getEncoderSwitch(byte index);
    int getAnalogMaxValue();
    int getAnalogMinValue();
    byte getMidiChannel();

    // Callbacks
    void setHandlePressDown(byte inputIndex, PressDownCallback fptr);
    void setHandleLongPressDown(byte inputIndex, LongPressDownCallback fptr);
    void setHandlePressUp(byte inputIndex, PressUpCallback fptr);
    void setHandleLongPressUp(byte inputIndex, LongPressUpCallback fptr);
    void setHandlePotentiometerChange(byte inputIndex, PotentiometerChangeCallback fptr);
    void setHandleRotaryChange(byte inputIndex, RotaryChangeCallback fptr);

    // MIDI Callbacks
    void setHandleMidiNoteOn(MidiNoteOnCallback fptr);
    void setHandleMidiNoteOff(MidiNoteOffCallback fptr);
    void setHandleMidiControlChange(MidiControlChangeCallback fptr);
    void setHandleMidiSysEx(MidiSysExCallback fptr);
};

// Instance pre init
Motherboard6 * Motherboard6::instance = nullptr;

/**
 * Constructor
 */
inline Motherboard6::Motherboard6(){
  this->ioNumber = 3 * this->columnsNumber;
  
  this->inputs = new Input[this->ioNumber];
  this->leds = new byte[this->ioNumber];
  this->ledsBrightness = new byte[this->ioNumber];
  this->ledsDuration = new unsigned int[this->ioNumber];
  this->buttons = new bool[this->ioNumber];
  this->potentiometers = new unsigned int[this->ioNumber];
  this->potentiometersPrevious = new unsigned int[this->ioNumber];
  this->potentiometersTemp = new unsigned int[this->ioNumber];
  this->potentiometersReadings = new byte[this->ioNumber];
  this->encoders = new int[this->ioNumber];
  this->encodersState = new byte[this->ioNumber];
  this->encodersSwitch = new bool[this->ioNumber];
  this->inputsPressDownCallback = new PressDownCallback[this->ioNumber];
  this->inputsLongPressDownCallback = new PressDownCallback[this->ioNumber];
  this->inputsPressUpCallback = new PressUpCallback[this->ioNumber];
  this->inputsLongPressUpCallback = new PressUpCallback[this->ioNumber];
  this->inputsPressTime = new elapsedMillis[this->ioNumber];
  this->inputsLongPressDownFired = new bool[this->ioNumber];
  this->inputsPotentiometerChangeCallback = new PotentiometerChangeCallback[this->ioNumber];
  this->inputsRotaryChangeCallback = new RotaryChangeCallback[this->ioNumber];

  for(byte i = 0; i < this->ioNumber; i++){
    this->inputs[i] = {id:i, type:0, name:"", midiCC:0};
    this->leds[i] = 0;
    this->ledsBrightness[i] = 255;
    this->ledsDuration[i] = 0;
    this->buttons[i] = true;
    this->potentiometers[i] = 0;
    this->potentiometersPrevious[i] = 0;
    this->potentiometersTemp[i] = 0;
    this->potentiometersReadings[i] = 0;
    this->encoders[i] = 0;
    this->encodersState[i] = 0;
    this->encodersSwitch[i] = true;
    this->inputsPressDownCallback[i] = nullptr;
    this->inputsLongPressDownCallback[i] = nullptr;
    this->inputsPressUpCallback[i] = nullptr;
    this->inputsLongPressUpCallback[i] = nullptr;
    this->inputsPressTime[i] = 0;
    this->inputsLongPressDownFired[i] = false;
    this->inputsPotentiometerChangeCallback[i] = nullptr;
    this->inputsRotaryChangeCallback[i] = nullptr;
  }

}

/**
 * Singleton instance
 */
inline Motherboard6 *Motherboard6::getInstance()    {
  if (!instance)
     instance = new Motherboard6;
  return instance;
}

/**
 * Init
 */
inline void Motherboard6::init(Input *inputs){
  // Init of the inputs
  for(byte i = 0; i < this->ioNumber; i++){
    this->inputs[i] = inputs[i];
  }
  
  // Main multiplexer
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  // 2nd level multiplexers
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(14, OUTPUT);

  // Matrix Rows
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  
  // Inputs / LEDs
  pinMode(22, INPUT_PULLUP);

  analogReadResolution(this->analogResolution);
  this->readMidiChannel();
  
  // Init sequence
  for(byte i = 0; i<this->ioNumber; i++){
    this->setLED(i, 1);
    this->iterateDisplay();
    this->updateDisplay();
    delay(50);
  }
  this->resetAllLED();
  this->updateDisplay();

  // MIDI init
  MIDI.setHandleNoteOn(this->midiNoteOnCallback);
  MIDI.setHandleNoteOff(this->midiNoteOffCallback);
  MIDI.setHandleControlChange(this->handleMidiControlChange);
  MIDI.begin(this->getMidiChannel());
  usbMIDI.setHandleNoteOn(this->midiNoteOnCallback);
  usbMIDI.setHandleNoteOff(this->midiNoteOffCallback);
  usbMIDI.setHandleSystemExclusive(this->handleMidiSysEx);
  usbMIDI.setHandleControlChange(this->handleMidiControlChange);
}

/**
 * Update
 */
inline void Motherboard6::update(){
  // Main clock
  if (this->clockMain >= this->intervalClockMain) {
    this->clockMain = 0;
  }
  
  if (this->clockMain > this->intervalClockMain / 2) {
    // Leds
    
    // Clock for flashing the LEDs
    if (this->clockDisplayFlash >= this->intervalDisplayFlash) {
      this->clockDisplayFlash = 0;
    }

    // Clorck for iterating over each LED
    if (this->clockDisplay >= this->intervalDisplay) {
      this->iterateDisplay();
      this->clockDisplay = 0;
    }
    
    this->updateDisplay();
  }else{
    // Inputs

    // At the end of the clock we iterate to next input
    if (this->clockInputs >= this->intervalInputs) {
      this->iterateInputs();
      this->clockInputs = 0;
    }else{
      // Reading the current input
      this->readCurrentInput();
    }
  }

  // Debug
  if (this->clockDebug >= 100) {
//    this->printInputs();
//    this->printLeds();
    this->clockDebug = 0;
  }

  
  MIDI.read(this->getMidiChannel());
  usbMIDI.read(this->getMidiChannel());
}


/**
 * Main multiplexer on LEDs
 */
inline void Motherboard6::setMainMuxOnLeds(){
  pinMode(22, OUTPUT);
  digitalWriteFast(2, LOW);
  digitalWriteFast(3, LOW);
  digitalWriteFast(4, LOW);
}

/**
 * Main multiplexer on Potentiometers
 */
inline void Motherboard6::setMainMuxOnPots(){
  pinMode(22, INPUT);
  digitalWriteFast(2, LOW);
  digitalWriteFast(3, HIGH);
  digitalWriteFast(4, LOW);
}

/**
 * Main multiplexer on Encoders
 */
inline void Motherboard6::setMainMuxOnEncoders1(){
  pinMode(22, INPUT_PULLUP);
  digitalWriteFast(2, LOW);
  digitalWriteFast(3, LOW);
  digitalWriteFast(4, HIGH);
}

/**
 * Main multiplexer on Encoder's switches
 */
inline void Motherboard6::setMainMuxOnEncoders2(){
  pinMode(22, INPUT_PULLUP);
  digitalWriteFast(2, HIGH);
  digitalWriteFast(3, LOW);
  digitalWriteFast(4, HIGH);
}

/**
 * Main multiplexer on Channel
 */
inline void Motherboard6::setMainMuxOnChannel(){
  pinMode(22, INPUT_PULLUP);
  digitalWriteFast(2, LOW);
  digitalWriteFast(3, HIGH);
  digitalWriteFast(4, HIGH);
}

/**
 * Iterate LEDs
 */
inline void Motherboard6::iterateDisplay(){
  // Iterating only on the acive leds
  // to save time between one led's iterations
  // and so improve brightness
  for(byte i = this->currentLed+1; i < this->currentLed+1 + this->ioNumber; i++){
    byte j = i % this->ioNumber;
    if(this->leds[j] > 0){
      this->currentLed = j;
      break;
    }
  }
}

/**
 * Refresh the leds
 */
inline void Motherboard6::updateDisplay(){
  this->setMainMuxOnLeds();
  
  byte r0 = bitRead(this->currentLed, 0);   
  byte r1 = bitRead(this->currentLed, 1);    
  byte r2 = bitRead(this->currentLed, 2);
  digitalWriteFast(5, r0);
  digitalWriteFast(9, r1);
  digitalWriteFast(14, r2);
    
  switch(this->leds[this->currentLed]){
    case 1:
      // Solid lightw
      this->writeLED(this->currentLed);
    break;
    
    case 2:
      // Slow flashing
      digitalWriteFast(22, HIGH);
      if (this->clockDisplayFlash % 400 > 200) {
        this->writeLED(this->currentLed);
      }
    break;
    
    case 3:
      // Fast flashing
      digitalWriteFast(22, HIGH);
      if (this->clockDisplayFlash % 200 > 100) {
        this->writeLED(this->currentLed);
      }
    break;

    case 4:
      // Solid for 50 milliseconds
      this->writeLED(this->currentLed);
      if (this->ledsDuration[this->currentLed] == 0) {
        this->ledsDuration[this->currentLed] = (clockDisplayFlash + 50) % intervalDisplayFlash;
      }
  
      if (this->clockDisplayFlash >= this->ledsDuration[this->currentLed]) {
        digitalWriteFast(22, HIGH);
        this->leds[this->currentLed] = 0;
        this->ledsDuration[this->currentLed] = 0;
      }
    break;

    case 5:
      // Solid low birghtness
      this->ledsBrightness[this->currentLed] = 128;
      this->writeLED(this->currentLed);
    break;

    default:
      digitalWriteFast(22, HIGH);
    break;
  }
}


/**
 * Iterate over the inputs
 */
inline void Motherboard6::iterateInputs(){
  this->currentInput++;
  this->currentInput = this->currentInput % this->ioNumber;
}

/**
 * Read value of current inout
 */
inline void Motherboard6::readCurrentInput(){
  switch(this->inputs[this->currentInput].type){
    default:
    case 0:
      // Silence is golden
    break;
    
    case 1:
      this->readButton(this->currentInput);
    break;
    case 2:
      this->readPotentiometer(this->currentInput);
    break;
    case 3:
      this->readEncoder(this->currentInput);
    break;
  }
}

/**
 * Get button value
 * @param byte inputeIndex The index of the input
 */
inline void Motherboard6::readButton(byte inputIndex) {
  this->setMainMuxOnEncoders2();

  byte rowNumber = inputIndex / this->columnsNumber;

  for (byte i = 0; i < 3; i++) {
    if (i == rowNumber) {
      digitalWriteFast(15 + i, LOW);
    } else {
      digitalWriteFast(15 + i, HIGH);
    }
  }

  byte columnNumber = inputIndex % this->columnsNumber;

  byte r0 = bitRead(columnNumber, 0);
  byte r1 = bitRead(columnNumber, 1);
  byte r2 = bitRead(columnNumber, 2);
  digitalWriteFast(5, r0);
  digitalWriteFast(9, r1);
  digitalWriteFast(14, r2);

  // Giving some time to the Mux and pins to switch
  if (this->clockInputs > this->intervalInputs / 1.5) {
    
    // Reading the new value
    bool newReading = digitalReadFast(22);

    // If there is a short or a long press callback on that input
    if(this->inputsPressDownCallback[inputIndex] != nullptr ||
       this->inputsPressUpCallback[inputIndex] != nullptr ||
       this->inputsLongPressDownCallback[inputIndex] != nullptr ||
       this->inputsLongPressUpCallback[inputIndex] != nullptr){
        
      // Inverted logic, 0 = button pushed
      // If previous value is not pushed and now is pushed
      // So if it's pushed
      if(this->buttons[inputIndex] && !newReading){ 
        // Start the counter of that input
        this->inputsPressTime[inputIndex] = 0;
        this->inputsLongPressDownFired[inputIndex] = false;
        
        // If there is a short press down callback on that input, and there is no Long Press down
        if(this->inputsLongPressDownCallback[inputIndex] == nullptr &&
           this->inputsPressDownCallback[inputIndex] != nullptr){
          this->inputsPressDownCallback[inputIndex](inputIndex);
        }
      }
  
      // If it stayed pressed for 200ms and Long Press Down callback hasn't been fired yet
      if(!this->buttons[inputIndex] && !newReading){ 
        if(this->inputsPressTime[inputIndex] >= 200 && !this->inputsLongPressDownFired[inputIndex]){
          
          if(this->inputsLongPressDownCallback[inputIndex] != nullptr){
            // Fire the callback
            this->inputsLongPressDownCallback[inputIndex](inputIndex);
            this->inputsLongPressDownFired[inputIndex] = true;
          }
        }
      }
  
      // If it's released
      if(!this->buttons[inputIndex] && newReading){ 
        // How long was it pressed
        if(this->inputsPressTime[inputIndex] < 200){
          // Short press
          
          // If there is a short press callback on that input
          if(this->inputsPressUpCallback[inputIndex] != nullptr){
            this->inputsPressUpCallback[inputIndex](inputIndex);
          }
        }else{
          // Long press
          
          // If there is a long press callback on that input
          if(this->inputsLongPressUpCallback[inputIndex] != nullptr){
            this->inputsLongPressUpCallback[inputIndex](inputIndex);
          }else if(this->inputsPressUpCallback[inputIndex] != nullptr){
            // If the input was pressed for a long time but there is only a short press callback
            // the short press callback should still be called
            this->inputsPressUpCallback[inputIndex](inputIndex);
          }
        }
      }
    }
  
    // Updating the value
    this->buttons[inputIndex] = newReading;
  }
}

inline void Motherboard6::triggerPressCallbacks(byte inputIndex, bool value){
  if(value){
    this->inputsPressDownCallback[inputIndex](inputIndex);
  }else{
    this->inputsPressUpCallback[inputIndex](inputIndex);
  }
}

/**
 * Get potentiometer value
 * @param byte inputeIndex The index of the input
 */
inline void Motherboard6::readPotentiometer(byte inputIndex){
  this->setMainMuxOnPots();
  
  byte r0 = bitRead(inputIndex, 0);   
  byte r1 = bitRead(inputIndex, 1);    
  byte r2 = bitRead(inputIndex, 2);
  digitalWrite(5, r0);
  digitalWrite(9, r1);
  digitalWrite(14, r2);

  this->potentiometersReadings[inputIndex] = this->potentiometersReadings[inputIndex] + 1;
  this->potentiometersTemp[inputIndex] += analogRead(22);
  
  if(this->potentiometersReadings[inputIndex] == 255){
    this->potentiometers[inputIndex] = this->potentiometersTemp[inputIndex] / 255; 
    this->potentiometers[inputIndex] = map(this->potentiometers[inputIndex], this->getAnalogMinValue(), this->getAnalogMaxValue(), 0, 1023);
    this->potentiometers[inputIndex] = constrain(this->potentiometers[inputIndex], (unsigned int)0, (unsigned int)1023);
    
    if(this->potentiometers[inputIndex] != this->potentiometersPrevious[inputIndex]){
      // Calling the potentiometer callback if there is one
      this->triggerPotentiometerChangeCallback(inputIndex, this->potentiometers[inputIndex], this->potentiometers[inputIndex] - this->potentiometersPrevious[inputIndex]);
    }
    
    this->potentiometersReadings[inputIndex] = 0;
    this->potentiometersTemp[inputIndex] = 0;
    this->potentiometersPrevious[inputIndex] = this->potentiometers[inputIndex];
  }
}

inline void Motherboard6::triggerPotentiometerChangeCallback(byte inputIndex, unsigned int value, unsigned int diff){
  if(this->inputsPotentiometerChangeCallback[inputIndex] != nullptr){
    this->inputsPotentiometerChangeCallback[inputIndex](inputIndex, value, diff);
  }
}

/**
 * Get encoder value
 * @param byte inputeIndex The index of the input
 */
inline void Motherboard6::readEncoder(byte inputIndex) {
  // Activating the right row in the matrix
  byte rowNumber = inputIndex / this->columnsNumber;

  // Setting the main multiplexer on encoders
  if (this->clockInputs < this->intervalInputs / 10) {
    for (byte i = 0; i < 3; i++) {
      if (i == rowNumber) {
        digitalWriteFast(15 + i, LOW);
      } else {
        digitalWriteFast(15 + i, HIGH);
      }
    }
    this->setMainMuxOnEncoders1();
  }

  // Getting right pin numbers on the multiplexer
  byte columnNumber = inputIndex % this->columnsNumber;
  byte muxPinA = columnNumber * 2;
  byte muxPinB = columnNumber * 2 + 1;

  // Giving time for the multiplexer to switch to Pin A
  if (this->clockInputs > this->intervalInputs / 10
      && this->clockInputs < this->intervalInputs / 6) {
    byte r0 = bitRead(muxPinA, 0);
    byte r1 = bitRead(muxPinA, 1);
    byte r2 = bitRead(muxPinA, 2);
    digitalWriteFast(5, r0);
    digitalWriteFast(9, r1);
    digitalWriteFast(14, r2);

    this->currentEncPinA = digitalReadFast(22);
  }

  // Giving time for the multiplexer to switch to Pin B
  if (this->clockInputs > this->intervalInputs / 6
      && this->clockInputs < this->intervalInputs / 2) {
    int r0 = bitRead(muxPinB, 0);
    int r1 = bitRead(muxPinB, 1);
    int r2 = bitRead(muxPinB, 2);
    digitalWriteFast(5, r0);
    digitalWriteFast(9, r1);
    digitalWriteFast(14, r2);

    this->currentEncPinB = digitalReadFast(22);
  }

  // When reading of Pin A and B is done we can interpret the result
  if (this->clockInputs > this->intervalInputs / 2
      && this->clockInputs < this->intervalInputs / 1.5) {

    byte pinstate = (this->currentEncPinB << 1) | this->currentEncPinA;
    // Determine new state from the pins and state table.
    this->encodersState[inputIndex] = this->ttable[this->encodersState[inputIndex] & 0xf][pinstate];
    // Return emit bits, ie the generated event.
    byte result = this->encodersState[inputIndex] & 0x30;

    if (result == DIR_CW) {
      this->encoders[inputIndex]--;

      // Calling the decrement callback if there is one
      this->triggerRotaryChangeCallback(inputIndex, false);
    } else if (result == DIR_CCW) {
      this->encoders[inputIndex]++;
      
      // Calling the decrement callback if there is one
      this->triggerRotaryChangeCallback(inputIndex, true);
    }

    // Setting the main multiplexer on encoder's buttons
    this->setMainMuxOnEncoders2();
    byte r0 = bitRead(columnNumber, 0);
    byte r1 = bitRead(columnNumber, 1);
    byte r2 = bitRead(columnNumber, 2);
    digitalWriteFast(5, r0);
    digitalWriteFast(9, r1);
    digitalWriteFast(14, r2);
  }

  // Giving time for the multiplexer to switch to Pin B
  if (this->clockInputs > this->intervalInputs / 1.5) {
//    this->encodersSwitch[inputIndex] = digitalReadFast(22);

    // Reading the new value
    bool newReading = digitalReadFast(22);
  
    // If there is a short or a long press callback on that input
    if(this->inputsPressDownCallback[inputIndex] != nullptr ||
       this->inputsPressUpCallback[inputIndex] != nullptr ||
       this->inputsLongPressDownCallback[inputIndex] != nullptr ||
       this->inputsLongPressUpCallback[inputIndex] != nullptr){
        
      // Inverted logic, 0 = button pushed
      // If previous value is not pushed and now is pushed
      // So if it's pushed
      if(this->encodersSwitch[inputIndex] && !newReading){ 
        // Start the counter of that input
        this->inputsPressTime[inputIndex] = 0;
        this->inputsLongPressDownFired[inputIndex] = false;

        // If there is a short press down callback on that input, and there is no Long Press down
        if(this->inputsLongPressDownCallback[inputIndex] == nullptr &&
           this->inputsPressDownCallback[inputIndex] != nullptr){
          this->inputsPressDownCallback[inputIndex](inputIndex);
        }
      }

      // If it stayed pressed for 200ms and Long Press Down callback hasn't been fired yet
      if(!this->encodersSwitch[inputIndex] && !newReading){ 
        if(this->inputsPressTime[inputIndex] >= 200 && !this->inputsLongPressDownFired[inputIndex]){
          
          if(this->inputsLongPressDownCallback[inputIndex] != nullptr){
            // Fire the callback
            this->inputsLongPressDownCallback[inputIndex](inputIndex);
            this->inputsLongPressDownFired[inputIndex] = true;
          }
        }
      }
      
      // If it's released
      if(!this->encodersSwitch[inputIndex] && newReading){ 
        // How long was it pressed
        if(this->inputsPressTime[inputIndex] < 200){
          // Short press
          
          // If there is a short press callback on that input
          if(this->inputsPressUpCallback[inputIndex] != nullptr){
            this->inputsPressUpCallback[inputIndex](inputIndex);
          }
        }else{
          // Long press
          
          // If there is a long press callback on that input
          if(this->inputsLongPressUpCallback[inputIndex] != nullptr){
            this->inputsLongPressUpCallback[inputIndex](inputIndex);
          }else if(this->inputsPressUpCallback[inputIndex] != nullptr){
            // If the input was pressed for a long time but there is only a short press callback
            // the short press callback should still be called
            this->inputsPressUpCallback[inputIndex](inputIndex);
          }
        }
      }
    }
  
    // Updating the value
    this->encodersSwitch[inputIndex] = newReading;
  }
}

inline void Motherboard6::triggerRotaryChangeCallback(byte inputIndex, bool value){
  if(this->inputsRotaryChangeCallback[inputIndex] != nullptr){
    this->inputsRotaryChangeCallback[inputIndex](false);
  }
}

/**
 * Read the Midi channel from the dipswitch
 */
inline void Motherboard6::readMidiChannel(){
  this->setMainMuxOnChannel();
  delay(50); // Only because this function is used in Init only

  byte midiChannel = 0b00000000;
  for(byte i=0; i<4; i++){
    byte r0 = bitRead(i, 0);   
    byte r1 = bitRead(i, 1);    
    byte r2 = bitRead(i, 2);
    digitalWriteFast(5, r0);
    digitalWriteFast(9, r1);
    digitalWriteFast(14, r2);
    delay(5); // Only because this function is used in Init only
    byte channelBit = !digitalRead(22);
    bitWrite(midiChannel, i, channelBit);
  }
  this->midiChannel = midiChannel + 1;
}

/**
 * Set a led status
 */
inline void Motherboard6::setLED(byte ledIndex, byte ledStatus, byte ledBrightness) {
  switch(ledStatus){
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      this->leds[ledIndex] = ledStatus;
      this->ledsBrightness[ledIndex] = ledBrightness;
    }
    break;
    default:
      this->leds[ledIndex] = 0;
    break;
  }
}



/**
 * Set all LEDs 
 */
inline void Motherboard6::setAllLED(unsigned int binary, byte ledStatus) {
  unsigned int n = binary;
  
  for (byte i = 0; i < this->ioNumber; i++) {
    if(n & 1){
      this->setLED(i, ledStatus);
    }else{
      this->setLED(i, 0);
    }
    n /= 2;
  }
}

/**
 * Toggle one LED
 */
inline void Motherboard6::toggleLED(byte index) {
  if(this->leds[index] > 0){
    this->leds[index] = 0;
  }else{
    this->leds[index] = 1;
  }
}

/**
 * Reset all LEDs
 */
inline void Motherboard6::resetAllLED() {
  for (byte i = 0; i < this->ioNumber; i++) {
    if (this->leds[i] != 4) {
      this->leds[i] = 0;
    }
  }
}

/**
 * Get input value
 * @param byte index The index of the input
 */
inline int Motherboard6::getInput(byte index){
  switch(this->inputs[index].type){
    default:
    case 0:
      // Empty
      return 0;
    break;
    case 1:
      // Button
      return !this->buttons[index];
    break;
    case 2:
      // Potentiometer
      return this->potentiometers[index];
    break;
    case 3:
      // Encoder
      // Device is not saving the encoders values, only the latest change
      int value = this->encoders[index];
      this->encoders[index] = 0; 
      return value;
    break;
  }
}

/**
 * Get encoder switch value
 * @param byte index The index of the input
 */
inline bool Motherboard6::getEncoderSwitch(byte index){
  return !this->encodersSwitch[index];
}

/**
 * Get max analog value according to resolution
 */
inline int Motherboard6::getAnalogMinValue(){
  return 0;
}

/**
 * Get max analog value according to resolution
 */
inline int Motherboard6::getAnalogMaxValue(){
  return (1 << this->analogResolution) - 1;
}

inline byte Motherboard6::getMidiChannel(){ 
  return this->midiChannel;
}

/**
 * Handle press down on a button
 */
inline void Motherboard6::setHandlePressDown(byte inputIndex, PressDownCallback fptr){
  // Press can only happen on a button and an encoder's switch
  if(this->inputs[inputIndex].type == 1 || this->inputs[inputIndex].type == 3){
    this->inputsPressDownCallback[inputIndex] = fptr;
  }
}

/**
 * Handle press up on a button
 */
inline void Motherboard6::setHandlePressUp(byte inputIndex, PressUpCallback fptr){
  // Press can only happen on a button and an encoder's switch
  if(this->inputs[inputIndex].type == 1 || this->inputs[inputIndex].type == 3){
    this->inputsPressUpCallback[inputIndex] = fptr;
  }
}

/**
 * Handle long press down on a button
 */
inline void Motherboard6::setHandleLongPressDown(byte inputIndex, LongPressDownCallback fptr){
  // Press can only happen on a button and an encoder's switch
  if(this->inputs[inputIndex].type == 1 || this->inputs[inputIndex].type == 3){
    this->inputsLongPressDownCallback[inputIndex] = fptr;
  }
}

/**
 * Handle long press up on a button
 */
inline void Motherboard6::setHandleLongPressUp(byte inputIndex, LongPressUpCallback fptr){
  // Press can only happen on a button and an encoder's switch
  if(this->inputs[inputIndex].type == 1 || this->inputs[inputIndex].type == 3){
    this->inputsLongPressUpCallback[inputIndex] = fptr;
  }
}

/**
 * Handle potentiometer
 */
inline void Motherboard6::setHandlePotentiometerChange(byte inputIndex, PotentiometerChangeCallback fptr){
  // Only for rotaries
  if(this->inputs[inputIndex].type == 2){
    this->inputsPotentiometerChangeCallback[inputIndex] = fptr;
  }
}

/**
 * Handle rotary
 */
inline void Motherboard6::setHandleRotaryChange(byte inputIndex, RotaryChangeCallback fptr){
  // Only for rotaries
  if(this->inputs[inputIndex].type == 3){
    this->inputsRotaryChangeCallback[inputIndex] = fptr;
  }
}

inline void Motherboard6::writeLED(byte index){
  if(this->ledsBrightness[index] == 255){
    digitalWriteFast(22, LOW);
  }else if(this->ledsBrightness[index] == 0){
    digitalWriteFast(22, HIGH);
  }else{
    byte reversedBrightness = map(this->ledsBrightness[index], 0, 255, 255, 0);
    analogWrite(22, reversedBrightness); 
  }
}

/**
 * Handle MIDI note on
 */
inline void Motherboard6::setHandleMidiNoteOn(MidiNoteOnCallback fptr){
  this->midiNoteOnCallback = fptr;
}

/**
 * Handle MIDI note off
 */
inline void Motherboard6::setHandleMidiNoteOff(MidiNoteOffCallback fptr){
  this->midiNoteOffCallback = fptr;
}

/**
 * Handle MIDI control change
 */
inline void Motherboard6::setHandleMidiControlChange(MidiControlChangeCallback fptr){
  this->midiControlChangeCallback = fptr;
}

inline void Motherboard6::handleMidiControlChange(byte channel, byte control, byte value){
  // Check what input the control coreesponds to
  // Trigger the callback on that input

  // Triggering the input's callbacks
  for(byte i = 0; i < Motherboard6::getInstance()->ioNumber; i++){
    // only for the inputs that matches the MIDI CC message
    if(Motherboard6::getInstance()->inputs[i].midiCC == control && Motherboard6::getInstance()->inputs[i].midiChannel == channel){
      // and triggering the callback that corresponds to the type of input
      switch(Motherboard6::getInstance()->inputs[i].type){
        case 0:
        default:
        break;

        case 1:
        {
          bool boolValue = value > 63;
          Motherboard6::getInstance()->triggerPressCallbacks(i, boolValue);
        }
        break;
        
        case 2:
          Motherboard6::getInstance()->triggerPotentiometerChangeCallback(i, map(value, 0, 127, 0, 1023), 0); // TODO: See if possible to calculate a diff here
        break;
        
        case 3:
        {
          bool boolValue = value > 63;
          Motherboard6::getInstance()->triggerRotaryChangeCallback(i, boolValue);
        }
        break;
      }
    }
    
  }

  if(Motherboard6::getInstance()->midiControlChangeCallback){
    Motherboard6::getInstance()->midiControlChangeCallback(channel, control, value);
  }
}

/**
 * Handle MIDI SysEx
 */
inline void Motherboard6::setHandleMidiSysEx(MidiSysExCallback fptr){
  this->midiSysExCallback = fptr;
}

inline void Motherboard6::handleMidiSysEx(const uint8_t* data, uint16_t length, bool last){
  Serial.println("onSysEx");

  switch(data[1]){
    // Device config request
    // Sending reply in a JSON string
    case 0:
    {
      String response = "{\"name\":\"synth\",\"midiChannel\":0,\"inputs\":[";
      for(byte i=0; i<6; i++){
        
        response += "{\"id\":\"" + 
          String(Motherboard6::getInstance()->inputs[i].id) +
          "\",\"type\":\"" +
          String(Motherboard6::getInstance()->inputs[i].type) +
          "\",\"name\":\"" +
          String(Motherboard6::getInstance()->inputs[i].name) +
          "\",\"midiCC\":\"" +
          String(Motherboard6::getInstance()->inputs[i].midiCC) +
          "\",\"midiChannel\":\"" +
          String(Motherboard6::getInstance()->inputs[i].midiChannel) +
          "\"}";
          
          if(i<5){
            response += ',';
          }
      }
      response += "]}";
      byte byteResponse[response.length()];
      response.getBytes(byteResponse, response.length()+1);
      byte byteTypedResponse[sizeof(byteResponse)+1];
      byteTypedResponse[0] = 0;
      for(unsigned int i=0; i<sizeof(byteResponse); i++){
        byteTypedResponse[i+1] = byteResponse[i];
      }
      usbMIDI.sendSysEx(sizeof(byteTypedResponse), byteTypedResponse, false);
    }
    break;

    // Device config import
    case 1:
    {
      Serial.print(data[2]); // Input Id
      Serial.print(" ");
      Serial.print(data[3]); // Input midiCC
      Serial.print(" ");
      Serial.println(data[4]); // Input midiChannel
      
      // Sending confirmation
      byte byteMessage[2] = {1,1};
      usbMIDI.sendSysEx(2, byteMessage, false);
      
      // Then save in EEPROM
    }
    break;
    
    default:
    break;
  }
  
  for(int i=0; i<length; i++){
    Serial.print(data[i]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.print("Last: ");
  Serial.println(last);
  Serial.println("");

  if(Motherboard6::getInstance()->midiSysExCallback){
    Motherboard6::getInstance()->midiSysExCallback(data, length, last);
  }
}

/**
 * Debug print
 */
inline void Motherboard6::printInputs(){
  Serial.println("Potentiometers:");
  for(byte j = 0; j < this->ioNumber; j++){
    Serial.print(this->potentiometers[j]);
    Serial.print(" ");
  }
  Serial.println("");
  
  Serial.println("Encoders:");
  for(byte j = 0; j < this->ioNumber; j++){
    Serial.print(this->encoders[j]);
    Serial.print(" ");
  }
  Serial.println("");

  Serial.println("Encoder switches:");
  for(byte j = 0; j < this->ioNumber; j++){
    Serial.print(this->encodersSwitch[j]);
    Serial.print(" ");
  }
  Serial.println("");

  Serial.println("Buttons:");
  for(byte j = 0; j < this->ioNumber; j++){
    Serial.print(this->buttons[j]);
    Serial.print(" ");
  }
  Serial.println("");
  
  Serial.println("Midi Channel:");
  Serial.print(this->midiChannel);
  Serial.println("");
  
  Serial.println("");
}

/**
 * Debug print
 */
inline void Motherboard6::printLeds(){
  Serial.println("Leds:");
  for(byte j = 0; j < this->ioNumber; j++){
    Serial.print(this->leds[j]);
    Serial.print(" ");
  }
  Serial.println("");
}
#endif
