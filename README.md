# DUYinoboy

**ArduinoBoy for Retrokits RK-002 Smart MIDI Cable**

DUYinoboy is a complete port of the classic ArduinoBoy project for the Retrokits RK-002 Smart MIDI Cable. Transform your Game Boy into a powerful MIDI instrument using just a smart MIDI cable and a modified Game Boy Link Cable.

## 🎮 What is DUYinoboy?

DUYinoboy brings all the functionality of ArduinoBoy to the ultra-compact RK-002 platform:

- **LSDJ Master/Slave Sync** - Sync Little Sound DJ with your DAW
- **LSDJ Keyboard Mode** - Play Game Boy like a synthesizer  
- **LSDJ Map Mode** - 4-channel polyphonic control
- **Nanoloop Support** - Master and slave sync for Nanoloop
- **Custom Modes** - Extensible for your own Game Boy music applications

## 🚀 Quick Start

### Prerequisites
- Retrokits RK-002 Smart MIDI Cable
- Game Boy (DMG, GBP, GBC, or GBA)
- Game Boy Link Cable for modification
- LSDJ, Nanoloop, or mGB cartridge
- Basic soldering skills

### Installation

1. **Hardware Setup**
   ```
   Game Boy Pin 3 (SIN) → RK-002 GPIO 1
   Game Boy Pin 6 (GND) → RK-002 Ground
   Game Boy Pin 2 (SOUT) → RK-002 GPIO 2 (optional)
   ```

2. **Upload Firmware**
   - Use DUY Web Editor: https://duy.retrokits.com/
   - Or Arduino IDE 1.8.x with RK-002 board package

3. **Connect and Play**
   - MIDI Keyboard → RK-002 → Game Boy
   - Select mode with MIDI Program Change 0-7

## 📋 Supported Modes

| Mode | Function | Game Boy Software |
|------|----------|-------------------|
| 0 | LSDJ Master Sync | LSDJ (any version) |
| 1 | LSDJ Slave Sync | LSDJ (any version) |
| 2 | LSDJ Keyboard | LSDJ Keyboard mode |
| 3 | LSDJ Map Mode | LSDJ (channels 1-4) |
| 4 | Nanoloop Master | Nanoloop 1.x |
| 5 | Nanoloop Slave | Nanoloop 1.x |
| 6-7 | Custom Modes | User-definable |

## 🔧 Hardware Requirements

### RK-002 Specifications
- **GPIO Pins**: Exactly 2 pins available
- **Power**: MIDI bus powered (5V from MIDI OUT)
- **Programming**: Via MIDI interface (not USB)
- **Platform**: ARM-based with Arduino IDE support

### Game Boy Link Cable Connections
- **Pin 2 (SOUT)**: Red wire → RK-002 GPIO 2 (optional)
- **Pin 3 (SIN)**: Orange wire → RK-002 GPIO 1 (required)
- **Pin 6 (GND)**: Blue wire → RK-002 Ground (required)

*Note: Always verify wire colors with continuity testing as manufacturers vary*

## 🌐 Development Options

### DUY Web Editor (Recommended)
- Browser-based development
- Cloud compilation and storage
- Direct upload to RK-002
- Community sharing built-in
- Perfect for beginners

### Arduino IDE 1.8.x
- Local development environment
- Full control over compilation
- Traditional Arduino workflow
- Better for advanced users

## 📚 Documentation

- [Complete Hardware Guide](docs/hardware-guide.md)
- [DUY Programming Tutorial](docs/duy-programming.md)
- [MIDI Control Reference](docs/midi-reference.md)
- [Troubleshooting Guide](docs/troubleshooting.md)

## 🎵 Compatible Software

- **LSDJ** (Little Sound DJ) - All versions
- **Nanoloop** - Version 1.x
- **mGB** - Full MIDI support
- **Custom homebrew** - Use provided examples

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

- Report bugs via GitHub Issues
- Submit pull requests for improvements
- Share your custom modes and examples
- Improve documentation

## 📄 License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

## 🙏 Credits

- **Original ArduinoBoy**: trash80 (Timothy Lamb)
- **RK-002 Platform**: Retrokits
- **DUYinoboy Port**: Contributors to this project

## 🔗 Links

- [Original ArduinoBoy](https://github.com/trash80/Arduinoboy)
- [Retrokits RK-002](https://retrokits.com/shop/rk002/)
- [DUY Web Editor](https://duy.retrokits.com/)
- [LSDJ Official Site](https://www.littlesounddj.com/)

---

**Transform your Game Boy into the ultimate chiptune instrument with just a smart MIDI cable!** 🎮🎵
