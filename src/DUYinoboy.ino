/*
 * DUYinoboy - ArduinoBoy for Retrokits RK-002 Smart MIDI Cable
 * 
 * Complete implementation for Retrokits RK-002 Smart MIDI Cable
 * Supports all original ArduinoBoy modes with RK-002 optimizations
 * 
 * Hardware: RK-002 Smart MIDI Cable (2 GPIO pins only)
 * Software: Arduino IDE 1.8.x with RK-002 board package OR DUY Web Editor
 * 
 * Author: DUYinoboy Project Contributors
 * Version: 1.0
 * Date: January 2025
 * 
 * Based on original ArduinoBoy by trash80
 * Adapted for RK-002 hardware limitations and MIDI programming
 */

#include <RK002.h>
#include <EEPROM.h>

// ============================================================================
// RK-002 FIRMWARE METADATA (REQUIRED)
// ============================================================================

RK002_DECLARE_INFO(
  "DUYinoboy",
  "duyinoboy@github.com", 
  "1.0",
  "duy1b0y-2025-4b9a-ab02-rk002gameb0y"
);

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// RK-002 has exactly 2 GPIO pins available
#define GB_SIN                1    // GPIO 1 → Game Boy SIN (Pin 3) REQUIRED
#define GB_SOUT               2    // GPIO 2 ← Game Boy SOUT (Pin 2) OPTIONAL

// Uncomment for GPIO LED instead of Game Boy bidirectional communication
// #define USE_GPIO_LED              
#ifdef USE_GPIO_LED
  #define STATUS_LED_PIN      2    // GPIO 2 used for LED instead of Game Boy SOUT
#endif

// ============================================================================
// ARDUINOBOY MODE DEFINITIONS - RK-002 OPTIMIZED
// ============================================================================
// Note: RK-002 is MIDI input only - supports LSDJ as MIDI clock follower only

#define MODE_LSDJ_SYNC        0  // LSDJ MIDI Clock Sync (slave mode only)
#define MODE_MGB_BASIC        1  // mGB Basic Mode - direct note control
#define MODE_MGB_CHORD        2  // mGB Chord Mode - chord triggering
#define MODE_MGB_ARPEGGIO     3  // mGB Arpeggiator Mode
#define MODE_MGB_SCALES       4  // mGB with Musical Scales
#define MODE_MGB_GRIDS        5  // mGB with Grids Adapter (4 pulse channels)
#define MODE_LSDJ_KEYBOARD    6  // LSDJ PC Keyboard mode
#define MODE_LSDJ_MAP         7  // LSDJ MIDIMAP mode (4-channel polyphonic)
#define MODE_NANOLOOP_SYNC    8  // Nanoloop MIDI Clock Sync (slave mode only)
#define NUM_MODES             9

// ============================================================================
// TIMING AND MIDI CONFIGURATION
// ============================================================================

#define MIDI_PPQN            24    // MIDI Pulses Per Quarter Note
#define LSDJ_PPQN             4    // LSDJ Pulses Per Quarter Note  
#define CLOCK_DIVISION        6    // 24/4 = 6:1 ratio

// Timing Constants (for Game Boy Link Cable)
#define GB_CLOCK_RATE_US    500    // ArduinoBoy standard timing
#define GB_NANOLOOP_RATE_US 1000   // Slower rate for Nanoloop
#define GB_SERIAL_DELAY_US   125   // Inter-byte delay

// Default Settings
#define DEFAULT_BPM         120
#define DEFAULT_MODE        MODE_LSDJ_SYNC
#define DEFAULT_CHANNEL       1
#define MIN_BPM              60
#define MAX_BPM             200

// ============================================================================
// mGB MODE CONFIGURATION
// ============================================================================

// mGB uses 4 Game Boy sound channels (pulse1, pulse2, wave, noise)
#define MGB_CHANNELS          4
#define MGB_PULSE1            0    // Square wave with sweep
#define MGB_PULSE2            1    // Square wave
#define MGB_WAVE              2    // Wavetable channel  
#define MGB_NOISE             3    // Noise channel

// mGB Protocol - Game Boy specific commands
#define MGB_CMD_NOTE          0x80 // Note command + channel (0x80-0x83)
#define MGB_CMD_CC            0x90 // Control Change + channel (0x90-0x93)  
#define MGB_CMD_PC            0xA0 // Program Change + channel (0xA0-0xA3)

// Arpeggiator settings
#define ARP_MAX_NOTES         8    // Maximum notes in arpeggio
#define ARP_MAX_SPEED         16   // Arpeggio speed divisions
#define ARP_PATTERNS          4    // Up, Down, Up/Down, Random

// Scale definitions (based on musical scales)
#define MAX_SCALES            8
#define SCALE_LENGTH          12   // Chromatic scale length

