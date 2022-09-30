# NUX BLE MIDI Controller (Version 1.0) (Dev)
## Important this Project is W.I.P.  
This project focuses on creating a BLE MIDI Floor controller built using an ESP32 and C++ with the Arduino framework to control the NUX Mighty Amp Line up.
(The Mighty Air was used for this project, so as a result config header files would need changing and depending your device model the CC message number would need changing too).

# Project instructions
TODO: Instructions on how to build the project

## Hardware:
TODO: Create list of Hardware

## Steps
TODO: Create instruction steps

# Future Additions/Changes
## Short Term
- Code Refactoring (with the aim to reduce the program size in flash)
- Code Refactoring (Refractor with an OOP approach)
- Design Custom PCB (w/ Removable ESP32)
- Design Enclosure 

## Long Term
- Design Custom PCB (Custom ESP32 Microcontroller with specific IO connectors)
- Change screen for a larger and/or higher resolution option
- Update UI

## Low Priority Changes 
- Look at alternative microcontroller(s) 

# Disclaimer and Questions
- This project was developed using the PlatformIO Extension for VSCode, as a result I have not converted or check compatibility with the Arduino IDE (This will be address at some point in the future)
- I am currently working on this in my free time, so whilst I will be working on this project when I can it means that updates will not be as frequent as I would like them to be.
- Instructions and guide to make you own project will come in time for those how are less familiar with Electronics and Microcontroller programming

# Libraries
The Libraries used in the project:
- SSD1306 Display: https://github.com/ThingPulse/esp8266-oled-ssd1306
- BLE & MIDI: https://github.com/lathoub/Arduino-BLE-MIDI
- Button (debounce): https://github.com/thomasfredericks/Bounce2
