
#include <DaisyDuino.h>

#include "utility/gpio.h"

#include <seesaw_neopixel.h>

DaisyHardware hw;
#define MAX_SIZE (48000 * 60 * 5)  // 5 minutes of floats at 48 khz
Metro tick;
size_t num_channels;

HardwareSerial hwSerial(1, 2);  // Arduino Nano handles PN532 reads and reports results over serial


#define PIN 0
// ATtiny1616 drives neopixels, talks to Seed over i2c
seesaw_NeoPixel strip = seesaw_NeoPixel(36, PIN, NEO_GRB + NEO_KHZ800);



#define CALIBRATIONMODE 0


// temporary joystick calibration constants
#define HALFSTEPZEROPOINT 545
#define HALFSTEPLOWPOINT 670
#define HALFSTEPHIGHPOINT 410

#define OCTAVEZEROPOINT 475
#define OCTAVELOWPOINT 600
#define OCTAVEHIGHPOINT 340




#define NUMCHORDS 8
#define NUMVOICESPERCHORD 3
#define NUMZONES 9
#define NUMMAINVOICES 24
#define NUMARPVOICES 8



float mainVoiceLevelModifier[NUMMAINVOICES];
float arpVoiceLevelModifier[NUMARPVOICES];




// MAIN VOICES
static Oscillator osc0;
static Oscillator osc1;
static Oscillator osc2;
static Oscillator osc3;

static Oscillator osc4;
static Oscillator osc5;
static Oscillator osc6;
static Oscillator osc7;

static Oscillator osc8;
static Oscillator osc9;
static Oscillator osc10;
static Oscillator osc11;

static Oscillator osc12;
static Oscillator osc13;
static Oscillator osc14;
static Oscillator osc15;

static Oscillator osc16;
static Oscillator osc17;
static Oscillator osc18;
static Oscillator osc19;

static Oscillator osc20;
static Oscillator osc21;
static Oscillator osc22;
static Oscillator osc23;


Oscillator *mainVoiceOscillator[NUMMAINVOICES] = {
  &osc0,
  &osc1,
  &osc2,
  &osc3,
  &osc4,
  &osc5,
  &osc6,
  &osc7,
  &osc8,
  &osc9,
  &osc10,
  &osc11,
  &osc12,
  &osc13,
  &osc14,
  &osc15,
  &osc16,
  &osc17,
  &osc18,
  &osc19,
  &osc20,
  &osc21,
  &osc22,
  &osc23,
};



//ARP VOICES
static Oscillator osc32;
static Oscillator osc33;
static Oscillator osc34;
static Oscillator osc35;

static Oscillator osc36;
static Oscillator osc37;
static Oscillator osc38;
static Oscillator osc39;

Oscillator *arpVoiceOscillator[NUMARPVOICES] = {
  &osc32,
  &osc33,
  &osc34,
  &osc35,
  &osc36,
  &osc37,
  &osc38,
  &osc39,
};



//AA VOICES
static Oscillator osc41;
static Oscillator osc42;
static Oscillator osc43;


Oscillator *aaVoiceOscillator[3] = {
  &osc41,
  &osc42,
  &osc43,
};




// DRUM VOICES
// kick
static Oscillator osc44;
static Oscillator osc45;
static Oscillator osc46;

Oscillator *kickOscillator[3] = {
  &osc44,
  &osc45,
  &osc46,
};


// snare
static WhiteNoise snareNoise;


// hihat
static WhiteNoise hihatNoise;





// MAIN VOICE ENVELOPES
static Adsr env0;
static Adsr env1;
static Adsr env2;
static Adsr env3;
static Adsr env4;
static Adsr env5;
static Adsr env6;
static Adsr env7;
static Adsr env8;
static Adsr env9;
static Adsr env10;
static Adsr env11;
static Adsr env12;
static Adsr env13;
static Adsr env14;
static Adsr env15;
static Adsr env16;
static Adsr env17;
static Adsr env18;
static Adsr env19;
static Adsr env20;
static Adsr env21;
static Adsr env22;
static Adsr env23;
static Adsr env24;
static Adsr env25;
static Adsr env26;
static Adsr env27;
static Adsr env28;
static Adsr env29;
static Adsr env30;
static Adsr env31;
static Adsr env32;




Adsr *mainVoiceEnvelope[33] = {
  &env0,
  &env1,
  &env2,
  &env3,
  &env4,
  &env5,
  &env6,
  &env7,
  &env8,
  &env9,
  &env10,
  &env11,
  &env12,
  &env13,
  &env14,
  &env15,
  &env16,
  &env17,
  &env18,
  &env19,
  &env20,
  &env21,
  &env22,
  &env23,
  &env24,
  &env25,
  &env26,
  &env27,
  &env28,
  &env29,
  &env30,
  &env31,
  &env32,
};


bool mainVoiceGate[NUMMAINVOICES];



//ARP ENVELOPE
static Adsr env33;
static Adsr env34;
static Adsr env35;
static Adsr env36;
static Adsr env37;
static Adsr env38;
static Adsr env39;
static Adsr env40;

Adsr *arpVoiceEnvelope[NUMARPVOICES] = {
  &env33,
  &env34,
  &env35,
  &env36,
  &env37,
  &env38,
  &env39,
  &env40,
};

bool arpVoiceGate[NUMARPVOICES];



//AA ENVELOPE
static Adsr env41;
static Adsr env42;
static Adsr env43;


Adsr *aaVoiceEnvelope[3] = {
  &env33,
  &env34,
  &env35,
};

bool aaVoiceGate[3];




// DRUM ENVELOPE
static Adsr kickEnv;
bool kickGate = 0;

static Adsr snareEnv;
bool snareGate = 0;

static Adsr hihatEnv;
bool hihatGate = 0;





// MAIN VOICE FILTER
static MoogLadder filter0;
static MoogLadder filter1;
static MoogLadder filter2;
static MoogLadder filter3;
static MoogLadder filter4;
static MoogLadder filter5;
static MoogLadder filter6;
static MoogLadder filter7;



MoogLadder *mainVoiceFilter[NUMCHORDS] = {
  &filter0,
  &filter1,
  &filter2,
  &filter3,
  &filter4,
  &filter5,
  &filter6,
  &filter7,
};


// ARP FILTER
Svf arpVoiceFilter;


// AA FILTER
static MoogLadder filter41;
static MoogLadder filter42;
static MoogLadder filter43;


MoogLadder *aaVoiceFilter[3] = {
  &filter41,
  &filter42,
  &filter43,
};


// DRUM FILTER
Svf snareHighPass;
Svf snareLowPass;
Svf hihatHighPass;
Svf hihatLowPass;


// TREMOLO
static Tremolo tremolo0;
static Tremolo tremolo1;
static Tremolo tremolo2;
static Tremolo tremolo3;
static Tremolo tremolo4;
static Tremolo tremolo5;
static Tremolo tremolo6;
static Tremolo tremolo7;

Tremolo *mainVoiceTremolo[NUMCHORDS] = {
  &tremolo0,
  &tremolo1,
  &tremolo2,
  &tremolo3,
  &tremolo4,
  &tremolo5,
  &tremolo6,
  &tremolo7,
};



// BITCRUSH
static Decimator decimator;





