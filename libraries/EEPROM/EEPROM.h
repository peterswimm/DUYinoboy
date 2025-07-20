#ifndef EEPROM_h
#define EEPROM_h

// Basic type definitions for compatibility
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef uint16_t 
typedef unsigned short uint16_t;
#endif

class EEPROMClass {
public:
    uint8_t read(int address) {
        // Stub implementation for compilation testing
        return 0;
    }
    
    void write(int address, uint8_t value) {
        // Stub implementation for compilation testing
    }
    
    void update(int address, uint8_t value) {
        // Stub implementation for compilation testing
    }
    
    uint16_t length() {
        return 1024; // Typical EEPROM size
    }
    
    uint8_t& operator[](int address) {
        static uint8_t dummy = 0;
        return dummy;
    }
};

extern EEPROMClass EEPROM;

#endif