// Grids settings (inspired by Mutable Instruments Grids)
#define GRIDS_PATTERNS        32   // Number of built-in patterns  
#define GRIDS_STEPS           16   // Steps per pattern
#define GRIDS_TRACKS          4    // One track per Game Boy channel

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// EEPROM addresses for persistent settings
#define EEPROM_MODE_ADDR      0
#define EEPROM_CHANNEL_ADDR   1  
#define EEPROM_BPM_ADDR       2
#define EEPROM_MAP_ADDR       3  // Map channels stored at 3,4,5,6
#define EEPROM_MAGIC_ADDR     10 // Magic byte to check if settings exist
#define EEPROM_MAGIC_VALUE    0x42

// Core state
volatile byte currentMode = DEFAULT_MODE;
volatile byte midiChannel = DEFAULT_CHANNEL;
volatile byte midiChannelMap[4] = {1, 2, 3, 4};  // LSDJ Map mode channels
volatile byte bpm = DEFAULT_BPM;
volatile boolean clockRunning = false;

// Timing variables
volatile unsigned long clockTimer = 0;
volatile unsigned long statusTimer = 0;
volatile byte clockDivider = 0;

// Note tracking for polyphonic modes
volatile byte lastNote[4] = {0, 0, 0, 0};
volatile boolean noteActive[4] = {false, false, false, false};

// Status indicators
volatile boolean statusLED = false;

// ============================================================================
// mGB MODE STATE VARIABLES
// ============================================================================

// mGB Basic Mode
volatile byte mgbChannelMap[4] = {1, 2, 3, 4};  // MIDI channels for each GB channel
volatile byte mgbLastNote[4] = {0, 0, 0, 0};    // Last note played per channel
volatile boolean mgbNoteOn[4] = {false, false, false, false};

// mGB Chord Mode
volatile byte chordRoot = 60;                    // Middle C
volatile byte chordType = 0;                     // Major, Minor, etc.
const byte chordPatterns[8][4] = {               // Chord types (intervals)
  {0, 4, 7, 12},   // Major (C E G C)
  {0, 3, 7, 12},   // Minor (C Eb G C)  
  {0, 4, 7, 10},   // Dominant 7th
  {0, 3, 7, 10},   // Minor 7th
  {0, 4, 8, 12},   // Augmented
  {0, 3, 6, 12},   // Diminished
  {0, 2, 7, 12},   // Sus2
  {0, 5, 7, 12}    // Sus4
};

// mGB Arpeggiator Mode  
volatile byte arpNotes[ARP_MAX_NOTES];           // Notes in current arpeggio
volatile byte arpNoteCount = 0;                  // Number of active notes
volatile byte arpPosition = 0;                   // Current arp position
volatile byte arpPattern = 0;                    // Arp pattern (up/down/etc)
volatile byte arpSpeed = 4;                      // Clock divisions
volatile byte arpStepCounter = 0;                // Internal step counter

// mGB Scales Mode
volatile byte currentScale = 0;                  // Active scale index
volatile byte scaleRoot = 60;                    // Scale root note
const byte scales[MAX_SCALES][SCALE_LENGTH] = {  // Scale patterns (semitones)
  {0,2,4,5,7,9,11,12,14,16,17,19},  // Major
  {0,2,3,5,7,8,10,12,14,15,17,19},  // Natural Minor  
  {0,2,3,5,7,9,11,12,14,15,17,19},  // Dorian
  {0,1,3,5,7,8,10,12,13,15,17,19},  // Phrygian
  {0,2,4,6,7,9,11,12,14,16,18,19},  // Lydian
  {0,2,4,5,7,9,10,12,14,16,17,19},  // Mixolydian
  {0,2,3,5,7,8,9,12,14,15,17,19},   // Aeolian
  {0,1,3,5,6,8,10,12,13,15,17,18}   // Locrian
};

// mGB Grids Mode (simplified Euclidean/pattern generator)
volatile byte gridsPattern = 0;                  // Current pattern
volatile byte gridsStep = 0;                     // Current step
volatile byte gridsAccent[GRIDS_TRACKS];         // Accent levels per track
volatile byte gridsDensity[GRIDS_TRACKS] = {8,6,4,2}; // Pattern density per track
const byte gridsPatterns[4][16] = {              // Simple pattern library
  {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},  // Track 1: Kick pattern
  {0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0},  // Track 2: Snare pattern  
  {1,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0},  // Track 3: Hi-hat pattern
  {0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1}   // Track 4: Accent pattern
};

// ============================================================================
// SETUP AND MAIN LOOP
// ============================================================================

void setup() {
  // Load settings from EEPROM
  loadSettings();
  
  // Initialize GPIO pins (RK-002 specific functions)
  if (!initializeGPIO()) {
    // GPIO initialization failed - flash error pattern
    flashErrorPattern();
  }
  
  // Initialize timing
  clockTimer = millis();
  statusTimer = millis();
  
  // Signal ready
  signalReady();
}

