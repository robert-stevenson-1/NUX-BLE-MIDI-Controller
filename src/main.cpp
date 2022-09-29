/*
FILE NAME:    main.cpp (Version 0.9)
AUTHOR:       Robert Stevenson
DATE CREATED: 22/09/2022
DESCRIPTION:  The main program source code where all the program logic and operations take place for
              communicating with the NUX BT Amp/Device by taking input from the controller and sending
              the relevant commands to the device.

CHANGES:      - Version 0.9 (29/09/2022): 
                  - Finished and Uploaded to Github. This version is essentially function 
                    and works, however project is missing essential implementation documentation/instructions
                    and need some final finishes and polish on the code.
                  - No longer using the Adafruit SSD1306 Library for the Display
*/

// Installed Library includes (Libraries installed via PlatformIO interface)
#include <Arduino.h>
#include <SSD1306Wire.h>
#include <Bounce2.h>
#include <BLEMidi_Transport.h>
#include <hardware/BLEMIDI_Client_ESP32.h>
// Own Library/File inclusions
#include "NUX_MIDI_CC.h"
#include "NUX_CONFIG.h"

// ===Misc Definitions===
#define POT_MAX_VALUE 4080    // Maximum cutoff reading from a Potentiometer source. Anything higher will be ignored. 
#define POT_MIN_VALUE 20      // Minimum cutoff reading from a Potentiometer source. Anything lower will be ignored.
#define DEBOUNCE_TIME 25      // Button debounce time in Milliseconds (ms).
#define FILTER_LEN 10         // Max number of stored readings taken when getting an average ADC reading.
#define SCREEN_ADDRESS 0x3C   // I2C address for the Display.

// ===Input/Output Pin Definitions===
#define LED_STATUS 2
#define BTN_GATE_PIN 15
#define BTN_EFX_PIN 4
#define BTN_MOD_PIN 5
#define BTN_DELAY_PIN 18
#define BTN_REVERB_PIN 13
#define BTN_DRUMS_PIN 19
#define BTN_PRESET_DOWN_PIN 12
#define BTN_PRESET_UP_PIN 23
#define BTN_OTHER_PIN 34 // This pin does not work as a button input (Working on alternative solution)

#define POT_MAST_VOL_PIN 32
#define POT_GAIN_PIN 33
#define POT_BASS_PIN 25
#define POT_MID_PIN 26
#define POT_TREBLE_PIN 14
#define POT_PRESENCE_PIN 27

// ===Breadboard Setup===
// #define LED_STATUS 13
// #define BTN_GATE_PIN 26
// #define BTN_EFX_PIN 18
// #define BTN_MOD_PIN 19
// #define BTN_DRUMS_PIN 23
// #define BTN_PRESET_UP_PIN 2
// #define BTN_PRESET_DOWN_PIN 15
// #define BTN_DELAY_PIN 21
// #define BTN_REVERB_PIN 14

// #define POT_MAST_VOL_PIN 25
// #define POT_GAIN_PIN 34
// #define POT_BASS_PIN 35
// #define POT_MID_PIN 32
// #define POT_TREBLE_PIN 33
// #define POT_PRESENCE_PIN 26
// ======================

// Initialise the Button Objects with the pin numbers that they will read the button states from.
Bounce2::Button btnGate = Bounce2::Button();
Bounce2::Button btnEFX = Bounce2::Button();
Bounce2::Button btnMod = Bounce2::Button();
Bounce2::Button btnDelay = Bounce2::Button();
Bounce2::Button btnReverb = Bounce2::Button();
Bounce2::Button btnDrums = Bounce2::Button();
Bounce2::Button btnPresetUp = Bounce2::Button();
Bounce2::Button btnPresetDown = Bounce2::Button();
Bounce2::Button btnOther = Bounce2::Button();

// Initialise the I2C Display
SSD1306Wire display(SCREEN_ADDRESS, SDA, SCL);

// Program control variables that determine the flow
bool initLoop = true;
bool connBT = false;
// Toggle values to detect when a footswitch has been pressed
bool drumsToggle = false;
bool gateToggle = false;
bool efxToggle = false;
bool modToggle = false;
bool delayToggle = false;
bool reverbToggle = false;
int8_t current_preset = 0;

