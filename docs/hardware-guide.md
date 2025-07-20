# DUYinoboy Hardware Guide

Complete hardware setup guide for DUYinoboy - ArduinoBoy for RK-002.

## Required Hardware

### Core Components
- **Retrokits RK-002** Smart MIDI Cable
- **Game Boy** (any model: DMG, GBP, GBC, GBA)
- **Game Boy Link Cable** for modification
- **MIDI Interface** or USB-MIDI device

### Tools for Cable Modification
- Soldering iron (15-30W)
- Fine solder (0.6mm rosin core)  
- Wire strippers
- Multimeter
- Small screwdriver set
- Heat shrink tubing

## Game Boy Link Cable Pinout

The Game Boy Link Port uses a 6-pin connector:

```
Game Boy Link Port Layout:
     ___
    |2 4 6|  
    |1 3 5|  
     ---     

Pin 1: VCC (+5V) - Not used
Pin 2: SOUT (Serial Out) - Red wire
Pin 3: SIN (Serial In) - Orange wire  
Pin 4: P14 - Not used
Pin 5: SCK (Serial Clock) - Green wire (not used)
Pin 6: GND (Ground) - Blue wire
```

## Required Connections

### Minimal Setup (2 wires)
```
Game Boy Pin 3 (SIN) → RK-002 GPIO 1
Game Boy Pin 6 (GND) → RK-002 Ground
```

### Full Setup (3 wires)
```
Game Boy Pin 2 (SOUT) → RK-002 GPIO 2 (optional)
Game Boy Pin 3 (SIN) → RK-002 GPIO 1 (required)
Game Boy Pin 6 (GND) → RK-002 Ground (required)
```

## Cable Modification Steps

### 1. Cable Preparation
1. Cut Game Boy Link Cable ~12 inches from connector
2. Strip outer jacket 2 inches to expose wires
3. Identify wires using multimeter continuity test

**Warning**: Wire colors vary by manufacturer! Always test with multimeter.

### 2. Wire Identification
Use multimeter in continuity mode:
1. Touch probe to Game Boy pin
2. Test each wire until you hear beep
3. Label wire with tape
4. Repeat for all needed pins

### 3. RK-002 Connection Points
The RK-002 has two small GPIO solder pads:
- **GPIO 1**: For Game Boy SIN (data to Game Boy)
- **GPIO 2**: For Game Boy SOUT (data from Game Boy)  
- **Ground**: Connect to MIDI connector ground

### 4. Soldering Process
1. Clean RK-002 PCB with isopropyl alcohol
2. Tin GPIO pads with small amount of solder
3. Strip and tin wire ends (2mm)
4. Solder connections:
   - Ground first (Pin 6 → RK-002 GND)
   - GPIO 1 (Pin 3 → RK-002 GPIO 1)
   - GPIO 2 (Pin 2 → RK-002 GPIO 2) - optional

### 5. Testing Connections
Verify with multimeter:
```
✓ Game Boy Pin 3 ↔ RK-002 GPIO 1
✓ Game Boy Pin 6 ↔ RK-002 Ground  
✓ Game Boy Pin 2 ↔ RK-002 GPIO 2 (if connected)

✗ No continuity between GPIO pins or to ground
```

## Alternative Connection Methods

### RK-202 Buttonboard (Easier)
If you have the RK-202 expansion:
```
Button 1 Terminal → Game Boy Pin 3 (SIN)
Button 2 Terminal → Game Boy Pin 2 (SOUT)
Ground Terminal → Game Boy Pin 6 (GND)
```

### Breadboard Prototyping
For testing before permanent modification:
```
RK-002 GPIO 1 → Breadboard → Game Boy Pin 3
RK-002 GPIO 2 → Breadboard → Game Boy Pin 2  
RK-002 Ground → Breadboard → Game Boy Pin 6
```

## Status LED Options

Since the RK-002 has no built-in LEDs:

### Option 1: GPIO LED
```cpp
#define USE_GPIO_LED
#define STATUS_LED_PIN 2  // Uses GPIO 2 instead of Game Boy SOUT
```
Connect LED + 220Ω resistor between GPIO 2 and ground.

### Option 2: Game Boy Visual Feedback
Use the Game Boy screen for status indication.

### Option 3: No Status LED
Rely on Game Boy software feedback only.

## Troubleshooting

### No Game Boy Response
- Check Pin 3 (SIN) connection to GPIO 1
- Verify ground connection (Pin 6)
- Test cable continuity
- Ensure Game Boy software is ready

### Intermittent Communication  
- Check solder joint quality
- Verify strain relief on wires
- Test for loose connections
- Check for interference

### Wrong Notes/Timing
- Verify correct pin connections
- Check for ground loops
- Test with different Game Boy models
- Adjust timing in software if needed

## Safety Notes

- Game Boy operates at 5V (safe voltage)
- RK-002 GPIO pins are 3.3V tolerant
- Use proper grounding to prevent static damage
- Work in well-ventilated area when soldering

## Compatible Game Boy Models

- **DMG** (Original Game Boy) - Works
- **GBP** (Game Boy Pocket) - Works  
- **GBC** (Game Boy Color) - Works
- **GBA** (Game Boy Advance) - Works
- **GBA SP** (Game Boy Advance SP) - Works

All models use the same link port protocol and are compatible with DUYinoboy.
