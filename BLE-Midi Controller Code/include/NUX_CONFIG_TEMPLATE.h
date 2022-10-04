// FILE NAME:    NUX_CONFIG_TEMPLATE.h
// AUTHOR:       Robert Stevenson
// DATE CREATED: 29/09/2022
// DESCRIPTION:  A config header file that contains the configuration information of the NUX bluetooth
//               devices for connect the MIDI controller to the device and the setting amp parameter

// IMPORTANT READ ME:
//   CREATE A COPY THIS HEADER FILE, REMOVE '_TEMPLATE' FROM THE FILE NAME
//   AND EDIT IT TO CONTAIN THE RELEVANT INFOMATION OF YOUR NUX BT DEVICE
//   TO USE WITH THE MIDI PROGRAM.

// Bluetooth Config
#define AMP_BT_NAME "NUX MIGHTY AIR MIDI" // Change with your NUX Devices BT Device Name
#define AMP_BT_ADDR "xx:xx:xx:xx:xx:xx"   // Replace with you NUX Devices BT Address

// Amp Settings Config
#define MAX_PRESET_NUM 7 // Define the max number of presets that your amp has which it can cycle through