// Amp EQ Values storage (Plus help data for reading averages to avoid fluctuant values from being read)
int8_t curMasterVolValue = 0;                // 0 - 100 range after the pot value is mapped (Master Volume)
int8_t oldMasterVolValue = 0;                // 0 - 100 range after the pot value is mapped (old Master Volume)
uint16_t masterVol_Buffer[FILTER_LEN] = {0}; // History Buffer to get accurate ADC readings and conversions (Master Volume)
int8_t masterVol_buf_i = 0;                  // Buffer index (Master Volume)

int8_t curGainValue = 0;                     // 0 - 100 range after the pot value is mapped (Gain)
int8_t oldGainValue = 0;                     // 0 - 100 range after the pot value is mapped (old Gain)
uint16_t gain_Buffer[FILTER_LEN] = {0};      // History Buffer to get accurate ADC readings and conversions (Gain)
int8_t gain_buf_i = 0;                       // Buffer index (Gain)

int8_t curBassValue = 0;                     // 0 - 100 range after the pot value is mapped (Bass)
int8_t oldBassValue = 0;                     // 0 - 100 range after the pot value is mapped (old Bass)
uint16_t bass_Buffer[FILTER_LEN] = {0};      // History Buffer to get accurate ADC readings and conversions (Bass)
int8_t bass_buf_i = 0;                       // Buffer index (Bass)

int8_t curMidValue = 0;                      // 0 - 100 range after the pot value is mapped (Mids)
int8_t oldMidValue = 0;                      // 0 - 100 range after the pot value is mapped (old Mids)
uint16_t mid_Buffer[FILTER_LEN] = {0};       // History Buffer to get accurate ADC readings and conversions (Gain)
int8_t mid_buf_i = 0;                        // Buffer index (Gain)

int8_t curTrebleValue = 0;                   // 0 - 100 range after the pot value is mapped (Treble)
int8_t oldTrebleValue = 0;                   // 0 - 100 range after the pot value is mapped (old Treble)
uint16_t treble_Buffer[FILTER_LEN] = {0};    // History Buffer to get accurate ADC readings and conversions (Gain)
int8_t treble_buf_i = 0;                     // Buffer index (Gain)

// Connect to the AMP via bluetooth with BT NAME
//BLEMIDI_CREATE_INSTANCE(AMP_BT_NAME, MIDI);
// Connect to the AMP via bluetooth with BT ADDR
BLEMIDI_CREATE_INSTANCE(AMP_BT_ADDR, MIDI);

// ===Function Definitions===
void displayInfo();                                                      // Display Info to the SSD1306
void readPots();                                                         // Read all the Pot Data
uint16_t readADCAvg(uint8_t pin, uint16_t *buffer, int8_t *bufferIndex); // Read and return the Average results from a Analog pin source
void syncDeviceDataChanges(byte ControlNumber, byte ControlValue);       // When receiving a control value change from the amp, sync the values to those in the MIDI Controller

void presetUp();     // Move up a preset
void presetDown();   // Move down a preset
void toggleDrums();  // Toggle the drum machine
void toggleGate();   // Toggle the Noise Gate
void toggleEFX();    // Toggle the Effects Pedal
void toggleMod();    // Toggle the Modulation Effects Pedal
void toggleDelay();  // Toggle the Delay Effects Pedal
void toggleReverb(); // Toggle the Reverb Effects Pedal
void toggleOther();  // Spare Button

void changeAmpMasterVol(int8_t value); // Change the Amp Master Volume Settings
void changeAmpGain(int8_t value);      // Change the Amp Gain Settings
void changeAmpBass(int8_t value);      // Change the Amp Bass Settings
void changeAmpMid(int8_t value);       // Change the Amp Mid Settings
void changeAmpTreble(int8_t value);    // Change the Amp Treble Settings