//Cmaj
int chordNote[8][4] = {
  { 36, 52, 43, 59 },
  { 38, 53, 45, 60 },
  { 40, 55, 47, 62 },
  { 41, 57, 48, 64 },
  { 43, 59, 50, 65 },
  { 45, 60, 52, 67 },
  { 47, 62, 53, 69 },
  { 48, 64, 55, 71 },
};



int arpNote[8][12] = {
  { 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62 },
  { 38, 41, 43, 45, 48, 50, 53, 55, 57, 60, 62, 65 },
  { 40, 43, 45, 47, 50, 52, 55, 57, 59, 62, 64, 67 },
  { 41, 43, 45, 47, 50, 53, 55, 57, 60, 62, 65, 67 },
  { 43, 45, 47, 50, 52, 55, 57, 59, 62, 64, 67, 69 },
  { 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72 },
  { 47, 50, 52, 53, 57, 59, 62, 64, 65, 69, 71, 74 },
  { 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74 },
};



int orderedNeopixel[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 30, 31, 8, 9, 10, 11, 24, 25, 26, 27, 12, 13, 14, 15, 20, 21, 22, 23, 16, 17, 18, 19 };

int zoneSensorPin[9] = { A0, A1, A2, A3, A4, A5, A6, A7, A8 };

int lastSensorReading[9] = {};
int sensorReading[9] = {};

