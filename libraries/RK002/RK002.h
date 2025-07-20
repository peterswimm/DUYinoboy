/*
 * RK002.h - Retrokits RK-002 Smart MIDI Cable Library
 * 
 * This is a minimal implementation for compilation testing.
 * For production use, install the official RK-002 board package
 * or use the DUY Web Editor at https://duy.retrokits.com/
 * 
 * Official Documentation:
 * - https://retrokits.com/rk002-duy/
 * - https://github.com/retrokits/RK002
 */

#ifndef RK002_H
#define RK002_H

// Check if we're in a real Arduino environment
#ifdef ARDUINO_H
#include <Arduino.h>
#else
// Define Arduino compatibility layer for compilation testing

// Basic type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t; 
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef unsigned char byte;
typedef bool boolean;

// Pin definitions
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Serial definitions 
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// Math definitions
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

// Forward declarations for Arduino core functions
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
void delay(uint32_t ms);
uint32_t millis();
void delayMicroseconds(uint32_t us);
uint32_t micros();
long random(long max);
long random(long min, long max);
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);
int abs(int value);

// Serial class stub
class SerialClass {
public:
    void begin(long baud) {}
    void print(const char* str) {}
    void println(const char* str) {}
    void print(int val) {}
    void println(int val) {}
    void print(int val, int format) {}
    void println(int val, int format) {}
    bool available() { return false; }
    int read() { return -1; }
};
extern SerialClass Serial;

#endif

// ============================================================================
// RK-002 FIRMWARE METADATA MACROS
// ============================================================================

#define RK002_DECLARE_INFO(name, author, version, uuid) \
  const char* RK002_INFO_NAME = name; \
  const char* RK002_INFO_AUTHOR = author; \
  const char* RK002_INFO_VERSION = version; \
  const char* RK002_INFO_UUID = uuid;

// ============================================================================
// GPIO DEFINITIONS
// ============================================================================

// GPIO Types
#define RK002_GPIO_OUTPUT         0
#define RK002_GPIO_INPUT          1  
#define RK002_GPIO_INPUT_PULLUP   2

// GPIO Pin Numbers (RK-002 has exactly 2 GPIO pins)
#define RK002_GPIO1               1
#define RK002_GPIO2               2

// ============================================================================
// MIDI CALLBACK FUNCTION PROTOTYPES
// ============================================================================

// Note that RK-002 callbacks return boolean:
// - true: pass MIDI message through
// - false: block MIDI message

// Note Messages
extern boolean RK002_onNoteOn(byte channel, byte note, byte velocity);
extern boolean RK002_onNoteOff(byte channel, byte note, byte velocity);

// Control Change Messages  
extern boolean RK002_onControlChange(byte channel, byte cc, byte value);

// Program Change Messages
extern boolean RK002_onProgramChange(byte channel, byte program);

// Real-Time Messages
extern boolean RK002_onClock();
extern boolean RK002_onStart();
extern boolean RK002_onStop();
extern boolean RK002_onContinue();

// System Messages
extern boolean RK002_onSystemExclusive(byte* data, byte length);

// ============================================================================
// GPIO FUNCTIONS
// ============================================================================

void RK002_setGPIOType(byte pin, byte type);
void RK002_writeGPIO(byte pin, byte value);
byte RK002_readGPIO(byte pin);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Random number generation
byte RK002_random();
unsigned int RK002_random16();

// MIDI Output (Note: RK-002 hardware is MIDI INPUT ONLY)
// These functions are provided for compatibility but will not work on actual hardware
void RK002_sendNoteOn(byte channel, byte note, byte velocity);
void RK002_sendNoteOff(byte channel, byte note, byte velocity);
void RK002_sendControlChange(byte channel, byte cc, byte value);
void RK002_sendProgramChange(byte channel, byte program);

// Timing functions
void RK002_delayMicroseconds(unsigned int us);
unsigned long RK002_millis();
unsigned long RK002_micros();

// ============================================================================
// WEAK CALLBACK IMPLEMENTATIONS (can be overridden)
// ============================================================================

// These provide default implementations that can be overridden by user code

boolean __attribute__((weak)) RK002_onNoteOn(byte channel, byte note, byte velocity) {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onNoteOff(byte channel, byte note, byte velocity) {
  return true; // Pass through by default  
}

boolean __attribute__((weak)) RK002_onControlChange(byte channel, byte cc, byte value) {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onProgramChange(byte channel, byte program) {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onClock() {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onStart() {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onStop() {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onContinue() {
  return true; // Pass through by default
}

boolean __attribute__((weak)) RK002_onSystemExclusive(byte* data, byte length) {
  return true; // Pass through by default
}

#endif // RK002_H