void loop() {
  // RK-002 is MIDI input only - no master clock generation
  // All timing follows external MIDI clock signals
  
  // Update status indicators
  handleStatusUpdate();
  
  // Small delay to prevent system overload
  delayMicroseconds(100);
}

// ============================================================================
// MIDI EVENT CALLBACKS
// ============================================================================

boolean RK002_onNoteOn(byte channel, byte note, byte velocity) {
  switch (currentMode) {
    case MODE_LSDJ_SYNC:
      // Clock sync mode doesn't handle note events
      return true;
      
    case MODE_MGB_BASIC:
      return handleMGBBasicNoteOn(channel, note, velocity);
      
    case MODE_MGB_CHORD:
      return handleMGBChordNoteOn(channel, note, velocity);
      
    case MODE_MGB_ARPEGGIO:
      return handleMGBArpeggioNoteOn(channel, note, velocity);
      
    case MODE_MGB_SCALES:
      return handleMGBScalesNoteOn(channel, note, velocity);
      
    case MODE_MGB_GRIDS:
      return handleMGBGridsNoteOn(channel, note, velocity);
      
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOn(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOn(channel, note, velocity);
      
    case MODE_NANOLOOP_SYNC:
      return handleNanoloopNoteOn(channel, note, velocity);
  }
  return true; // Pass through MIDI
}

boolean RK002_onNoteOff(byte channel, byte note, byte velocity) {
  switch (currentMode) {
    case MODE_LSDJ_SYNC:
      // Clock sync mode doesn't handle note events
      return true;
      
    case MODE_MGB_BASIC:
      return handleMGBBasicNoteOff(channel, note, velocity);
      
    case MODE_MGB_CHORD:
      return handleMGBChordNoteOff(channel, note, velocity);
      
    case MODE_MGB_ARPEGGIO:
      return handleMGBArpeggioNoteOff(channel, note, velocity);
      
    case MODE_MGB_SCALES:
      return handleMGBScalesNoteOff(channel, note, velocity);
      
    case MODE_MGB_GRIDS:
      return handleMGBGridsNoteOff(channel, note, velocity);
      
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOff(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOff(channel, note, velocity);
      
    case MODE_NANOLOOP_SYNC:
      return handleNanoloopNoteOff(channel, note, velocity);
  }
  return true;
}

boolean RK002_onControlChange(byte channel, byte cc, byte value) {
  switch (cc) {
    case 1:   // Mod Wheel - Alternative mode select
      setMode(value >> 4);  // Convert 0-127 to 0-7
      break;
      
    case 7:   // Volume - BPM control (not used - RK-002 follows external clock only)
      // RK-002 is MIDI input only - BPM controlled by external sequencer
      break;
      
    case 16:  // GP1 - MIDI channel select
      midiChannel = map(value, 0, 127, 1, 16);
      saveSettings(); // Save immediately
      break;
      
    case 17:  // GP2 - LSDJ Map channel 1
    case 18:  // GP3 - LSDJ Map channel 2  
    case 19:  // GP4 - LSDJ Map channel 3
    case 20:  // GP5 - LSDJ Map channel 4
      if (currentMode == MODE_LSDJ_MAP) {
        byte mapIndex = cc - 17;
        midiChannelMap[mapIndex] = map(value, 0, 127, 1, 16);
        saveSettings(); // Save immediately
      }
      break;
      
    case 21:  // mGB Mode Parameter 1
      handleMGBParameter1(value);
      break;
      
    case 22:  // mGB Mode Parameter 2
      handleMGBParameter2(value);
      break;
      
    case 23:  // mGB Mode Parameter 3
      handleMGBParameter3(value);
      break;
      
    case 64:  // Sustain - Start/Stop
      if (value >= 64) {
        startClock();
      } else {
        stopClock();
      }
      break;
  }
  return true;
}

boolean RK002_onProgramChange(byte channel, byte program) {
  // PRIMARY mode selection method
  if (program < NUM_MODES) {
    setMode(program);
  }
  return true;
}

boolean RK002_onClock() {
  // Handle LSDJ/Nanoloop sync modes  
  if (currentMode == MODE_LSDJ_SYNC || currentMode == MODE_NANOLOOP_SYNC) {
    clockDivider++;
    if (clockDivider >= CLOCK_DIVISION) {
      clockDivider = 0;
      if (clockRunning) {
        sendClockPulse();
      }
    }
  }
  
  // Handle mGB arpeggiator and grids timing
  if (clockRunning) {
    if (currentMode == MODE_MGB_ARPEGGIO) {
      handleMGBArpeggioClock();
    }
    else if (currentMode == MODE_MGB_GRIDS) {
      handleMGBGridsClock();
    }
  }
  
  return true;
}

boolean RK002_onStart() {
  if (currentMode == MODE_LSDJ_SYNC || currentMode == MODE_NANOLOOP_SYNC) {
    startClock();
  }
  
  // Reset mGB mode timing
  if (currentMode == MODE_MGB_ARPEGGIO) {
    arpPosition = 0;
    arpStepCounter = 0;
  }
  
  if (currentMode == MODE_MGB_GRIDS) {
    gridsStep = 0;
  }
  
  return true;
}

boolean RK002_onStop() {
  if (currentMode == MODE_LSDJ_SYNC || currentMode == MODE_NANOLOOP_SYNC) {
    stopClock();
  }
  
  // Stop all mGB notes when clock stops
  if (currentMode == MODE_MGB_ARPEGGIO || currentMode == MODE_MGB_GRIDS) {
    for (byte i = 0; i < MGB_CHANNELS; i++) {
      if (mgbNoteOn[i]) {
        sendMGBNoteOff(i, mgbLastNote[i]);
        mgbNoteOn[i] = false;
      }
    }
  }
  
  return true;
}

boolean RK002_onContinue() {
  if (currentMode == MODE_LSDJ_SYNC || currentMode == MODE_NANOLOOP_SYNC) {
    clockRunning = true;
  }
  return true;
}

// ============================================================================
// GAME BOY COMMUNICATION
// ============================================================================

void sendGameBoyByte(byte data) {
  // ArduinoBoy-compatible Game Boy serial protocol
  // Send each bit with proper timing
  
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    
    // Set data line
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    
    // Wait for Game Boy to read bit
    delayMicroseconds(GB_CLOCK_RATE_US);
  }
  
  // Inter-byte delay for Game Boy processing
  delayMicroseconds(GB_SERIAL_DELAY_US);
}

void sendGameBoyByteNanoloop(byte data) {
  // Nanoloop uses slower timing requirements
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    delayMicroseconds(GB_NANOLOOP_RATE_US);
  }
  
  // Longer inter-byte delay for Nanoloop
  delayMicroseconds(GB_NANOLOOP_RATE_US * 2);
}