// Currently this function is unused as no smoothing of values is need
//float valueFilter(float _alpha, float _oldValue, float _newValue); // return the Filtered Result

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  adcAttachPin(POT_MAST_VOL_PIN);
  adcAttachPin(POT_GAIN_PIN);
  adcAttachPin(POT_BASS_PIN);
  adcAttachPin(POT_MID_PIN);
  adcAttachPin(POT_TREBLE_PIN);

  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);

  if (!display.init())
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Setup display
  display.clear();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,0,"Searching...");
  display.display();

  MIDI.begin(MIDI_CHANNEL_OMNI);

  BLEMIDI.setHandleConnected([]()
                              {
                              Serial.println("---------CONNECTED---------");
                              digitalWrite(LED_STATUS, HIGH);
                              displayInfo();
                              connBT = true;
                              initLoop = true;
                              // Set the toggle values to there defaults (Technically not necessary, but added in hope that 
                              // in the future if I can read the preset data then I can set the appropriate toggle variables)
                              drumsToggle = false;
                              gateToggle = false;
                              efxToggle = false;
                              modToggle = false;
                              delayToggle = false;
                              reverbToggle = false; 
                              });
  BLEMIDI.setHandleDisconnected([]()
                                {
                              Serial.println("---------NOT CONNECTED---------");
                              digitalWrite(LED_STATUS, LOW);
                              connBT = false;
                              initLoop = true; 
                              display.clear();
                              display.drawString(0,0, "Lost Connection");
                              display.drawString(0,10, "Rescanning...");
                              display.display();
                              });
  MIDI.setHandleControlChange([](byte channel, byte ControlNumber, byte ControlValue)
                              {
                              Serial.print("ControlChange: CH: ");
                              Serial.print(channel);
                              Serial.print(" | ");
                              Serial.print(ControlNumber);
                              Serial.print(", ");
                              Serial.println(ControlValue);
                              syncDeviceDataChanges(ControlNumber, ControlValue); 
                              });

// Setup the buttons by attaching pins and setting debounce times
  btnGate.attach(BTN_GATE_PIN, INPUT_PULLUP);
  btnGate.interval(DEBOUNCE_TIME);
  btnGate.setPressedState(LOW);

  btnEFX.attach(BTN_EFX_PIN, INPUT_PULLUP);
  btnEFX.interval(DEBOUNCE_TIME);
  btnEFX.setPressedState(LOW);
  
  btnMod.attach(BTN_MOD_PIN, INPUT_PULLUP);
  btnMod.interval(DEBOUNCE_TIME);
  btnMod.setPressedState(LOW);
  
  btnDelay.attach(BTN_DELAY_PIN, INPUT_PULLUP);
  btnDelay.interval(DEBOUNCE_TIME);
  btnDelay.setPressedState(LOW);
  
  btnReverb.attach(BTN_REVERB_PIN, INPUT_PULLUP);
  btnReverb.interval(DEBOUNCE_TIME);
  btnReverb.setPressedState(LOW);
  
  btnDrums.attach(BTN_DRUMS_PIN, INPUT_PULLUP);
  btnDrums.interval(DEBOUNCE_TIME);
  btnDrums.setPressedState(LOW);
  
  btnPresetUp.attach(BTN_PRESET_UP_PIN, INPUT_PULLUP);
  btnPresetUp.interval(DEBOUNCE_TIME);
  btnPresetUp.setPressedState(LOW);
  
  btnPresetDown.attach(BTN_PRESET_DOWN_PIN, INPUT_PULLUP);
  btnPresetDown.interval(DEBOUNCE_TIME);
  btnPresetDown.setPressedState(LOW);
  
  btnOther.attach(BTN_OTHER_PIN, INPUT_PULLUP);
  btnOther.interval(DEBOUNCE_TIME);
  btnOther.setPressedState(LOW);

}