int zoneReading[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int lastZoneReading[9] = {};


// divided by 30
int colorBase[8][3] = {
  { 0, 8, 0 },
  { 3, 8, 0 },
  { 8, 8, 0 },
  { 8, 0, 0 },
  { 6, 0, 8 },
  { 0, 2, 8 },
  { 0, 8, 8 },
  { 8, 8, 8 },
};

int buttonPin = D26;
int ledPin = D27;
int irRxPin = D7;
int irTxPin = D14;
int hallPin = D3;
int octaveJoystickSelectPin = D6;

bool lastArpActiveState = false;
bool arpActiveState = false;

bool irRxState = false;
bool lastIrRxState = false;
bool irTxState = false;
bool lastIrTxState = false;

bool hallState = HIGH;
bool lastHallState = HIGH;

bool octaveJoystickSelectState = HIGH;
bool lastOctaveJoystickSelectState = HIGH;
bool octaveLocked = false;
bool lastOctaveLocked = false;
bool octaveLockDirection = LOW;

int octavePixelMultiplier = 0;
int lastOctavePixelMultiplier = 0;

bool buttonState = LOW;
bool lastButtonState = LOW;
unsigned long lastButtonPressTime = 0;
int resetPressDuration = 5000;

int activeThreshold = 30;

float octaveMultiplier = 1.0;

bool chordActive[NUMCHORDS] = { false, false, false, false, false, false, false, false };
bool lastChordActive[NUMCHORDS] = { false, false, false, false, false, false, false, false };

int lastNumActive = 1;  // initialize as 1 so that we check NFC on first loop
int numActive = 0;

int lastLooperModeCounter = 0;
int looperModeCounter = 0;

bool first = true;
bool looperRecording = false;
bool looperPlaying = false;
bool looperPassthrough = true;

int pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int mod = MAX_SIZE;
int len = 0;
float drywet = 0;
bool res = false;

unsigned long lastButtonLedFlashTime = 0;
int buttonLedFlashInterval = 300;
bool buttonLedState = LOW;

unsigned long lastArpStepTime = 0;

int currentArpScaleStep[NUMCHORDS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int lastArpScaleStep[NUMCHORDS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

uint8_t data[16];

uint8_t lastPatch = 0;
uint8_t currentPatch = 0;

uint8_t keyModifier = 0;
uint8_t lastKeyModifier = 0;

uint8_t program = 0;

unsigned long lastNFCCheckTime = 0;
int nfcCheckInterval = 1500;
unsigned long lastActiveTime = 0;

float halfStepModifier = 1.0;
float upMappedHalfStepJoystickReadingFloat;
float downMappedHalfStepJoystickReadingFloat;

float arpLevel = 0.0;

int halfStepPixelMultiplier = 0;
int lastHalfStepPixelMultiplier = 0;
bool halfStepPixelReset = false;
float octaveModifier = 1.0;
float upMappedOctaveJoystickReadingFloat;
float downMappedOctaveJoystickReadingFloat;

float looperInput = 0.0;
float looperOutput = 0.0;

int pixelMappedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int pixelZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int lastPixelMappedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

bool irToggleState = 1;

bool firstPress = true;

bool lightsActive = true;

unsigned long lastHallCheckTime = 0;
int hallCheckInterval = 100;

bool melodyMode = false;

int song = 0;

int lastSequenceStep = -1;
int sequenceStep = 0;

bool lastAaState = false;
bool aaState = false;

unsigned long lastAaSequenceStepTime = 0;
int currentAaSequenceStep = 0;

bool analogReadEnabled = true;

unsigned long lastOffTime = 0;

int nfcWaitInterval = 1000;

bool noteSet = false;

bool newPadPlaced = false;

int pulseCounter = 0;
unsigned long lastFirstPulseTime = 0;
int pulseTimeout = 20;

unsigned long lastGateResetTime = 0;

unsigned long lastPixelOnTime[32];

int smoothedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };



#define NUMARPRIPPLESTEPS 8
int lastArpRippleModIndex = 0;
int arpRippleModIndex = 0;
// float arpRippleModifier[8] = { 0.75, 0.875, 1.0, 0.875, 0.75, 0.625, 0.5, 0.625 };
// float arpRippleModifier[2][NUMARPRIPPLESTEPS]{
//   { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
//   { 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0.0, 0.25 },
// };

//TEMPORARY BYPASS
float arpRippleModifier[2][NUMARPRIPPLESTEPS]{
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
};


//MELODY MODE
// Somewhere Over the Rainbow
// Ode to Joy
// The Way You Look Tonight
// Swan Lake
// You Are My Sunshine

int numSequenceSteps[5] = { 23, 39, 33, 16, 46 };

int melodyNote[5][48]{
  { 0, 7, 6, 4, 5, 6, 7, 0, 5, 4, 5, 3, 2, 0, 1, 2, 3, 1, 6, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2, 9, 2, 3, 4, 9, 4, 3, 2, 1, 0, 9, 0, 1, 2, 9, 2, 1, 9, 1, 2, 9, 2, 3, 4, 9, 4, 3, 2, 1, 0, 9, 0, 1, 2, 1, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 4, 9, 4, 9, 4, 0, 1, 2, 3, 2, 1, 2, 3, 4, 3, 2, 3, 4, 5, 4, 3, 4, 5, 6, 7, 0, 1, 2, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 5, 1, 2, 3, 4, 5, 3, 5, 3, 5, 1, 3, 1, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 9, 0, 1, 2, 9, 2, 9, 2, 1, 2, 0, 9, 0, 9, 0, 1, 2, 3, 5, 9, 5, 4, 3, 2, 0, 1, 2, 3, 5, 9, 5, 4, 3, 2, 0, 9, 0, 1, 2, 3, 1, 9, 1, 2, 0, 0, 0 },
};


int melodyNoteOctaveModifier[5][48]{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};


int melodyNoteAccidentalModifier[5][48]{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};



bool tickProcess;

void resetBuffer();

void looperProcess(float &looperOutput, float *looperInput, size_t i);



int aaStepDuration = 110;
int aanNumChords = 2;
int aaChord[64];  // theoretically we could have 64 changes, a chord change every 16th note, but this is incredibly unlikely
int aaChordEventStep[64];

int aaChordNote[4][7]{
  { 36, 38, 40, 41, 43, 45, 47 },
  { 40, 41, 43, 45, 47, 48, 50 },
  { 43, 45, 47, 48, 50, 52, 53 },
  { 47, 48, 50, 52, 53, 55, 57 },
};


int aaVoiceNote[3][64];

bool aaDrumSequenceStep[3][64]{
  { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
};



int aaQuarterSequenceStep[32] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124 };
int aaSixteenthOneSequenceStep[32] = { 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125 };
int aaEighthSequenceStep[32] = { 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126 };
int aaSixteenthTwoSequenceStep[32] = { 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127 };












void setup() {

  delay(50);

  float samplerate;
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();
  tick.Init(1.0f, samplerate);
  resetBuffer();


  //MAIN VOICES
  for (int i = 0; i < NUMMAINVOICES; i++) {
    mainVoiceOscillator[i]->Init(samplerate);
    mainVoiceFilter[i]->Init(samplerate);
  }

  for (int i = 0; i < 32; i++) {
    mainVoiceEnvelope[i]->Init(samplerate);
  }

  for (int i = 0; i < NUMCHORDS; i++) {
    mainVoiceFilter[i]->Init(samplerate);
    mainVoiceTremolo[i]->Init(samplerate);
  }



  //ARP VOICES
  for (int i = 0; i < NUMARPVOICES; i++) {
    arpVoiceOscillator[i]->Init(samplerate);
    arpVoiceEnvelope[i]->Init(samplerate);
  }
  arpVoiceFilter.Init(samplerate);



  //AA VOICES
  for (int i = 0; i < 3; i++) {
    aaVoiceOscillator[i]->Init(samplerate);
    aaVoiceEnvelope[i]->Init(samplerate);
  }



  // DRUM VOICES
  for (int i = 0; i < 3; i++) {
    kickOscillator[i]->Init(samplerate);
    kickOscillator[i]->SetWaveform(kickOscillator[i]->WAVE_SIN);
  }
  kickOscillator[0]->SetFreq(60.0);
  kickOscillator[1]->SetFreq(61.0);
  kickOscillator[2]->SetFreq(62.0);

  snareNoise.Init();
  hihatNoise.Init();

  kickEnv.Init(samplerate);
  snareEnv.Init(samplerate);
  hihatEnv.Init(samplerate);

  kickEnv.SetAttackTime(0.025);
  kickEnv.SetDecayTime(0.125);
  kickEnv.SetSustainLevel(0.0);
  kickEnv.SetReleaseTime(0.001);

  snareEnv.SetAttackTime(0.025);
  snareEnv.SetDecayTime(0.125);
  snareEnv.SetSustainLevel(0.0);
  snareEnv.SetReleaseTime(0.001);

  hihatEnv.SetAttackTime(0.025);
  hihatEnv.SetDecayTime(0.05);
  hihatEnv.SetSustainLevel(0.0);
  hihatEnv.SetReleaseTime(0.001);

  snareHighPass.Init(samplerate);
  snareLowPass.Init(samplerate);
  hihatHighPass.Init(samplerate);
  hihatLowPass.Init(samplerate);

  snareHighPass.SetFreq(200.0);
  snareHighPass.SetRes(0.85);
  snareHighPass.SetDrive(0.8);

  snareLowPass.SetFreq(5000.0);
  snareLowPass.SetRes(0.85);
  snareLowPass.SetDrive(0.8);

  hihatHighPass.SetFreq(8000.0);
  hihatHighPass.SetRes(0.85);
  hihatHighPass.SetDrive(0.8);

  hihatLowPass.SetFreq(10000.0);
  hihatLowPass.SetRes(0.85);
  hihatLowPass.SetDrive(0.8);

  decimator.Init();

  delay(25);


  Serial.begin(9600);
  hwSerial.begin(115200);

  delay(25);

  Serial.println("Hello");


  while (!strip.begin()) {
    delay(10);
  }
  strip.show();


  delay(25);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(irRxPin, INPUT_PULLUP);
  pinMode(irTxPin, OUTPUT);
  pinMode(hallPin, INPUT);
  pinMode(octaveJoystickSelectPin, INPUT_PULLUP);
  digitalWrite(irTxPin, LOW);


  updatePatch();

  DAISY.SetAudioBlockSize(48);

  DAISY.begin(MyCallback);


  for (int i = 0; i < NUMMAINVOICES; i++) {
    mainVoiceGate[i] = 0;
  }

  for (int i = 0; i < 3; i++) {
    aaVoiceGate[i] = 0;
  }

  for (int i = 0; i < 32; i++) {
    lastPixelOnTime[i] = 0;
  }

  rainbowChaseSlow();
}













void MyCallback(float **in, float **out, size_t size) {

  float mainVoiceOscProcess[NUMMAINVOICES];
  float arpVoiceOscProcess[NUMARPVOICES];
  float unfilteredArpVoices = 0.0;
  float aaVoiceOscProcess[3];
  float kickVoiceProcess[3];
  float sumKickOscillators = 0.0;
  float snareNoiseProcess = 0.0;
  float hihatNoiseProcess = 0.0;
  float mainVoiceFilterProcess[NUMCHORDS];
  float arpVoiceFilterProcess[NUMARPVOICES];
  float aaVoiceFilterProcess[3];
  float snareHighPassProcess = 0.0;
  float snareLowPassProcess = 0.0;
  float hihatHighPassProcess = 0.0;
  float hihatLowPassProcess = 0.0;
  float mainVoiceEnvProcess[NUMMAINVOICES];
  float arpVoiceEnvProcess[NUMARPVOICES];
  float aaVoiceEnvProcess[3];
  float kickEnvProcess = 0.0;
  float snareEnvProcess = 0.0;
  float hihatEnvProcess = 0.0;
  float kickOutput = 0.0;
  float snareOutput = 0.0;
  float hihatOutput = 0.0;
  float mainVoiceOutput[NUMMAINVOICES];
  float arpVoiceOutput[NUMARPVOICES];
  float aaVoiceOutput[3];
  float mainVoiceTremoloProcess[NUMCHORDS];
  float bitcrushProccess;
  float sumMainVoices = 0.0;
  float sumArpVoices = 0.0;
  float sumAaVoices = 0.0;
  float sumAaDrums = 0.0;
  float looperInput = 0.0;
  float looperOutput = 0.0;
  float probeOscillatorOutput = 0.0;
  float finalOutput = 0.0;


  for (int j = 0; j < 8; j++) {

    if (smoothedZoneReading[j] > zoneReading[j] + 0) {
      smoothedZoneReading[j] = smoothedZoneReading[j] - 1;
    } else if (smoothedZoneReading[j] < zoneReading[j] - 0) {
      smoothedZoneReading[j] = smoothedZoneReading[j] + 1;
    } else {
      smoothedZoneReading[j] = zoneReading[j];
    }
    mainVoiceLevelModifier[j] = smoothedZoneReading[j] / 1023.0;
  }


  for (size_t i = 0; i < size; i++) {

    tickProcess = tick.Process();

    if (currentPatch == 0) {  // DEFAULT
      // only process every third voice
      for (int j = 0; j < NUMCHORDS; j++) {
        mainVoiceEnvProcess[(j * NUMVOICESPERCHORD)] = mainVoiceEnvelope[(j * NUMVOICESPERCHORD) + 9]->Process(mainVoiceGate[j * NUMVOICESPERCHORD]);
        mainVoiceOscProcess[j * NUMVOICESPERCHORD] = mainVoiceOscillator[j * NUMVOICESPERCHORD]->Process();
        mainVoiceFilterProcess[j] = mainVoiceFilter[j]->Process(mainVoiceOscProcess[j * NUMVOICESPERCHORD]);
        mainVoiceOutput[j * NUMVOICESPERCHORD] = mainVoiceFilterProcess[j] * mainVoiceEnvProcess[j * NUMVOICESPERCHORD] * mainVoiceLevelModifier[j];
      }
      sumMainVoices = 0;
      for (int j = 0; j < NUMCHORDS; j++) {
        sumMainVoices = sumMainVoices + mainVoiceOutput[j * NUMVOICESPERCHORD];
      }
      // only process 3 arp voices
      for (int j = 0; j < 3; j++) {
        arpVoiceEnvProcess[j] = arpVoiceEnvelope[j]->Process(arpVoiceGate[j]);
        arpVoiceOscProcess[j] = arpVoiceOscillator[j]->Process();
        // arpVoiceFilterProcess[j] = arpVoiceFilter[j]->Process(arpVoiceOscProcess[j]);
        arpVoiceFilterProcess[j] = arpVoiceOscProcess[j];
        arpVoiceOutput[j] = arpVoiceFilterProcess[j] * arpVoiceLevelModifier[j];
      }
      sumArpVoices = 0;
      for (int j = 0; j < 3; j++) {
        sumArpVoices = sumArpVoices + arpVoiceOutput[j];
      }

      arpVoiceFilter.Process(sumArpVoices);

      sumArpVoices = arpVoiceFilter.Low();

      looperInput = sumMainVoices + sumArpVoices;
    }




    else if (currentPatch == 4) {  //SMOOTH
      // only process every third voice
      for (int j = 0; j < NUMCHORDS; j++) {
        mainVoiceEnvProcess[j * NUMVOICESPERCHORD] = mainVoiceEnvelope[j * NUMVOICESPERCHORD]->Process(mainVoiceGate[j * NUMVOICESPERCHORD]);
        mainVoiceOscProcess[j * NUMVOICESPERCHORD] = mainVoiceOscillator[j * NUMVOICESPERCHORD]->Process();
        mainVoiceFilterProcess[j] = mainVoiceFilter[j]->Process(mainVoiceOscProcess[j * NUMVOICESPERCHORD]);
        mainVoiceOutput[j * NUMVOICESPERCHORD] = mainVoiceFilterProcess[j] * mainVoiceEnvProcess[j * NUMVOICESPERCHORD] * mainVoiceLevelModifier[j];
      }
      sumMainVoices = 0;
      for (int j = 0; j < NUMCHORDS; j++) {
        sumMainVoices = sumMainVoices + mainVoiceOutput[j * NUMVOICESPERCHORD];
      }
      // only process 1 arp voices
      for (int j = 0; j < 1; j++) {
        arpVoiceEnvProcess[j] = arpVoiceEnvelope[j]->Process(arpVoiceGate[j]);
        arpVoiceOscProcess[j] = arpVoiceOscillator[j]->Process();
        // arpVoiceFilterProcess[j] = arpVoiceFilter[j]->Process(arpVoiceOscProcess[j]);
        arpVoiceFilterProcess[j] = arpVoiceOscProcess[j];
        arpVoiceOutput[j] = arpVoiceFilterProcess[j] * arpVoiceLevelModifier[j];
      }
      sumArpVoices = 0;
      for (int j = 0; j < 1; j++) {
        sumArpVoices = sumArpVoices + arpVoiceOutput[j];
      }


      arpVoiceFilter.Process(sumArpVoices);

      sumArpVoices = arpVoiceFilter.Low();

      looperInput = sumMainVoices + sumArpVoices;





    } else if (currentPatch == 3) {  // WAVY
      // only process every third voice
      for (int j = 0; j < NUMCHORDS; j++) {
        mainVoiceEnvProcess[j * NUMVOICESPERCHORD] = mainVoiceEnvelope[j * NUMVOICESPERCHORD]->Process(mainVoiceGate[j * NUMVOICESPERCHORD]);
        mainVoiceOscProcess[j * NUMVOICESPERCHORD] = mainVoiceOscillator[j * NUMVOICESPERCHORD]->Process();
        mainVoiceFilterProcess[j] = mainVoiceFilter[j]->Process(mainVoiceOscProcess[j * NUMVOICESPERCHORD]);
        mainVoiceOutput[j * NUMVOICESPERCHORD] = mainVoiceFilterProcess[j] * mainVoiceEnvProcess[j * NUMVOICESPERCHORD] * mainVoiceLevelModifier[j];
      }
      sumMainVoices = 0;
      for (int j = 0; j < NUMCHORDS; j++) {
        sumMainVoices = sumMainVoices + mainVoiceOutput[j * NUMVOICESPERCHORD];
      }
      sumMainVoices = mainVoiceTremolo[0]->Process(sumMainVoices);

      // only process 3 arp voices
      for (int j = 0; j < 3; j++) {
        arpVoiceEnvProcess[j] = arpVoiceEnvelope[j]->Process(arpVoiceGate[j]);
        arpVoiceOscProcess[j] = arpVoiceOscillator[j]->Process();
        // arpVoiceFilterProcess[j] = arpVoiceFilter[j]->Process(arpVoiceOscProcess[j]);
        arpVoiceFilterProcess[j] = arpVoiceOscProcess[j];
        arpVoiceOutput[j] = arpVoiceFilterProcess[j] * arpVoiceLevelModifier[j];
      }
      sumArpVoices = 0;
      for (int j = 0; j < 3; j++) {
        sumArpVoices = sumArpVoices + arpVoiceOutput[j];
      }


      arpVoiceFilter.Process(sumArpVoices);

      sumArpVoices = arpVoiceFilter.Low();

      looperInput = sumMainVoices + sumArpVoices;




    } else if (currentPatch == 2) {  // ROUGH
      // only process every third voice
      for (int j = 0; j < NUMCHORDS; j++) {
        mainVoiceEnvProcess[j * NUMVOICESPERCHORD] = mainVoiceEnvelope[j * NUMVOICESPERCHORD]->Process(mainVoiceGate[j * NUMVOICESPERCHORD]);
        mainVoiceOscProcess[j * NUMVOICESPERCHORD] = mainVoiceOscillator[j * NUMVOICESPERCHORD]->Process();
        mainVoiceFilterProcess[j] = mainVoiceFilter[j]->Process(mainVoiceOscProcess[j * NUMVOICESPERCHORD]);
        mainVoiceOutput[j * NUMVOICESPERCHORD] = mainVoiceFilterProcess[j] * mainVoiceEnvProcess[j * NUMVOICESPERCHORD] * mainVoiceLevelModifier[j];
      }
      sumMainVoices = 0;
      for (int j = 0; j < NUMCHORDS; j++) {
        sumMainVoices = sumMainVoices + mainVoiceOutput[j * NUMVOICESPERCHORD];
      }


      // only process 3 arp voices
      for (int j = 0; j < 3; j++) {
        arpVoiceEnvProcess[j] = arpVoiceEnvelope[j]->Process(arpVoiceGate[j]);
        arpVoiceOscProcess[j] = arpVoiceOscillator[j]->Process();
        // arpVoiceFilterProcess[j] = arpVoiceFilter[j]->Process(arpVoiceOscProcess[j]);
        arpVoiceFilterProcess[j] = arpVoiceOscProcess[j];
        arpVoiceOutput[j] = arpVoiceFilterProcess[j] * arpVoiceLevelModifier[j];
      }
      sumArpVoices = 0;
      for (int j = 0; j < 3; j++) {
        sumArpVoices = sumArpVoices + arpVoiceOutput[j];
      }

      arpVoiceFilter.Process(sumArpVoices);

      sumArpVoices = arpVoiceFilter.Low();

      looperInput = (sumMainVoices + sumArpVoices) * 0.5;
      looperInput = decimator.Process(looperInput);
    }




    // AUTO-ACCOMPANIST
    if (aaState == true) {

      // AA #nofilter
      for (int j = 0; j < 3; j++) {
        aaVoiceEnvProcess[j] = aaVoiceEnvelope[j]->Process(aaVoiceGate[j]);
        aaVoiceOscProcess[j] = aaVoiceOscillator[j]->Process();
        aaVoiceFilterProcess[j] = aaVoiceOscProcess[j];
        aaVoiceOutput[j] = aaVoiceFilterProcess[j] * aaVoiceEnvProcess[j];
      }
      sumAaVoices = 0;
      for (int j = 0; j < 3; j++) {
        sumAaVoices = sumAaVoices + aaVoiceOutput[j];
      }
      sumAaVoices = sumAaVoices * 0.05;


      // KICK
      sumKickOscillators = kickOscillator[0]->Process() + kickOscillator[1]->Process() + kickOscillator[2]->Process();
      kickEnvProcess = kickEnv.Process(kickGate);
      kickOutput = sumKickOscillators * kickEnvProcess;

      // SNARE
      snareNoiseProcess = snareNoise.Process();
      snareEnvProcess = snareEnv.Process(snareGate);
      snareHighPass.Process(snareNoiseProcess);
      snareHighPassProcess = snareHighPass.High();
      snareLowPass.Process(snareHighPassProcess);
      snareLowPassProcess = snareLowPass.Low();
      snareOutput = snareEnvProcess * snareLowPassProcess;

      // HIHAT
      hihatNoiseProcess = hihatNoise.Process();
      hihatEnvProcess = hihatEnv.Process(hihatGate);
      hihatHighPass.Process(hihatNoiseProcess);
      hihatHighPassProcess = hihatHighPass.High();
      hihatLowPass.Process(hihatHighPassProcess);
      hihatLowPassProcess = hihatLowPass.Low();
      hihatOutput = hihatEnvProcess * hihatHighPassProcess;


      if (tickProcess) {

        for (int i = 0; i < 3; i++) {
          aaVoiceGate[i] = 1;
          if (aaVoiceNote[i][currentAaSequenceStep] == 100) {
            // do nothing
          } else if (aaVoiceNote[i][currentAaSequenceStep] == 99) {
            // aaVoiceGate[i] = 0;
          } else {
            aaVoiceOscillator[i]->SetFreq((mtof(aaVoiceNote[i][currentAaSequenceStep] + keyModifier + 12) * 1.0));
            // aaVoiceGate[i] = 1;
          }
        }

        kickGate = aaDrumSequenceStep[0][currentAaSequenceStep];
        snareGate = aaDrumSequenceStep[1][currentAaSequenceStep];
        hihatGate = aaDrumSequenceStep[2][currentAaSequenceStep];

        currentAaSequenceStep++;

        if (currentAaSequenceStep >= 64) {
          currentAaSequenceStep = 0;
        }
      }
    }

    sumAaDrums = (kickOutput * 0.3) + (snareOutput * 0.03) + (hihatOutput * 0.03);

    looperProcess(looperOutput, looperInput, i);

    finalOutput = (looperOutput * 2.0) + sumAaVoices + sumAaDrums;


    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = finalOutput;
    }
  }
}













void loop() {


  if (hwSerial.available() > 0) {  // Nano has a message for us

    int nanoRead = hwSerial.read();

    if (nanoRead == 201) {  // read request recieved
      if ((numActive == 0 && millis() > lastOffTime + nfcWaitInterval) || newPadPlaced == true) {
        analogReadEnabled = false;
        hwSerial.write(1);  // approve
        newPadPlaced = false;
      } else {
        hwSerial.write(0);  // deny
      }
    }


    else if (nanoRead == 202) {  // nano is finished reading and didnt find anything
      if (currentPatch != 0) {   // we still have a texture pad loaded, even though no tag has been read
        currentPatch = 0;
        rainbowChaseFast();
        updatePatch();
      }
      analogReadEnabled = true;  // enable analogRead
    }


    else {  // recieved card data from nano

      lastPatch = currentPatch;
      lastKeyModifier = keyModifier;

      // key card parameter types are stored in different blocks, as defined below
      if (nanoRead >= 64 && nanoRead <= 76) {
        keyModifier = nanoRead - 64;
      } else if (nanoRead >= 77 && nanoRead <= 89) {
        currentPatch = nanoRead - 77;
      } else if (nanoRead >= 90 && nanoRead <= 99) {
        melodyMode = true;
        song = nanoRead - 90;
      } else if (nanoRead == 100) {
        lightsActive = false;
      } else if (nanoRead == 101) {
        generateAutoAccompanyment();
        rainbowChaseFast();
      } else if (nanoRead == 102) {
        lastAaState = aaState;
        rainbowChaseFast();
        aaState = !aaState;
      }

      if (currentPatch != lastPatch) {
        updatePatch();
        rainbowChaseFast();
      }
      if (keyModifier != lastKeyModifier) {
        rainbowChaseFast();
      }

      analogReadEnabled = true;  // enable analogRead
    }
  }
















  //ANALOG READS
  if (analogReadEnabled == true) {

    //SEMITONE JOYSTICK
    int halfStepJoystickReading = analogRead(A9);

    int upMappedHalfStepJoystickReading = map(halfStepJoystickReading, HALFSTEPZEROPOINT, HALFSTEPHIGHPOINT, 1000, 1059);
    upMappedHalfStepJoystickReading = constrain(upMappedHalfStepJoystickReading, 1000, 1059);
    upMappedHalfStepJoystickReadingFloat = upMappedHalfStepJoystickReading / 1000.0;

    int downMappedHalfStepJoystickReading = map(halfStepJoystickReading, HALFSTEPLOWPOINT, HALFSTEPZEROPOINT, 944, 1000);
    downMappedHalfStepJoystickReading = constrain(downMappedHalfStepJoystickReading, 944, 1000);
    downMappedHalfStepJoystickReadingFloat = downMappedHalfStepJoystickReading / 1000.0;

    lastHalfStepPixelMultiplier = halfStepPixelMultiplier;
    int halfStepPixelMultiplier = map(abs(halfStepJoystickReading - HALFSTEPZEROPOINT), 0, 200, 0, 30);

    if (halfStepJoystickReading > (HALFSTEPZEROPOINT + 15)) {
      halfStepModifier = downMappedHalfStepJoystickReadingFloat;
      if (halfStepPixelMultiplier != lastHalfStepPixelMultiplier) {
        strip.setPixelColor(34, (8 * halfStepPixelMultiplier), (8 * halfStepPixelMultiplier), (8 * halfStepPixelMultiplier));
        strip.setPixelColor(35, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      halfStepPixelReset = false;
    } else if (halfStepJoystickReading < (HALFSTEPZEROPOINT - 15)) {
      halfStepModifier = upMappedHalfStepJoystickReadingFloat;
      if (halfStepPixelMultiplier != lastHalfStepPixelMultiplier) {
        if (numActive != 0) {
          strip.setPixelColor(35, (8 * halfStepPixelMultiplier), (8 * halfStepPixelMultiplier), (8 * halfStepPixelMultiplier));
          strip.setPixelColor(34, 0, 0, 0);
          if (lightsActive) {
            strip.show();
          }
        }
      }
      halfStepPixelReset = false;
    } else {
      halfStepModifier = 1.0;
      if (halfStepPixelReset == false) {
        strip.setPixelColor(34, 0, 0, 0);
        strip.setPixelColor(35, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
        halfStepPixelReset = true;
      }
    }



    //OCTAVE JOYSTICK
    lastOctaveLocked = octaveLocked;

    lastOctaveJoystickSelectState = octaveJoystickSelectState;
    octaveJoystickSelectState = digitalRead(octaveJoystickSelectPin);

    int octaveJoystickReading = analogRead(A10);

    int upMappedOctaveJoystickReading = map(octaveJoystickReading, OCTAVEZEROPOINT, OCTAVEHIGHPOINT, 1000, 2000);
    upMappedOctaveJoystickReading = constrain(upMappedOctaveJoystickReading, 1000, 2000);
    upMappedOctaveJoystickReadingFloat = upMappedOctaveJoystickReading / 1000.0;

    int downMappedOctaveJoystickReading = map(octaveJoystickReading, OCTAVELOWPOINT, OCTAVEZEROPOINT, 500, 1000);
    downMappedOctaveJoystickReading = constrain(downMappedOctaveJoystickReading, 500, 2000);
    downMappedOctaveJoystickReadingFloat = downMappedOctaveJoystickReading / 1000.0;

    lastOctavePixelMultiplier = octavePixelMultiplier;
    octavePixelMultiplier = map(abs(octaveJoystickReading - OCTAVEZEROPOINT), 0, 200, 0, 30);

    if (octaveJoystickReading > (OCTAVEZEROPOINT + 20)) {
      octaveModifier = downMappedOctaveJoystickReadingFloat;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        strip.setPixelColor(32, (8 * octavePixelMultiplier), (8 * octavePixelMultiplier), (8 * octavePixelMultiplier));
        strip.setPixelColor(33, 0, 0, 0);

        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        octaveLocked = !octaveLocked;
        octaveLockDirection = LOW;
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }


    } else if (octaveJoystickReading < (OCTAVEZEROPOINT - 20)) {
      octaveModifier = upMappedOctaveJoystickReadingFloat;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        strip.setPixelColor(33, (8 * octavePixelMultiplier), (8 * octavePixelMultiplier), (8 * octavePixelMultiplier));
        strip.setPixelColor(32, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        octaveLocked = !octaveLocked;
        octaveLockDirection = HIGH;
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }


    } else {
      octaveModifier = 1.0;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        strip.setPixelColor(32, 0, 0, 0);
        strip.setPixelColor(33, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        if (octaveLocked == true) {
          octaveLocked = false;
        }
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }
    }

    if (octaveLocked == true) {
      if (octaveLockDirection == HIGH) {
        octaveModifier = 2.0;
        if (lastOctaveLocked == false) {
          strip.setPixelColor(33, 200, 200, 200);
          strip.setPixelColor(32, 0, 0, 0);
          if (lightsActive) {
            strip.show();
          }
        }

      } else if (octaveLockDirection == LOW) {
        octaveModifier = 0.5;
        if (lastOctaveLocked == false) {
          strip.setPixelColor(32, 200, 200, 200);
          strip.setPixelColor(33, 0, 0, 0);
          if (lightsActive) {
            strip.show();
          }
        }
      }
    }



    //  READ FSRs
    for (int i = 0; i < NUMZONES; i++) {
      lastZoneReading[i] = zoneReading[i];
      zoneReading[i] = analogRead(zoneSensorPin[i]);
    }


    if (CALIBRATIONMODE == 1) {
      Serial.println("ACC:");
      Serial.println(halfStepJoystickReading);
      Serial.println("");
      Serial.println("");

      Serial.println("OCT:");
      Serial.println(octaveJoystickReading);
      Serial.println("");
      Serial.println("");

      for (int i = 0; i < NUMZONES; i++) {
        Serial.println(i);
        Serial.println(zoneReading[i]);
        Serial.println("");
        Serial.println("");
      }
      delay(100);
    }



    activeThreshold = 25;


    lastArpActiveState = arpActiveState;

    if (zoneReading[8] > 150) {  // middle zone has higher threshold than other zones
      arpActiveState = true;
    } else {
      arpActiveState = false;
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      lastChordActive[i] = chordActive[i];
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      if (zoneReading[i] > activeThreshold) {
        chordActive[i] = true;
      } else {
        chordActive[i] = false;
      }
    }


    lastNumActive = numActive;
    numActive = 0;
    for (int i = 0; i < NUMCHORDS; i++) {
      numActive = numActive + chordActive[i];
    }

    if (numActive != 0) {
      lastActiveTime = millis();
    }

    if (numActive == 0 && lastNumActive != 0) {
      lastOffTime = millis();
    }
  }



















  //MAIN VOICES

  float mainVoiceFilterCutoff[NUMMAINVOICES];

  for (int i = 0; i < NUMCHORDS; i++) {
    mainVoiceFilterCutoff[i] = (zoneReading[i] * 10) + 500;
  }

  if (currentPatch == 4) {
    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilterCutoff[i] = mainVoiceFilterCutoff[i] / 2;
    }
  }

  for (int i = 0; i < NUMCHORDS; i++) {
    for (int j = 0; j < NUMVOICESPERCHORD; j++) {
      mainVoiceFilter[(i * NUMVOICESPERCHORD) + j]->SetFreq(mainVoiceFilterCutoff[(i * NUMVOICESPERCHORD) + j]);
    }
  }

  for (int i = 0; i < NUMCHORDS; i++) {
    if (lastChordActive[i] == false && chordActive[i] == true) {
      mainVoiceGate[i * NUMVOICESPERCHORD] = 1;

    } else if (lastChordActive[i] == true && chordActive[i] == false) {
      mainVoiceGate[i * NUMVOICESPERCHORD] = 0;
    }
  }

  int indexCounterMainOsc = 0;
  for (int i = 0; i < NUMCHORDS; i++) {
    for (int j = 0; j < NUMVOICESPERCHORD; j++) {
      mainVoiceOscillator[indexCounterMainOsc]->SetFreq((mtof(chordNote[i][j] + keyModifier + 24) * 1.0) * halfStepModifier * octaveModifier);
      indexCounterMainOsc++;
    }
  }


  int sumZoneReadings = 0;
  for (int i = 0; i < NUMCHORDS; i++) {
    sumZoneReadings = sumZoneReadings + zoneReading[i];
  }

  int mappedSumZoneReadings = map(sumZoneReadings, 0, 8192, 15, 1500);
  float tremoloRate = mappedSumZoneReadings / 10.0;
  for (int j = 0; j < NUMCHORDS; j++) {
    mainVoiceTremolo[j]->SetFreq(tremoloRate);
    mainVoiceTremolo[j]->SetDepth(1.0);
  }

  decimator.SetDownsampleFactor(0.5);
  decimator.SetBitcrushFactor(0.6);
  decimator.SetBitsToCrush(6);



















  // ARP VOICES
  int arpStepDuration = map(zoneReading[8], 100, 1024, 150, 30);
  arpStepDuration = constrain(arpStepDuration, 10, 150);

  if (millis() > lastArpStepTime + arpStepDuration) {
    lastArpStepTime = millis();

    for (int i = 0; i < NUMCHORDS; i++) {
      lastArpScaleStep[i] = currentArpScaleStep[i];
      currentArpScaleStep[i] = lastArpScaleStep[i] + random(-2, 3);
      if (currentArpScaleStep[i] < 0) {
        currentArpScaleStep[i] = random(1, 2);
      } else if (currentArpScaleStep[i] > 11) {
        currentArpScaleStep[i] = random(8, 11);
      }
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceOscillator[i]->SetFreq((mtof(arpNote[i][currentArpScaleStep[i]] + keyModifier + 24) * 2.0) * halfStepModifier * octaveModifier);
    }

    lastArpRippleModIndex = arpRippleModIndex;
    arpRippleModIndex++;

    if (arpRippleModIndex > 7) {
      arpRippleModIndex = 0;
    }
  }

  float arpVoiceFilterCutoff = zoneReading[8] * 1.5;
  arpVoiceFilter.SetFreq(arpVoiceFilterCutoff);

  if (arpActiveState == true) {
    int arpExponent = 1;
    arpLevel = zoneReading[8] / 1023.0;
    arpLevel = pow(arpLevel, arpExponent);
    arpLevel = arpLevel / 5.0;
  } else {
    arpLevel = 0.0;
  }


  for (int i = 0; i < NUMCHORDS; i++) {
    arpVoiceLevelModifier[i] = arpLevel;
  }

  if (currentPatch == 3) {
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceLevelModifier[i] = arpVoiceLevelModifier[i] / 2.0;
    }
  }


  if (arpActiveState != lastArpActiveState) {
    // Serial.println("arpenv");
    for (int i = 0; i < 3; i++) {  // 3 voices no matter what
      arpVoiceGate[i] = !arpVoiceGate;
    }
  }


















  // NEOPIXELS
  if (melodyMode == false) {

    for (int i = 0; i < NUMCHORDS; i++) {
      lastPixelMappedZoneReading[i] = pixelMappedZoneReading[i];
      pixelMappedZoneReading[i] = map(pixelZoneReading[i], 0, 900, 0, 30);
      pixelMappedZoneReading[i] = constrain(pixelMappedZoneReading[i], 0, 29);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      if ((chordActive[i] == true && pixelMappedZoneReading[i] != lastPixelMappedZoneReading[i]) || (lastChordActive[i] == false && chordActive[i] == true)) {
        for (int j = 0; j < 4; j++) {
          strip.setPixelColor(orderedNeopixel[(i * 4) + j], (colorBase[i][0] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS], (colorBase[i][1] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS], (colorBase[i][2] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS]);
        }
        if (lightsActive) {
          strip.show();
        }
      } else if (lastChordActive[i] == true && chordActive[i] == false) {
        for (int j = 0; j < 4; j++) {
          strip.setPixelColor(orderedNeopixel[(i * 4) + j], 0, 0, 0);
        }
        if (lightsActive) {
          strip.show();
        }
      }
    }
  }

  if (melodyMode == true) {

    int highestZoneReading = 0;

    int strongestZone = 0;

    for (int i = 0; i < NUMZONES; i++) {
      if (zoneReading[i] > highestZoneReading) {
        highestZoneReading = zoneReading[i];
        strongestZone = i;
      }
    }

    if (noteSet == false) {
      if (melodyNote[song][sequenceStep] == 9) {
        for (int i = 0; i < 36; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
      } else {
        for (int i = 0; i < 4; i++) {
          strip.setPixelColor(orderedNeopixel[(melodyNote[song][sequenceStep] * 4) + i], colorBase[melodyNote[song][sequenceStep]][0] * 25, colorBase[melodyNote[song][sequenceStep]][1] * 25, colorBase[melodyNote[song][sequenceStep]][2] * 25);
        }
      }

      if (melodyNoteOctaveModifier[song][sequenceStep] == 2) {
        strip.setPixelColor(33, 210, 210, 210);
      } else {
        strip.setPixelColor(33, 0, 0, 0);
      }

      if (melodyNoteOctaveModifier[song][sequenceStep] == 1) {
        strip.setPixelColor(32, 210, 210, 210);
      } else {
        strip.setPixelColor(32, 0, 0, 0);
      }

      if (melodyNoteAccidentalModifier[song][sequenceStep] == 2) {
        strip.setPixelColor(35, 210, 210, 210);
      } else {
        strip.setPixelColor(35, 0, 0, 0);
      }

      if (melodyNoteAccidentalModifier[song][sequenceStep] == 1) {
        strip.setPixelColor(34, 210, 210, 210);
      } else {
        strip.setPixelColor(34, 0, 0, 0);
      }
      strip.show();
      noteSet = true;
    }


    if (melodyNote[song][sequenceStep] == 9) {
      if (numActive == 0) {
        for (int i = 0; i < 36; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
        sequenceStep++;
        noteSet = false;
      }
    } else if (chordActive[melodyNote[song][sequenceStep]] == true && strongestZone == melodyNote[song][sequenceStep]) {
      for (int i = 0; i < 36; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      sequenceStep++;
      noteSet = false;
    }


    if (sequenceStep >= numSequenceSteps[song]) {
      sequenceStep = 0;
      melodyMode = false;
    }
  }












  // AUTO ACCOMPANIST
  if (lastAaState == true && aaState == false) {
    for (int i = 0; i < 3; i++) {
      aaVoiceGate[i] = 0;
    }
  }














  // IR READ
  if (pulseCounter == 1 && millis() > lastFirstPulseTime + pulseTimeout) {
    pulseCounter = 0;
  }

  if (pulseCounter >= 2) {
    looperModeCounter = 2;
    pos = 0;

    pulseCounter = 0;
  }


















  // LOOPER
  lastButtonState = buttonState;
  buttonState = digitalRead(buttonPin);
  lastLooperModeCounter = looperModeCounter;

  if (buttonState == LOW) {
    if (lastButtonState == HIGH) {
      lastButtonPressTime = millis();
      delay(8);
      looperModeCounter++;
      if (looperModeCounter == 2) {
        first = false;
        mod = len;
        len = 0;
        res = true;
      }
      if (looperModeCounter == 1 && firstPress == false) {
        resetBuffer();
        res = false;
      }
      if (looperModeCounter > 2) {
        looperModeCounter = 0;
        firstPress = false;
        digitalWrite(irTxPin, LOW);
      }
    }

    if (millis() > lastButtonPressTime + resetPressDuration) {
      resetToBootLoader();
    }

  } else if (lastButtonState == LOW && buttonState == HIGH) {
    delay(8);
  }

  if (looperModeCounter == 0) {
    looperPassthrough = true;
    looperRecording = false;
    looperPlaying = false;
  } else if (looperModeCounter == 1) {
    looperPassthrough = false;
    looperRecording = true;
    looperPlaying = false;
  } else if (looperModeCounter == 2) {
    looperPassthrough = false;
    looperRecording = false;
    looperPlaying = true;
  }















  // BUTTON LED
  if (lightsActive) {
    if (looperModeCounter == 0) {
      digitalWrite(ledPin, LOW);
    } else if (looperModeCounter == 1) {
      if (millis() > lastButtonLedFlashTime + buttonLedFlashInterval) {
        lastButtonLedFlashTime = millis();
        if (buttonLedState == HIGH) {
          digitalWrite(ledPin, LOW);
          buttonLedState = LOW;
        } else {
          digitalWrite(ledPin, HIGH);
          buttonLedState = HIGH;
        }
      }
    } else if (looperModeCounter == 2) {
      digitalWrite(ledPin, HIGH);
    }
  }
}
// end of main loop





















void resetBuffer() {
  first = true;
  pos = 0;
  len = 0;
  for (int i = 0; i < mod; i++) {
    buf[i] = 0;
  }
  mod = MAX_SIZE;
}



void writeBuffer(float looperInput, size_t i) {
  buf[pos] = buf[pos] * 0.5 + looperInput * 0.5;
  if (first) {
    len++;
  }
}



void looperProcess(float &looperOutput, float looperInput, size_t i) {
  if (looperRecording) {
    writeBuffer(looperInput, i);
    looperOutput = looperInput;
  }

  // automatic looptime
  if (len >= MAX_SIZE) {
    first = false;
    mod = MAX_SIZE;
    len = 0;
  }

  if (!looperPassthrough) {
    pos++;
    pos %= mod;

    if (pos <= 2000) {
      digitalWrite(irTxPin, HIGH);
    } else {
      digitalWrite(irTxPin, LOW);
    }
  }


  if (looperPlaying) {
    looperOutput = buf[pos] + looperInput;
  }


  if (looperPassthrough) {
    looperOutput = looperInput;
  }
}











void updatePatch() {

  //DEFAULT
  if (currentPatch == 0) {
    //MAIN VOICES
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SAW);
    }


    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.025);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
  }







  // SMOOTH
  if (currentPatch == 4) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.025);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
  }




  // WAVY
  if (currentPatch == 3) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.025);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SIN);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(500);
    }
  }



  //ROUGH
  if (currentPatch == 2) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SAW);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.025);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
  }


  //AA VOICES
  for (int i = 0; i < 3; i++) {
    aaVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
    aaVoiceEnvelope[i]->SetSustainLevel(1.0);
  }
}







