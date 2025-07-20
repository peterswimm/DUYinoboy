# DUYinoboy Hardware Guide

Complete hardware setup guide for DUYinoboy - ArduinoBoy for RK-002.

**References**: 
- [Game Boy Link Cable Pinout - Hackaday](https://hackaday.io/project/160329-blinky-for-game-boy/log/150762-game-link-cable-and-connector-pinout)
- [RK-002 Specifications - Retrokits](https://retrokits.com/shop/rk002/)
- [Pan Docs External Connectors](https://gbdev.io/pandocs/External_Connectors.html)

## Required Hardware

### Core Components
- **[Retrokits RK-002](https://retrokits.com/shop/rk002/)** Smart MIDI Cable
- **Game Boy** (any model: DMG, GBP, GBC, GBA)
- **Game Boy Link Cable** for modification (official Nintendo recommended)
- **MIDI Interface** with MIDI power support or USB-MIDI device
- **Optional**: [RK-202 Buttonboard](https://retrokits.com/shop/rk202/) for easier connections

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
Game Boy Link Port Layout (EXT connector):
     ___
    |2 4 6|  
    |1 3 5|  
     ---     

Pin 1: VCC (+5V) - Not used in DUYinoboy
Pin 2: SOUT (Serial Out) - Typically red wire (colors vary by manufacturer)
Pin 3: SIN (Serial In) - Typically orange wire (REQUIRED)
Pin 4: P14 - Not used
Pin 5: SCK (Serial Clock) - Typically green wire (used in advanced modes)
Pin 6: GND (Ground) - Typically blue wire (REQUIRED)
```

**⚠️ Important**: Wire colors vary by manufacturer! Always verify with multimeter continuity testing.
**Reference**: [Game Boy Link Cable Details](https://badd10de.dev/notes/gb-link-cables.html)

## Required Connections

### Minimal Setup (2 wires)
```
Game Boy Pin 3 (SIN) → RK-002 GPIO 1
Game Boy Pin 6 (GND) → RK-002 Ground
```

### Full Setup (3 wires)
```
Game Boy Pin 2 (SOUT) → RK-002 GPIO 2 (optional - for bidirectional)
Game Boy Pin 3 (SIN) → RK-002 GPIO 1 (required - data to Game Boy)
Game Boy Pin 6 (GND) → RK-002 Ground (required)
```

**Note**: Pin 5 (SCK) may be needed for advanced modes but RK-002 has only 2 GPIO pins.

## Cable Modification Steps

### 1. Cable Preparation
1. Cut Game Boy Link Cable ~12 inches from connector
2. Strip outer jacket 2 inches to expose wires
3. Identify wires using multimeter continuity test

**⚠️ Critical Warning**: Wire colors vary significantly by manufacturer! Official Nintendo cables use different colors than third-party cables. Always verify connections with multimeter continuity testing before soldering.

**Reference**: [ConsoleMods GBA Pinouts](https://consolemods.org/wiki/GBA:Connector_Pinouts)

### 2. Wire Identification
Use multimeter in continuity mode:
1. Touch probe to Game Boy pin
2. Test each wire until you hear beep
3. Label wire with tape
4. Repeat for all needed pins

### 3. RK-002 Connection Points
The RK-002 has two small GPIO solder pads on the PCB:
- **GPIO 1**: For Game Boy SIN (data to Game Boy) - Pin 3
- **GPIO 2**: For Game Boy SOUT (data from Game Boy) - Pin 2 (optional)
- **Ground**: Connect to MIDI connector ground - Pin 6

**Technical Note**: RK-002 GPIO pins are 5V tolerant, making direct connection to Game Boy safe.
**Reference**: [RK-002 Technical Specifications](https://retrokits.com/shop/rk002/)

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

### RK-202 Buttonboard (Easier Alternative)
If you have the [RK-202 expansion](https://retrokits.com/shop/rk202/):
```
Button 1 Terminal → Game Boy Pin 3 (SIN)
Button 2 Terminal → Game Boy Pin 2 (SOUT)
Ground Terminal → Game Boy Pin 6 (GND)
```
This eliminates the need for direct PCB soldering on the RK-002.

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

- **Voltage Compatibility**: Game Boy operates at 5V, RK-002 GPIO pins are 5V tolerant
- **No Level Shifting Required**: Direct connection is safe for all Game Boy models
- **ESD Protection**: Use proper grounding to prevent static damage
- **Soldering Safety**: Work in well-ventilated area, use proper temperature control
- **Testing**: Always test connections with multimeter before applying power
- **MIDI Power**: Ensure your MIDI interface supports MIDI bus power for RK-002

**References**: 
- [Game Boy Hardware Documentation](https://gbdev.io/pandocs/)
- [RK-002 Power Requirements](https://retrokits.com/shop/rk002/)

## Compatible Game Boy Models

- **DMG** (Original Game Boy, 1989) - ✅ Full compatibility, 5V signals
- **GBP** (Game Boy Pocket, 1996) - ✅ Full compatibility, 5V signals
- **GBC** (Game Boy Color, 1998) - ✅ Full compatibility, 5V signals
- **GBA** (Game Boy Advance, 2001) - ✅ Full compatibility, 3.3V/5V signals
- **GBA SP** (Game Boy Advance SP, 2003) - ✅ Full compatibility, 3.3V/5V signals

**Technical Note**: All models use the same 6-pin link port protocol. GBA/GBA SP operate at 3.3V internally but 5V on link port when running Game Boy/Game Boy Color software.

**Reference**: [Game Boy Link Cable Compatibility](https://en.wikipedia.org/wiki/Game_Link_Cable)