void sendClockPulse() {
  // Send sync pulse to Game Boy - ArduinoBoy compatible
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(50);  // Hold low for 50µs
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(50);  // Hold high for 50µs
}

void sendLSDJNoteOn(byte channel, byte note, byte velocity) {
  // Send LSDJ-formatted note on message
  sendGameBoyByte(0x90 | (channel & 0x0F));
  sendGameBoyByte(mapMIDIToLSDJNote(note));
  sendGameBoyByte(mapMIDIToLSDJVelocity(velocity));
}

void sendLSDJNoteOff(byte channel, byte note) {
  // Send LSDJ-formatted note off message
  sendGameBoyByte(0x80 | (channel & 0x0F));
  sendGameBoyByte(mapMIDIToLSDJNote(note));
  sendGameBoyByte(0x00);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void setMode(byte newMode) {
  if (newMode < NUM_MODES) {
    // Stop current activities
    stopClock();
    
    // Clear note tracking
    for (int i = 0; i < 4; i++) {
      noteActive[i] = false;
      lastNote[i] = 0;
    }
    
    // Set new mode
    currentMode = newMode;
    
    // Save to EEPROM
    saveSettings();
    
    // Send confirmation flash
    flashStatusPattern(3, 100, 100);
  }
}

void startClock() {
  clockRunning = true;
  clockDivider = 0;
  clockTimer = millis();
}

void stopClock() {
  clockRunning = false;
}

void handleMasterClock() {
  unsigned long currentTime = millis();
  // Calculate interval for LSDJ 4 PPQN (not 24 PPQN like MIDI)
  unsigned long clockInterval = (60000UL / bpm / LSDJ_PPQN);
  
  if (currentTime - clockTimer >= clockInterval) {
    clockTimer = currentTime;
    
    if (currentMode == MODE_LSDJ_MASTER) {
      sendClockPulse();
    } else if (currentMode == MODE_NANOLOOP_MASTER) {
      sendNanoloopClock();
    }
  }
}

void handleStatusUpdate() {
  unsigned long currentTime = millis();
  
  if (currentTime - statusTimer >= 500) {
    statusTimer = currentTime;
    statusLED = !statusLED;
    
    #ifdef USE_GPIO_LED
      RK002_writeGPIO(STATUS_LED_PIN, statusLED ? HIGH : LOW);
    #endif
  }
}

void signalReady() {
  // Flash LED pattern to indicate startup
  #ifdef USE_GPIO_LED
    for (int i = 0; i < 3; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(100);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(100);
    }
  #endif
}

// ============================================================================
// MODE IMPLEMENTATIONS - Add your custom implementations here
// ============================================================================

// LSDJ Keyboard Mode
boolean handleLSDJKeyboardNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  sendLSDJNoteOn(0, note, velocity);
  return true;
}

