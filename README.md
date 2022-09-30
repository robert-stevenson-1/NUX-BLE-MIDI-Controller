# NUX BLE MIDI Controller (Version 0.9)
## Important this Project is W.I.P.  
This project focuses on creating a BLE MIDI Floor controller built using an ESP32 and C++ with the Arduino framework to control the NUX Mighty Amp Line up.
(The Mighty Air was used for this project, so as a result config header files would need changing and depending your device model the CC message number would need changing too).

## Hardware:
TODO: Populate Hardware list
- 1 x ESP32 (I used an ESP32 devkit v1 development board, other varients may work) (The one with 30 GPIO Pins broken out)
- 6 x 10K Ohm Potentiometers (Only 5 are functional, the 6th, "Presence", pot will be implemented in V1.0
- 9 x Monentary Footswitches
- 1 x SSD1306 128x64 Pixel OLED Display Module
- Wire (Preferably a range of Colours, as it will help)
- Encloser (A box with a reasonable amount of room inside that you can fit all the wiring and mount the Pots and Switches to will work)

# Project instructions
TODO: Instructions on how to build the project

## Steps
TODO: Create instruction steps

# Future Additions/Changes
- Code Refactoring 
  - Aim to reduce the program size in flash
  - Refractor with an OOP approach
- Unlock ability to communication the devices more to get access to more device data (eg. Preset Data, Battery info, etc...)
- Change screen for a larger and/or higher resolution option
- Design a modular, custom, PCB (w/ Removable ESP32)
- Design 3D Prinable Enclosure 
- Update UI

## Low Priority Changes 
- Design Custom PCB (Custom ESP32 Microcontroller with specific IO connectors)
- Explore alternative microcontroller(s) 

# Disclaimer and Questions
- This project was developed using the PlatformIO Extension for VSCode, as a result I have not converted or check compatibility with the Arduino IDE (This will be address at some point in the future)
- I am currently working on this in my free time, so whilst I will be working on this project when I can it means that updates will not be as frequent as I would like them to be.
- Instructions and guide to make you own controller will come in time for those how are less familiar with Electronics and Microcontroller programming and don't want to work out via the code how to build it.

# Libraries
The Libraries used in the project:
- SSD1306 Display: https://github.com/ThingPulse/esp8266-oled-ssd1306
- BLE & MIDI: https://github.com/lathoub/Arduino-BLE-MIDI
- Button (debounce): https://github.com/thomasfredericks/Bounce2

# References
A List of Document, forum post, project, etc... that is I've used to help make the project
-  MIDI CC Messages source: https://docs.google.com/document/d/1g6yunKcy2s1OGOkwraDXkDEBKd4hu8FxxlSFoJo2eE0/edit (PDF Copy included in project in case this disappears)
