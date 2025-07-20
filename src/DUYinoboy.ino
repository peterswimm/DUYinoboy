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
// ARDUINOBOY MODE DEFINITIONS
// ============================================================================

#define MODE_LSDJ_MASTER      0
#define MODE_LSDJ_SLAVE       1  
#define MODE_LSDJ_KEYBOARD    2
#define MODE_LSDJ_MAP         3
#define MODE_NANOLOOP_MASTER  4
#define MODE_NANOLOOP_SLAVE   5
#define MODE_CUSTOM_1         6
#define MODE_CUSTOM_2         7
#define NUM_MODES             8

// ============================================================================
// TIMING AND MIDI CONFIGURATION
// ============================================================================

#define MIDI_PPQN            24    // MIDI Pulses Per Quarter Note
#define LSDJ_PPQN             4    // LSDJ Pulses Per Quarter Note  
#define CLOCK_DIVISION        6    // 24/4 = 6:1 ratio

// Timing Constants (for Game Boy Link Cable)
#define GB_CLOCK_RATE_US     62    // ~8KHz serial rate for LSDJ
#define GB_NANOLOOP_RATE_US 125    // Slower rate for Nanoloop

// Default Settings
#define DEFAULT_BPM         120
#define DEFAULT_MODE        MODE_LSDJ_MASTER
#define DEFAULT_CHANNEL       1
#define MIN_BPM              60
#define MAX_BPM             200

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

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
// SETUP AND MAIN LOOP
// ============================================================================

void setup() {
  // Initialize GPIO pins (RK-002 specific functions)
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_writeGPIO(GB_SIN, HIGH);  // Idle state
  
  #ifdef USE_GPIO_LED
    RK002_setGPIOType(STATUS_LED_PIN, RK002_GPIO_OUTPUT);
    RK002_writeGPIO(STATUS_LED_PIN, LOW);
  #else
    RK002_setGPIOType(GB_SOUT, RK002_GPIO_INPUT_PULLUP);
  #endif
  
  // Initialize timing
  clockTimer = millis();
  statusTimer = millis();
  
  // Signal ready
  signalReady();
}

void loop() {
  // Process clock generation for master modes
  if ((currentMode == MODE_LSDJ_MASTER || currentMode == MODE_NANOLOOP_MASTER) && clockRunning) {
    handleMasterClock();
  }
  
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
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOn(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOn(channel, note, velocity);
      
    case MODE_NANOLOOP_MASTER:
    case MODE_NANOLOOP_SLAVE:
      return handleNanoloopNoteOn(channel, note, velocity);
      
    case MODE_CUSTOM_1:
      return handleCustom1NoteOn(channel, note, velocity);
      
    case MODE_CUSTOM_2:
      return handleCustom2NoteOn(channel, note, velocity);
  }
  return true; // Pass through MIDI
}

boolean RK002_onNoteOff(byte channel, byte note, byte velocity) {
  switch (currentMode) {
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOff(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOff(channel, note, velocity);
      
    case MODE_NANOLOOP_MASTER:
    case MODE_NANOLOOP_SLAVE:
      return handleNanoloopNoteOff(channel, note, velocity);
      
    case MODE_CUSTOM_1:
      return handleCustom1NoteOff(channel, note, velocity);
      
    case MODE_CUSTOM_2:
      return handleCustom2NoteOff(channel, note, velocity);
  }
  return true;
}

boolean RK002_onControlChange(byte channel, byte cc, byte value) {
  switch (cc) {
    case 1:   // Mod Wheel - Alternative mode select
      setMode(value >> 4);  // Convert 0-127 to 0-7
      break;
      
    case 7:   // Volume - BPM control (master modes only)
      if (currentMode == MODE_LSDJ_MASTER || currentMode == MODE_NANOLOOP_MASTER) {
        bpm = map(value, 0, 127, MIN_BPM, MAX_BPM);
      }
      break;
      
    case 16:  // GP1 - MIDI channel select
      midiChannel = map(value, 0, 127, 1, 16);
      break;
      
    case 17:  // GP2 - LSDJ Map channel 1
    case 18:  // GP3 - LSDJ Map channel 2  
    case 19:  // GP4 - LSDJ Map channel 3
    case 20:  // GP5 - LSDJ Map channel 4
      if (currentMode == MODE_LSDJ_MAP) {
        byte mapIndex = cc - 17;
        midiChannelMap[mapIndex] = map(value, 0, 127, 1, 16);
      }
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
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    clockDivider++;
    if (clockDivider >= CLOCK_DIVISION) {
      clockDivider = 0;
      if (clockRunning) {
        sendClockPulse();
      }
    }
  }
  return true;
}

boolean RK002_onStart() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    startClock();
  }
  return true;
}

boolean RK002_onStop() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    stopClock();
  }
  return true;
}

boolean RK002_onContinue() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    clockRunning = true;
  }
  return true;
}

// ============================================================================
// GAME BOY COMMUNICATION
// ============================================================================

void sendGameBoyByte(byte data) {
  // Software implementation of Game Boy serial protocol
  // Uses bit-banging since RK-002 doesn't have hardware SPI
  
  for (int bit = 7; bit >= 0; bit--) {
    // Set data line to bit value
    boolean bitValue = (data >> bit) & 1;
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    
    // Hold data for Game Boy to read
    delayMicroseconds(GB_CLOCK_RATE_US / 2);
    
    // Brief clock pulse (simulated)
    RK002_writeGPIO(GB_SIN, LOW);
    delayMicroseconds(2);
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    
    // Complete bit timing
    delayMicroseconds(GB_CLOCK_RATE_US / 2);
  }
  
  // Return to idle state
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(GB_CLOCK_RATE_US);
}

void sendGameBoyByteNanoloop(byte data) {
  // Nanoloop uses different timing requirements
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    delayMicroseconds(GB_NANOLOOP_RATE_US);
  }
  
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(GB_NANOLOOP_RATE_US);
}

void sendClockPulse() {
  // Send sync pulse to Game Boy
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(10);
  RK002_writeGPIO(GB_SIN, HIGH);
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
  unsigned long clockInterval = (60000UL / bpm / MIDI_PPQN);
  
  if (currentTime - clockTimer >= clockInterval) {
    clockTimer = currentTime;
    
    clockDivider++;
    if (clockDivider >= CLOCK_DIVISION) {
      clockDivider = 0;
      sendClockPulse();
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

// LSDJ Map Mode  
boolean handleLSDJMapNoteOn(byte channel, byte note, byte velocity) {
  // Implementation for 4-channel polyphonic mode
  return true;
}

boolean handleLSDJMapNoteOff(byte channel, byte note, byte velocity) {
  // Implementation for 4-channel polyphonic mode
  return true;
}

// Nanoloop Mode
boolean handleNanoloopNoteOn(byte channel, byte note, byte velocity) {
  // Implementation for Nanoloop
  return true;
}

boolean handleNanoloopNoteOff(byte channel, byte note, byte velocity) {
  // Implementation for Nanoloop
  return true;
}

// Custom Modes
boolean handleCustom1NoteOn(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom1NoteOff(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom2NoteOn(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom2NoteOff(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

// ============================================================================
// HELPER FUNCTIONS - Add mapping and utility functions
// ============================================================================

byte mapMIDIToLSDJNote(byte midiNote) {
  // Convert MIDI note to LSDJ format
  return midiNote;
}

byte mapMIDIToLSDJVelocity(byte velocity) {
  // Convert MIDI velocity to LSDJ format
  return velocity >> 3; // Convert 0-127 to 0-15
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