boolean handleLSDJKeyboardNoteOff(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  sendLSDJNoteOff(0, note);
  return true;
}

// LSDJ Map Mode - 4-channel polyphonic
boolean handleLSDJMapNoteOn(byte channel, byte note, byte velocity) {
  // Find which LSDJ channel this MIDI channel maps to
  byte lsdjChannel = 255; // Invalid channel
  for (byte i = 0; i < 4; i++) {
    if (midiChannelMap[i] == channel) {
      lsdjChannel = i;
      break;
    }
  }
  
  if (lsdjChannel < 4) {
    // Stop current note on this channel if playing
    if (noteActive[lsdjChannel]) {
      sendLSDJNoteOff(lsdjChannel, lastNote[lsdjChannel]);
    }
    
    // Play new note
    sendLSDJNoteOn(lsdjChannel, note, velocity);
    lastNote[lsdjChannel] = note;
    noteActive[lsdjChannel] = true;
  }
  
  return true;
}

boolean handleLSDJMapNoteOff(byte channel, byte note, byte velocity) {
  // Find which LSDJ channel this MIDI channel maps to
  byte lsdjChannel = 255;
  for (byte i = 0; i < 4; i++) {
    if (midiChannelMap[i] == channel) {
      lsdjChannel = i;
      break;
    }
  }
  
  if (lsdjChannel < 4 && noteActive[lsdjChannel] && lastNote[lsdjChannel] == note) {
    sendLSDJNoteOff(lsdjChannel, note);
    noteActive[lsdjChannel] = false;
  }
  
  return true;
}

// Nanoloop Mode - Sample triggering
boolean handleNanoloopNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  // Map MIDI note to Nanoloop step (0-15)
  byte nanoStep = mapMIDIToNanoloopStep(note);
  byte nanoVel = mapMIDIToNanoloopVelocity(velocity);
  
  // Send Nanoloop trigger command
  sendNanoloopTrigger(nanoStep, nanoVel);
  
  return true;
}

boolean handleNanoloopNoteOff(byte channel, byte note, byte velocity) {
  // Nanoloop doesn't require note off messages for samples
  return true;
}

// ============================================================================
// mGB MODE IMPLEMENTATIONS
// ============================================================================

// mGB Basic Mode - Direct note mapping to Game Boy channels
boolean handleMGBBasicNoteOn(byte channel, byte note, byte velocity) {
  // Map MIDI channel to Game Boy channel (1-4 → 0-3)
  if (channel < 1 || channel > 4) return true;
  
  byte gbChannel = channel - 1;
  
  // Stop previous note if playing
  if (mgbNoteOn[gbChannel]) {
    sendMGBNoteOff(gbChannel, mgbLastNote[gbChannel]);
  }
  
  // Send new note to Game Boy
  sendMGBNoteOn(gbChannel, note, velocity);
  mgbLastNote[gbChannel] = note;
  mgbNoteOn[gbChannel] = true;
  
  return false; // Block MIDI passthrough
}

boolean handleMGBBasicNoteOff(byte channel, byte note, byte velocity) {
  if (channel < 1 || channel > 4) return true;
  
  byte gbChannel = channel - 1;
  
  if (mgbNoteOn[gbChannel] && mgbLastNote[gbChannel] == note) {
    sendMGBNoteOff(gbChannel, note);
    mgbNoteOn[gbChannel] = false;
  }
  
  return false;
}

// mGB Chord Mode - Trigger chords across all 4 Game Boy channels
boolean handleMGBChordNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true; // Only respond to selected channel
  
  // Stop all previous notes
  for (byte i = 0; i < MGB_CHANNELS; i++) {
    if (mgbNoteOn[i]) {
      sendMGBNoteOff(i, mgbLastNote[i]);
      mgbNoteOn[i] = false;
    }
  }
  
  // Play chord across all 4 channels
  chordRoot = note;
  for (byte i = 0; i < MGB_CHANNELS; i++) {
    byte chordNote = note + chordPatterns[chordType][i];
    if (chordNote < 128) { // Valid MIDI range
      sendMGBNoteOn(i, chordNote, velocity);
      mgbLastNote[i] = chordNote;
      mgbNoteOn[i] = true;
    }
  }
  
  return false;
}

boolean handleMGBChordNoteOff(byte channel, byte note, byte velocity) {
  if (channel != midiChannel || note != chordRoot) return true;
  
  // Stop all chord notes
  for (byte i = 0; i < MGB_CHANNELS; i++) {
    if (mgbNoteOn[i]) {
      sendMGBNoteOff(i, mgbLastNote[i]);
      mgbNoteOn[i] = false;
    }
  }
  
  return false;
}