void resetToBootLoader() {
  dsy_gpio_pin bootpin = { DSY_GPIOG, 3 };
  dsy_gpio pin;
  pin.mode = DSY_GPIO_MODE_OUTPUT_PP;
  pin.pin = bootpin;
  dsy_gpio_init(&pin);
  dsy_gpio_write(&pin, 1);
  delay(10);
  HAL_NVIC_SystemReset();
}









void readIR() {
  irRxState = !digitalRead(irRxPin);
  pulseCounter++;

  if (pulseCounter == 1) {
    lastFirstPulseTime = millis();
  }
}










void rainbowChaseFast() {

  int nextPixelInterval = 7;
  int holdOnDuration = 150;
  bool pixelOn[32];
  bool allClear = false;

  for (int i = 0; i < 32; i++) {
    pixelOn[i] = false;
  }

  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(orderedNeopixel[i], colorBase[i / 4][0] * 2, colorBase[i / 4][1] * 2, colorBase[i / 4][2] * 2);
    lastPixelOnTime[i] = millis();
    pixelOn[i] = true;
    for (int j = 0; j < 32; j++) {
      if (millis() > lastPixelOnTime[j] + holdOnDuration && pixelOn[j] == true) {
        strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
        pixelOn[j] = false;
      }
    }
    delay(nextPixelInterval);
    strip.show();
  }

  while (!allClear) {
    allClear = true;
    for (int j = 0; j < 32; j++) {
      if (pixelOn[j] == true) {
        allClear = false;
        if (millis() > lastPixelOnTime[j] + holdOnDuration) {
          strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
          strip.show();
          pixelOn[j] = false;
        }
      }
    }
  }
}







