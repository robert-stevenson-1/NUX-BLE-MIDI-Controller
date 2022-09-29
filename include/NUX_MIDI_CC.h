// FILE NAME:    NUX_MIDI_CC.h
// AUTHOR:       Robert Stevenson
// DATE CREATED: 26/09/2022
// DESCRIPTION:  A config header file that contains the MIDI CC values to control the NUX
//               bluetooth devices (Preloaded with the values and CC used for controlling the 
//               the NUX Mighty Air with the default controller program setup)
//               Commands sources from the file in "Dev Docs\NUX CC Messages.pdf" 
//               (Original Source linked in pdf, author Unkown at time of writing, please contract me if you know
//               so I can give proper credit) 

// Preset Control related CC Message IDs
#define CC_PRESET 49

// Noise Gate related CC Message IDs
#define CC_GAIN_ENABLE 22

// Effects Pedal related CC Message IDs
#define CC_EFX_ENABLE 84

// Modulation Pedal related CC Message IDs
#define CC_MOD_ENABLE 56

// Delay Pedal related CC Message IDs
#define CC_DELAY_ENABLE 28

// Reverb Pedal related CC Message IDs
#define CC_REVERB_ENABLE 36

// Amplifier related CC Message IDs
#define CC_AMP_MASTER_VOL 81
#define CC_AMP_GAIN 79
#define CC_AMP_BASS 13
#define CC_AMP_MID 14
#define CC_AMP_TREBLE 15
#define CC_AMP_PRESENCE 80

// Drum Machine related CC Message IDs
#define CC_DRUMS_ENABLE 122