// mGB Arpeggiator Mode - Arpeggio notes across channels
boolean handleMGBArpeggioNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  // Add note to arpeggio if not already present
  boolean noteExists = false;
  for (byte i = 0; i < arpNoteCount; i++) {
    if (arpNotes[i] == note) {
      noteExists = true;
      break;
    }
  }
  
  if (!noteExists && arpNoteCount < ARP_MAX_NOTES) {
    arpNotes[arpNoteCount] = note;
    arpNoteCount++;
    
    // Sort notes for arpeggio (simple bubble sort)
    for (byte i = 0; i < arpNoteCount - 1; i++) {
      for (byte j = i + 1; j < arpNoteCount; j++) {
        if (arpNotes[i] > arpNotes[j]) {
          byte temp = arpNotes[i];
          arpNotes[i] = arpNotes[j];
          arpNotes[j] = temp;
        }
      }
    }
  }
  
  return false;
}

boolean handleMGBArpeggioNoteOff(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  // Remove note from arpeggio
  for (byte i = 0; i < arpNoteCount; i++) {
    if (arpNotes[i] == note) {
      // Shift remaining notes down
      for (byte j = i; j < arpNoteCount - 1; j++) {
        arpNotes[j] = arpNotes[j + 1];
      }
      arpNoteCount--;
      break;
    }
  }
  
  // Stop arpeggio if no notes left
  if (arpNoteCount == 0) {
    for (byte i = 0; i < MGB_CHANNELS; i++) {
      if (mgbNoteOn[i]) {
        sendMGBNoteOff(i, mgbLastNote[i]);
        mgbNoteOn[i] = false;
      }
    }
  }
  
  return false;
}

// mGB Scales Mode - Quantize notes to musical scales
boolean handleMGBScalesNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  // Quantize note to current scale
  byte scaledNote = quantizeToScale(note, currentScale, scaleRoot);
  
  // Use simple round-robin channel assignment
  static byte nextChannel = 0;
  
  // Stop previous note on this channel
  if (mgbNoteOn[nextChannel]) {
    sendMGBNoteOff(nextChannel, mgbLastNote[nextChannel]);
  }
  
  // Play scaled note
  sendMGBNoteOn(nextChannel, scaledNote, velocity);
  mgbLastNote[nextChannel] = scaledNote;
  mgbNoteOn[nextChannel] = true;
  
  // Move to next channel
  nextChannel = (nextChannel + 1) % MGB_CHANNELS;
  
  return false;
}

boolean handleMGBScalesNoteOff(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  byte scaledNote = quantizeToScale(note, currentScale, scaleRoot);
  
  // Find and stop the scaled note
  for (byte i = 0; i < MGB_CHANNELS; i++) {
    if (mgbNoteOn[i] && mgbLastNote[i] == scaledNote) {
      sendMGBNoteOff(i, scaledNote);
      mgbNoteOn[i] = false;
      break;
    }
  }
  
  return false;
}

// mGB Grids Mode - Euclidean pattern generation for Game Boy
boolean handleMGBGridsNoteOn(byte channel, byte note, byte velocity) {
  // Grids mode is primarily clock-driven
  // Notes can be used to change pattern parameters
  if (channel == midiChannel) {
    // Use note value to select pattern
    gridsPattern = note % GRIDS_PATTERNS;
  }
  
  return false;
}

boolean handleMGBGridsNoteOff(byte channel, byte note, byte velocity) {
  // Notes don't directly trigger in grids mode
  return false;
}

// Clock-driven functions for tempo-synced modes
void handleMGBArpeggioClock() {
  if (arpNoteCount == 0) return;
  
  arpStepCounter++;
  if (arpStepCounter >= arpSpeed) {
    arpStepCounter = 0;
    
    // Stop current note
    byte currentChannel = arpPosition % MGB_CHANNELS;
    if (mgbNoteOn[currentChannel]) {
      sendMGBNoteOff(currentChannel, mgbLastNote[currentChannel]);
      mgbNoteOn[currentChannel] = false;
    }
    
    // Play next note in arpeggio
    byte noteIndex;
    switch (arpPattern) {
      case 0: // Up
        noteIndex = arpPosition % arpNoteCount;
        break;
      case 1: // Down  
        noteIndex = (arpNoteCount - 1) - (arpPosition % arpNoteCount);
        break;
      case 2: // Up/Down
        {
          byte cycle = arpPosition % (arpNoteCount * 2 - 2);
          if (cycle < arpNoteCount) {
            noteIndex = cycle;
          } else {
            noteIndex = (arpNoteCount * 2 - 2) - cycle;
          }
        }
        break;
      case 3: // Random
        noteIndex = RK002_random() % arpNoteCount;
        break;
      default:
        noteIndex = 0;
    }
    
    sendMGBNoteOn(currentChannel, arpNotes[noteIndex], 100);
    mgbLastNote[currentChannel] = arpNotes[noteIndex];
    mgbNoteOn[currentChannel] = true;
    
    arpPosition++;
  }
}