void loop()
{
  MIDI.read();
  if (connBT)
  {
    if (initLoop)
    {
      initLoop = false;
      MIDI.sendControlChange(CC_PRESET, 0, 1);
    }

    // check button inputs
    btnGate.update();
    if (btnGate.pressed())
      toggleGate();

    btnEFX.update();
    if (btnEFX.pressed())
      toggleEFX();

    btnMod.update();
    if (btnMod.pressed())
      toggleMod();

    btnDelay.update();
    if (btnDelay.pressed())
      toggleDelay();

    btnReverb.update();
    if (btnReverb.pressed())
      toggleReverb();

    btnDrums.update();
    if (btnDrums.pressed())
      toggleDrums();

    btnPresetUp.update();
    if (btnPresetUp.pressed())
      presetUp();

    btnPresetDown.update();
    if (btnPresetDown.pressed())
      presetDown();

    // btnOther.loop();
    // if(btnOther.isPressed()) ;

    // Read and check for Pot value updates
    readPots();
  }
}

void displayInfo()
{
  display.clear();
  display.drawString(0,0, "Connected");
  display.drawString(0,8, "Preset: " + String(current_preset + 1));
  display.drawString(0,16, "Master Vol: " + String(curMasterVolValue));
  display.drawString(0,24, "Gain: " + String(curGainValue));
  display.drawString(0,32, "Bass: " +String(curBassValue));
  display.drawString(0,40, "Mid: " + String(curMidValue));
  display.drawString(0,48, "Treble: " + String(curTrebleValue));
  display.display();
}

uint16_t readADCAvg(uint8_t pin, uint16_t *buffer, int8_t *bufferIndex)
{
  uint16_t Sum = 0;
  int16_t data_Raw = analogRead(pin);

  buffer[(*bufferIndex)++] = data_Raw;
  if ((*bufferIndex) == FILTER_LEN)
  {
    (*bufferIndex) = 0;
  }

  for (int i = 0; i < FILTER_LEN; i++)
  {
    Sum += buffer[i];
  }

  return (Sum / FILTER_LEN);
}

void readPots()
{
  changeAmpMasterVol(map(readADCAvg(POT_MAST_VOL_PIN, masterVol_Buffer, &masterVol_buf_i), POT_MIN_VALUE, POT_MAX_VALUE, 0, 127));
  changeAmpGain(map(readADCAvg(POT_GAIN_PIN, gain_Buffer, &gain_buf_i), POT_MIN_VALUE, POT_MAX_VALUE, 0, 127));
  changeAmpBass(map(readADCAvg(POT_BASS_PIN, bass_Buffer, &bass_buf_i), POT_MIN_VALUE, POT_MAX_VALUE, 0, 127));
  changeAmpMid(map(readADCAvg(POT_MID_PIN, mid_Buffer, &mid_buf_i), POT_MIN_VALUE, POT_MAX_VALUE, 0, 127));
  changeAmpTreble(map(readADCAvg(POT_TREBLE_PIN, treble_Buffer, &treble_buf_i), POT_MIN_VALUE, POT_MAX_VALUE, 0, 127));
}

void syncDeviceDataChanges(byte ControlNumber, byte ControlValue)
{
  switch ((int)ControlNumber)
  {
  case CC_PRESET:
    current_preset = ControlValue;
    break;

    // TODO: POPULATE TO OTHER CC COMMAND SYNCING

  default:
    break;
  }

  displayInfo();
}

void presetUp()
{
  //Serial.println("BTN preset UP Pressed"); // Debug Message(s)

  current_preset++;
  if (current_preset > MAX_PRESET_NUM - 1)
  {
    current_preset = 0;
  }
  //Serial.print("Current Preset: "); // Debug Message(s)
  //Serial.println(current_preset);   // Debug Message(s)

  MIDI.sendControlChange(CC_PRESET, current_preset, 1);
  displayInfo();
}

void presetDown()
{
  //Serial.println("BTN preset DOWN Pressed"); // Debug Message(s)

  current_preset--;
  if (current_preset < 0)
  {
    current_preset = MAX_PRESET_NUM - 1;
  }
  //Serial.print("Current Preset: "); // Debug Message(s)
  //Serial.println(current_preset);   // Debug Message(s)

  MIDI.sendControlChange(CC_PRESET, current_preset, 1);
  displayInfo();
}

