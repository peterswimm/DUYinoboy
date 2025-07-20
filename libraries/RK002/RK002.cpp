/*
 * RK002.cpp - Retrokits RK-002 Smart MIDI Cable Library Implementation
 * 
 * This is a minimal implementation for compilation testing.
 * For production use, install the official RK-002 board package
 * or use the DUY Web Editor at https://duy.retrokits.com/
 */

#include "RK002.h"

// ============================================================================
// GPIO FUNCTIONS IMPLEMENTATION
// ============================================================================

void RK002_setGPIOType(byte pin, byte type) {
  // Stub implementation for compilation
  // On real hardware, this configures GPIO pin direction and pull-up
  if (pin == 1 || pin == 2) {
    switch (type) {
      case RK002_GPIO_OUTPUT:
        pinMode(pin + 2, OUTPUT); // Map to Arduino pins 3,4 for simulation
        break;
      case RK002_GPIO_INPUT:
        pinMode(pin + 2, INPUT);
        break;
      case RK002_GPIO_INPUT_PULLUP:
        pinMode(pin + 2, INPUT_PULLUP);
        break;
    }
  }
}

void RK002_writeGPIO(byte pin, byte value) {
  // Stub implementation for compilation
  // On real hardware, this writes to GPIO pins
  if (pin == 1 || pin == 2) {
    digitalWrite(pin + 2, value ? HIGH : LOW); // Map to Arduino pins 3,4
  }
}

byte RK002_readGPIO(byte pin) {
  // Stub implementation for compilation
  // On real hardware, this reads from GPIO pins
  if (pin == 1 || pin == 2) {
    return digitalRead(pin + 2) ? 1 : 0; // Map to Arduino pins 3,4
  }
  return 0;
}

// ============================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// ============================================================================

byte RK002_random() {
  return random(256);
}

unsigned int RK002_random16() {
  return random(65536);
}

void RK002_sendNoteOn(byte channel, byte note, byte velocity) {
  // Note: RK-002 hardware is MIDI INPUT ONLY
  // This function is a stub for compatibility
  // Real hardware cannot send MIDI output
}

void RK002_sendNoteOff(byte channel, byte note, byte velocity) {
  // Note: RK-002 hardware is MIDI INPUT ONLY  
  // This function is a stub for compatibility
  // Real hardware cannot send MIDI output
}

void RK002_sendControlChange(byte channel, byte cc, byte value) {
  // Note: RK-002 hardware is MIDI INPUT ONLY
  // This function is a stub for compatibility
  // Real hardware cannot send MIDI output
}

void RK002_sendProgramChange(byte channel, byte program) {
  // Note: RK-002 hardware is MIDI INPUT ONLY
  // This function is a stub for compatibility  
  // Real hardware cannot send MIDI output
}

void RK002_delayMicroseconds(unsigned int us) {
  delayMicroseconds(us);
}

unsigned long RK002_millis() {
  return millis();
}

unsigned long RK002_micros() {
  return micros();
}

// ============================================================================
// ARDUINO COMPATIBILITY LAYER
// ============================================================================

// Global Serial instance
SerialClass Serial;

// Arduino core function implementations (when not using real Arduino environment)
#ifndef ARDUINO_ARCH_AVR
void pinMode(uint8_t pin, uint8_t mode) {
    // Stub implementation for compilation testing
}

void digitalWrite(uint8_t pin, uint8_t value) {
    // Stub implementation for compilation testing
}

int digitalRead(uint8_t pin) {
    // Stub implementation for compilation testing
    return LOW;
}

void delay(uint32_t ms) {
    // Stub implementation for compilation testing
}

uint32_t millis() {
    // Stub implementation for compilation testing
    return 0;
}

void delayMicroseconds(uint32_t us) {
    // Stub implementation for compilation testing
}

uint32_t micros() {
    // Stub implementation for compilation testing
    return 0;
}

long random(long min, long max) {
    // Stub implementation for compilation testing
    return min;
}

long random(long max) {
    // Stub implementation for compilation testing
    return 0;
}

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh) {
    // Arduino map function implementation
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

int abs(int value) {
    // Absolute value function
    return value < 0 ? -value : value;
}
#endif
