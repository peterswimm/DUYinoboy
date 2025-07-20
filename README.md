# DUYinoboy

> 🤖 **⚠️ AI-GENERATED PROJECT WARNING ⚠️** 
> 
> **This entire codebase was created through AI conversation and prompts.**
> 
> - ❌ **NOT TESTED** on real RK-002 hardware
> - ❌ **NOT VERIFIED** by hardware experts  
> - ❌ **MAY CONTAIN BUGS** or hardware incompatibilities
> - ⚠️  **USE AT YOUR OWN RISK** - especially with vintage Game Boy hardware
> 
> **📋 Read the full [DISCLAIMER.md](DISCLAIMER.md) before proceeding!**
> 
> **For production use**: Upload via [DUY Web Editor](https://duy.retrokits.com/) and test thoroughly!

**ArduinoBoy for Retrokits RK-002 Smart MIDI Cable**

DUYinoboy is a complete port of the classic ArduinoBoy project for the Retrokits RK-002 Smart MIDI Cable. Transform your Game Boy into a powerful MIDI-controlled instrument using just a smart MIDI cable and a modified Game Boy Link Cable.

## 🎮 What is DUYinoboy?

DUYinoboy brings ArduinoBoy functionality to the ultra-compact RK-002 platform, optimized for MIDI input only:

- **LSDJ MIDI Clock Sync** - Sync Little Sound DJ as MIDI clock follower
- **LSDJ Keyboard Mode** - Play Game Boy like a synthesizer  
- **LSDJ Map Mode** - 4-channel polyphonic control
- **Nanoloop Clock Sync** - Sync Nanoloop as MIDI clock follower
- **Custom Modes** - Extensible for your own Game Boy music applications

**Note**: RK-002 is MIDI input only - your DAW/sequencer provides the master clock

## 🚀 Quick Start

### Prerequisites
- [Retrokits RK-002 Smart MIDI Cable](https://retrokits.com/shop/rk002/)
- Game Boy (DMG, GBP, GBC, or GBA)
- Game Boy Link Cable for modification
- [Little Sound DJ (LSDJ)](https://www.littlesounddj.com/), [Nanoloop](http://www.nanoloop.com/), or [mGB](https://github.com/trash80/mGB) cartridge
- Basic soldering skills
- MIDI interface with MIDI power support

### Installation

1. **Hardware Setup**
   ```
   Game Boy Pin 3 (SIN) → RK-002 GPIO 1 (required)
   Game Boy Pin 6 (GND) → RK-002 Ground (required)
   Game Boy Pin 2 (SOUT) → RK-002 GPIO 2 (optional for bidirectional)
   ```
   See detailed [Hardware Guide](docs/hardware-guide.md)

2. **Upload Firmware**
   - **Recommended**: [DUY Web Editor](https://duy.retrokits.com/) - browser-based development
   - **Alternative**: Arduino IDE 1.8.x with [RK-002 board package](https://retrokits.com/rk002-duy/)
   - **Simulation**: Use [Wokwi Arduino Simulator](https://wokwi.com/arduino) for testing

3. **Connect and Play**
   - MIDI Keyboard → RK-002 → Game Boy
   - Select mode with MIDI Program Change 0-7
   - Settings auto-save to EEPROM

### Development Environment (This Repository)

This repository includes minimal RK-002 and EEPROM libraries in the `libraries/` folder for VS Code IntelliSense and compilation testing:

- **RK002.h/.cpp**: Stub implementations for Arduino compatibility
- **EEPROM.h/.cpp**: Minimal EEPROM library for syntax checking
- **.vscode/c_cpp_properties.json**: VS Code configuration for Arduino projects

**⚠️ Important**: These are development stubs only! For actual hardware deployment:
- Use the [DUY Web Editor](https://duy.retrokits.com/) (recommended)
- Or install the [official RK-002 board package](https://retrokits.com/rk002-duy/) for Arduino IDE

## 📋 Supported Modes

**Note**: DUYinoboy supports MIDI input only - all modes operate as MIDI clock followers

| Mode | Function | Game Boy Software | MIDI Channel |
|------|----------|-------------------|--------------|
| 0 | LSDJ MIDI Clock Sync | LSDJ (slave sync mode) | N/A (clock only) |
| 1 | mGB Basic Mode | mGB ROM | Channels 1-4 → GB Channels |
| 2 | mGB Chord Mode | mGB ROM | Selected channel |
| 3 | mGB Arpeggiator | mGB ROM | Selected channel |
| 4 | mGB with Scales | mGB ROM | Selected channel |
| 5 | mGB with Grids | mGB ROM | Pattern triggers |
| 6 | LSDJ Keyboard Mode | LSDJ Keyboard mode | Selected channel |
| 7 | LSDJ Map Mode | LSDJ (channels 1-4) | Configurable |
| 8 | Nanoloop MIDI Clock Sync | Nanoloop 1.x (slave sync) | N/A (clock only) |

### mGB Mode Details

**mGB Basic** (Mode 1): Direct note control
- MIDI Ch 1 → Game Boy Pulse 1
- MIDI Ch 2 → Game Boy Pulse 2  
- MIDI Ch 3 → Game Boy Wave
- MIDI Ch 4 → Game Boy Noise

**mGB Chord** (Mode 2): Intelligent chord generation
- Single notes trigger chords across all 4 Game Boy channels
- CC 21: Chord type (Major, Minor, 7th, etc.)
- CC 22: Chord inversion
- 8 built-in chord types

**mGB Arpeggiator** (Mode 3): Pattern-based arpeggios
- Notes held in memory, arpeggiated across channels
- CC 21: Arpeggio pattern (Up, Down, Up/Down, Random)
- CC 22: Arpeggio speed (1-16 clock divisions)
- MIDI clock sync for tempo

**mGB Scales** (Mode 4): Musical scale quantization
- Notes quantized to musical scales in real-time
- CC 21: Scale type (Major, Minor, Dorian, etc.)
- CC 22: Scale root note
- 8 built-in musical scales

**mGB Grids** (Mode 5): Euclidean pattern generation
- Inspired by Mutable Instruments Grids
- 4-track Euclidean patterns for Game Boy channels
- CC 21-23: Pattern density and accent control
- MIDI clock driven for rhythmic patterns

## 🔧 Hardware Requirements

### RK-002 Specifications
- **GPIO Pins**: Exactly 2 pins available for I/O
- **MIDI**: Input only (no MIDI output capability)
- **Power**: MIDI bus powered (requires MIDI interface with power support)
- **Programming**: Via MIDI interface using Arduino IDE or [DUY Web Editor](https://duy.retrokits.com/)
- **Platform**: ARM-based microcontroller with Arduino compatibility
- **Cable Length**: ~1.5 meters
- **Expansion**: Compatible with [RK-202 Buttonboard](https://retrokits.com/shop/rk202/) for additional controls

### MIDI Clock Follower Design
DUYinoboy is designed as a **MIDI clock follower only** - your DAW/sequencer provides the master clock. The RK-002 hardware only supports MIDI input, so all timing signals are converted from incoming MIDI clock to Game Boy GPIO signals. No master clock generation capability.

### Game Boy Link Cable Connections
- **Pin 2 (SOUT)**: Red wire → RK-002 GPIO 2 (optional)
- **Pin 3 (SIN)**: Orange wire → RK-002 GPIO 1 (required)
- **Pin 6 (GND)**: Blue wire → RK-002 Ground (required)

*Note: Always verify wire colors with continuity testing as manufacturers vary*

## 🌐 Development Options

### [DUY Web Editor](https://duy.retrokits.com/) (Recommended)
- Browser-based development environment
- Cloud compilation and storage
- Direct upload to RK-002 via MIDI
- Community sharing built-in
- Perfect for beginners
- No local setup required

### Arduino IDE 1.8.x
- Local development environment
- Full control over compilation
- Traditional Arduino workflow
- Requires [RK-002 board package installation](https://retrokits.com/rk002-duy/)
- Better for advanced users

### [Wokwi Arduino Simulator](https://wokwi.com/arduino)
- Online simulation and testing
- No hardware required for development
- Real-time GPIO visualization
- Perfect for prototyping and learning
- See [Simulation Guide](docs/simulation-guide.md)

## 📚 Documentation

- [Complete Hardware Guide](docs/hardware-guide.md) - Wiring, soldering, and connections
- [MIDI Control Reference](docs/midi-reference.md) - All MIDI commands and modes
- [Simulation Guide](docs/simulation-guide.md) - Online testing with Wokwi and others
- [DUY Programming Tutorial](docs/duy-programming.md) - Custom firmware development
- [Troubleshooting Guide](docs/troubleshooting.md) - Common issues and solutions

## 🎵 Compatible Software

- **[LSDJ](https://www.littlesounddj.com/)** (Little Sound DJ) - All versions, keyboard mode support
- **[Nanoloop](http://www.nanoloop.com/)** - Version 1.x for Game Boy
- **[mGB](https://github.com/trash80/mGB)** - Full MIDI sound module support
- **Custom homebrew** - Use provided examples and templates

### LSDJ Mode Support
- **Master/Slave Sync**: [Multiboot MIDI Sync](https://littlesounddj.fandom.com/wiki/Multiboot_MIDI_Sync)
- **Keyboard Mode**: [PC Keyboard Interface](https://littlesounddj.fandom.com/wiki/PC_Keyboard_Interface) compatible
- **Map Mode**: 4-channel polyphonic control

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

- Report bugs via GitHub Issues
- Submit pull requests for improvements
- Share your custom modes and examples
- Improve documentation

## 📄 License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

## 🙏 Credits

- **Original ArduinoBoy**: [trash80 (Timothy Lamb)](https://github.com/trash80/Arduinoboy)
- **RK-002 Platform**: [Retrokits](https://retrokits.com/)
- **Little Sound DJ**: [Johan Kotlinski](https://www.littlesounddj.com/)
- **mGB**: [trash80](https://github.com/trash80/mGB)
- **DUYinoboy Port**: Contributors to this project

## 🔗 Links

### Hardware & Software
- [Original ArduinoBoy Repository](https://github.com/trash80/Arduinoboy)
- [Retrokits RK-002 Smart MIDI Cable](https://retrokits.com/shop/rk002/)
- [RK-202 Buttonboard Expansion](https://retrokits.com/shop/rk202/)
- [Little Sound DJ Official Site](https://www.littlesounddj.com/)
- [Nanoloop Official Site](http://www.nanoloop.com/)
- [mGB MIDI Game Boy](https://github.com/trash80/mGB)

### Development Tools
- [DUY Web Editor](https://duy.retrokits.com/) - Official RK-002 programming environment
- [Wokwi Arduino Simulator](https://wokwi.com/arduino) - Online simulation and testing
- [Arduino IDE](https://www.arduino.cc/en/software) - Local development environment
- [RK-002 Arduino Board Package](https://retrokits.com/rk002-duy/) - Installation guide

### Community & Documentation
- [LSDJ Wiki](https://littlesounddj.fandom.com/wiki/Little_Sound_Dj_Wiki) - Comprehensive LSDJ documentation
- [LSDJ Operating Manual](https://www.littlesounddj.com/lsd/latest/documentation/LSDj_9_2_6.pdf) - Official documentation
- [Retrokits Support](https://retrokits.com/category/rk002/) - RK-002 resources and updates

---

**Transform your Game Boy into the ultimate chiptune instrument with just a smart MIDI cable!** 🎮🎵