void toggleDrums()
{
  //Serial.println("Drums Toggled"); // Debug Message(s)
  if (drumsToggle)
  {
    drumsToggle = false;
    MIDI.sendControlChange(CC_DRUMS_ENABLE, 0, 1);
  }
  else
  {
    drumsToggle = true;
    MIDI.sendControlChange(CC_DRUMS_ENABLE, 1, 1);
  }
  displayInfo();
}

void toggleGate()
{
  //Serial.println("GATE Toggled"); // Debug Message(s)
  if (gateToggle)
  {
    gateToggle = false;
    MIDI.sendControlChange(CC_GAIN_ENABLE, 0, 1);
  }
  else
  {
    gateToggle = true;
    MIDI.sendControlChange(CC_GAIN_ENABLE, 1, 1);
  }
  displayInfo();
}

void toggleEFX()
{
  //Serial.println("EFX Toggled"); // Debug Message(s)
  if (efxToggle)
  {
    efxToggle = false;
    MIDI.sendControlChange(CC_EFX_ENABLE, 0, 1);
  }
  else
  {
    efxToggle = true;
    MIDI.sendControlChange(CC_EFX_ENABLE, 1, 1);
  }
  displayInfo();
}

void toggleMod()
{
  //Serial.println("MOD Toggled"); // Debug Message(s)
  if (modToggle)
  {
    modToggle = false;
    MIDI.sendControlChange(CC_MOD_ENABLE, 0, 1);
  }
  else
  {
    modToggle = true;
    MIDI.sendControlChange(CC_MOD_ENABLE, 1, 1);
  }
  displayInfo();
}

void toggleDelay()
{
  //Serial.println("DELAY Toggled"); // Debug Message(s)
  if (delayToggle)
  {
    delayToggle = false;
    MIDI.sendControlChange(CC_DELAY_ENABLE, 0, 1);
  }
  else
  {
    delayToggle = true;
    MIDI.sendControlChange(CC_DELAY_ENABLE, 1, 1);
  }
  displayInfo();
}

void toggleReverb()
{
  //Serial.println("REVERB Toggled"); // Debug Message(s)
  if (reverbToggle)
  {
    reverbToggle = false;
    MIDI.sendControlChange(CC_REVERB_ENABLE, 0, 1);
  }
  else
  {
    reverbToggle = true;
    MIDI.sendControlChange(CC_REVERB_ENABLE, 1, 1);
  }
  displayInfo();
}

void changeAmpMasterVol(int8_t value)
{
  curMasterVolValue = value;
  if (oldMasterVolValue != curMasterVolValue)
  {
    oldMasterVolValue = curMasterVolValue;
    MIDI.sendControlChange(CC_AMP_MASTER_VOL, curMasterVolValue, 1);
    displayInfo();
  }
}

void changeAmpGain(int8_t value)
{
  curGainValue = value;
  if (oldGainValue != curGainValue)
  {
    oldGainValue = curGainValue;
    MIDI.sendControlChange(CC_AMP_GAIN, curGainValue, 1);
    displayInfo();
  }
}

void changeAmpBass(int8_t value)
{
  curBassValue = value;
  if (oldBassValue != curBassValue)
  {
    oldBassValue = curBassValue;
    MIDI.sendControlChange(CC_AMP_BASS, curBassValue, 1);
    displayInfo();
  }
}

void changeAmpMid(int8_t value)
{
  curMidValue = value;
  if (oldMidValue != curMidValue)
  {
    oldMidValue = curMidValue;
    MIDI.sendControlChange(CC_AMP_MID, curMidValue, 1);
    displayInfo();
  }
}

void changeAmpTreble(int8_t value)
{
  curTrebleValue = value;
  if (oldTrebleValue != curTrebleValue)
  {
    oldTrebleValue = curTrebleValue;
    MIDI.sendControlChange(CC_AMP_TREBLE, curTrebleValue, 1);
    displayInfo();
  }
}

// Currently this function is unused as no smoothing of values is need
// float valueFilter(float _alpha, float _oldValue, float _newValue)
// {
//   return (_alpha * _newValue) + ((1 - _alpha) * _oldValue);
// }