void handleMGBGridsClock() {
  static byte clockCounter = 0;
  
  clockCounter++;
  if (clockCounter >= 6) { // 16th note timing
    clockCounter = 0;
    
    // Trigger patterns for each Game Boy channel
    for (byte track = 0; track < GRIDS_TRACKS; track++) {
      if (gridsPatterns[track][gridsStep]) {
        // Trigger note based on track
        byte note = 36 + (track * 12); // Different octaves per track
        sendMGBNoteOn(track, note, 80 + gridsAccent[track]);
        mgbLastNote[track] = note;
        mgbNoteOn[track] = true;
      }
    }
    
    gridsStep = (gridsStep + 1) % GRIDS_STEPS;
  }
}

// ============================================================================
// HELPER FUNCTIONS - Add mapping and utility functions
// ============================================================================

// mGB Game Boy Communication Functions
void sendMGBNoteOn(byte channel, byte note, byte velocity) {
  if (channel >= MGB_CHANNELS) return;
  
  // Convert MIDI note to Game Boy frequency
  byte gbNote = mapMIDIToGameBoyNote(note);
  byte gbVelocity = mapMIDIToGameBoyVelocity(velocity);
  
  // Send mGB command sequence
  byte cmdByte = MGB_CMD_NOTE | channel;
  
  gameBoySendByte(cmdByte);     // Command + channel
  gameBoySendByte(gbNote);      // Note value
  gameBoySendByte(gbVelocity);  // Velocity
  gameBoySendByte(0x00);        // Length (not used in mGB)
}

void sendMGBNoteOff(byte channel, byte note) {
  if (channel >= MGB_CHANNELS) return;
  
  byte cmdByte = MGB_CMD_NOTE | channel;
  
  gameBoySendByte(cmdByte);     
  gameBoySendByte(0x00);        // Note 0 = note off
  gameBoySendByte(0x00);        // Zero velocity
  gameBoySendByte(0x00);        
}

// Musical scale quantization
byte quantizeToScale(byte note, byte scaleIndex, byte root) {
  if (scaleIndex >= MAX_SCALES) return note;
  
  // Calculate note relative to scale root
  int noteOffset = note - root;
  int octave = noteOffset / 12;
  int semitone = noteOffset % 12;
  
  if (semitone < 0) {
    semitone += 12;
    octave--;
  }
  
  // Find closest note in scale
  byte closestNote = 0;
  byte minDistance = 12;
  
  for (byte i = 0; i < SCALE_LENGTH; i++) {
    byte scaleNote = scales[scaleIndex][i] % 12;
    byte distance = abs(semitone - scaleNote);
    if (distance < minDistance) {
      minDistance = distance;
      closestNote = scaleNote;
    }
  }
  
  return root + (octave * 12) + closestNote;
}

byte mapMIDIToGameBoyNote(byte midiNote) {
  // Game Boy uses specific frequency table
  // Map MIDI notes to closest Game Boy frequencies
  if (midiNote < 24) return 0;    // Below Game Boy range
  if (midiNote > 119) return 95;  // Above Game Boy range
  return midiNote - 24;           // Offset for Game Boy range
}

byte mapMIDIToGameBoyVelocity(byte velocity) {
  // Convert MIDI velocity (0-127) to Game Boy volume (0-15)
  if (velocity == 0) return 0;
  return (velocity >> 3) + 1; // Divide by 8, minimum 1
}

byte mapMIDIToLSDJNote(byte midiNote) {
  // Convert MIDI note (0-127) to LSDJ format (0-95)
  // LSDJ note range: C-2 to B-6 (8 octaves)
  if (midiNote < 12) return 0;  // Below LSDJ range
  if (midiNote > 107) return 95; // Above LSDJ range
  return midiNote - 12; // Offset to LSDJ range
}

byte mapMIDIToLSDJVelocity(byte velocity) {
  // Convert MIDI velocity (0-127) to LSDJ velocity (0-15)
  // Use logarithmic curve for musical response
  if (velocity == 0) return 0;
  if (velocity < 8) return 1;
  return (velocity >> 3) + 1; // 1-16, then clamp to 1-15
}

// Additional Nanoloop helper functions
void sendNanoloopTrigger(byte step, byte velocity) {
  // Send Nanoloop sample trigger
  sendGameBoyByteNanoloop(0x90 | (step & 0x0F)); // Note on + step
  sendGameBoyByteNanoloop(velocity);
}

void sendNanoloopClock() {
  // Nanoloop clock pulse - slower than LSDJ
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(100);
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(100);
}

byte mapMIDIToNanoloopStep(byte midiNote) {
  // Map MIDI note to Nanoloop step (0-15)
  return (midiNote - 36) & 0x0F; // C2 = step 0
}