void rainbowChaseSlow() {

  int nextPixelInterval = 28;
  int holdOnDuration = 375;
  bool pixelOn[32];
  bool allClear = false;

  for (int i = 0; i < 32; i++) {
    pixelOn[i] = false;
  }


  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(orderedNeopixel[i], colorBase[i / 4][0] * 3, colorBase[i / 4][1] * 3, colorBase[i / 4][2] * 3);
    lastPixelOnTime[i] = millis();
    pixelOn[i] = true;
    for (int j = 0; j < 32; j++) {
      if (millis() > lastPixelOnTime[j] + holdOnDuration && pixelOn[j] == true) {
        strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
        pixelOn[j] = false;
      }
    }
    delay(nextPixelInterval);
    strip.show();
  }

  while (!allClear) {
    allClear = true;
    for (int j = 0; j < 32; j++) {
      if (pixelOn[j] == true) {
        allClear = false;
        if (millis() > lastPixelOnTime[j] + holdOnDuration) {
          strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
          strip.show();
          pixelOn[j] = false;
        }
      }
    }
  }
}







void generateAutoAccompanyment() {


  // // initialize random number generator
  int randomSeedValue = millis() % 112798;
  randomSeed(randomSeedValue);


  int tempoVariation = random(-6, 6);

  float tickFrequency = 12.0 + tempoVariation;

  tick.SetFreq(tickFrequency);


  // generate when the chords change, and how many chords there are
  aaChordEventStep[0] = 0;  // first chord always happens on beat 1
  int aaNumChords = 0;
  int previousEventStep = 0;
  int eventStep = 0;
  int maxNumBeats = 8;  // next chord must occur within this number of steps

  for (int i = 1; eventStep < 64; i++) {  // i = 1: start with 2nd chord
    previousEventStep = eventStep;

    int randomNumber = random(0, 100);

    if (randomNumber < 0) {
      aaChordEventStep[i] = aaSixteenthOneSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;

    } else if (randomNumber >= 0 && randomNumber < 0) {
      aaChordEventStep[i] = aaSixteenthTwoSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }

    else if (randomNumber >= 15 && randomNumber < 25) {
      aaChordEventStep[i] = aaEighthSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }

    else {
      aaChordEventStep[i] = aaQuarterSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }
  }

  // generate the chord progression
  for (int i = 0; i < aaNumChords; i++) {
    aaChord[i] = random(0, 7);
  }

  // generate note hold durations
  int noteHoldSteps[3][aaNumChords];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < aaNumChords; j++) {
      noteHoldSteps[i][j] = random(1, aaChordEventStep[j + 1] - aaChordEventStep[j]);
    }
  }

  // generate bass voice sequence
  int currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {
      aaVoiceNote[0][i] = aaChordNote[0][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {
      aaVoiceNote[0][i] = 99;
    } else {
      aaVoiceNote[0][i] = 100;
    }
  }

  // generate next voice sequence
  currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {
      aaVoiceNote[1][i] = aaChordNote[1][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {
      aaVoiceNote[1][i] = 99;
    } else {
      aaVoiceNote[1][i] = 100;
    }
  }


  // generate next voice sequence
  currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {  // note on
      aaVoiceNote[2][i] = aaChordNote[2][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {  // note off
      aaVoiceNote[2][i] = 99;
    } else {  // no event
      aaVoiceNote[2][i] = 100;
    }
  }


  aaState = true;
  currentAaSequenceStep = 0;
}