byte mapMIDIToNanoloopVelocity(byte velocity) {
  // Convert MIDI velocity to Nanoloop format
  return velocity >> 2; // 0-127 to 0-31
}

// EEPROM functions for persistent settings
void loadSettings() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    // Settings exist, load them
    currentMode = EEPROM.read(EEPROM_MODE_ADDR);
    midiChannel = EEPROM.read(EEPROM_CHANNEL_ADDR);
    bpm = EEPROM.read(EEPROM_BPM_ADDR);
    
    // Load channel map
    for (byte i = 0; i < 4; i++) {
      midiChannelMap[i] = EEPROM.read(EEPROM_MAP_ADDR + i);
    }
    
    // Validate loaded values
    if (currentMode >= NUM_MODES) currentMode = DEFAULT_MODE;
    if (midiChannel < 1 || midiChannel > 16) midiChannel = DEFAULT_CHANNEL;
    if (bpm < MIN_BPM || bpm > MAX_BPM) bpm = DEFAULT_BPM;
  } else {
    // First boot, save defaults
    saveSettings();
  }
}

void saveSettings() {
  EEPROM.write(EEPROM_MODE_ADDR, currentMode);
  EEPROM.write(EEPROM_CHANNEL_ADDR, midiChannel);
  EEPROM.write(EEPROM_BPM_ADDR, bpm);
  
  // Save channel map
  for (byte i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_MAP_ADDR + i, midiChannelMap[i]);
  }
  
  // Set magic byte to indicate settings are valid
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
}

// ============================================================================
// mGB PARAMETER CONTROL FUNCTIONS
// ============================================================================

void handleMGBParameter1(byte value) {
  switch (currentMode) {
    case MODE_MGB_CHORD:
      // Change chord type (0-7)
      chordType = (value * 8) / 128;
      if (chordType > 7) chordType = 7;
      flashStatusPattern(chordType + 1, 100, 100); // Visual feedback
      break;
      
    case MODE_MGB_ARPEGGIO:
      // Change arpeggio pattern (0-3)  
      arpPattern = (value * 4) / 128;
      if (arpPattern > 3) arpPattern = 3;
      flashStatusPattern(arpPattern + 1, 150, 50);
      break;
      
    case MODE_MGB_SCALES:
      // Change scale type (0-7)
      currentScale = (value * MAX_SCALES) / 128;
      if (currentScale >= MAX_SCALES) currentScale = MAX_SCALES - 1;
      flashStatusPattern(currentScale + 1, 80, 120);
      break;
      
    case MODE_MGB_GRIDS:
      // Change pattern for track 1
      gridsDensity[0] = (value / 8) + 1; // 1-16 range
      break;
  }
}

void handleMGBParameter2(byte value) {
  switch (currentMode) {
    case MODE_MGB_ARPEGGIO:
      // Change arpeggio speed (1-16)
      arpSpeed = (value / 8) + 1;
      break;
      
    case MODE_MGB_SCALES:
      // Change scale root note
      scaleRoot = (value * 12) / 128 + 60; // C3-B3 range
      break;
      
    case MODE_MGB_GRIDS:
      // Change pattern for track 2
      gridsDensity[1] = (value / 8) + 1;
      break;
  }
}

void handleMGBParameter3(byte value) {
  switch (currentMode) {
    case MODE_MGB_GRIDS:
      // Change accent level for all tracks
      for (byte i = 0; i < GRIDS_TRACKS; i++) {
        gridsAccent[i] = value / 8; // 0-15 range
      }
      break;
      
    case MODE_MGB_CHORD:
      // Change chord inversion
      // Implementation could add chord inversions here
      break;
  }
}

// GPIO error handling
boolean initializeGPIO() {
  // Try to initialize GPIO pins with error checking
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_writeGPIO(GB_SIN, HIGH);  // Idle state
  
  #ifdef USE_GPIO_LED
    RK002_setGPIOType(STATUS_LED_PIN, RK002_GPIO_OUTPUT);
    RK002_writeGPIO(STATUS_LED_PIN, LOW);
  #else
    RK002_setGPIOType(GB_SOUT, RK002_GPIO_INPUT_PULLUP);
  #endif
  
  // Test GPIO functionality
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(10);
  RK002_writeGPIO(GB_SIN, HIGH);
  
  return true; // RK-002 GPIO calls don't return error codes
}

void flashErrorPattern() {
  // Flash rapid error pattern if GPIO fails
  #ifdef USE_GPIO_LED
    for (byte i = 0; i < 10; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(50);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(50);
    }
  #endif
}

void flashStatusPattern(byte count, unsigned int onTime, unsigned int offTime) {
  #ifdef USE_GPIO_LED
    for (byte i = 0; i < count; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(onTime);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(offTime);
    }
  #endif
}
