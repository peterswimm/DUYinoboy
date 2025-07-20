# Contributing to DUYinoboy

Thank you for your interest in contributing to DUYinoboy! This project aims to make Game Boy music creation accessible through the RK-002 Smart MIDI Cable.

## How to Contribute

### 🐛 Bug Reports
- Use GitHub Issues to report bugs
- Include RK-002 firmware version
- Describe Game Boy model and software used
- Provide steps to reproduce the issue

### 🎵 New Features
- Discuss major changes in Issues first
- Follow existing code style and patterns
- Test with real Game Boy hardware
- Update documentation accordingly

### 📚 Documentation
- Improve setup guides and tutorials
- Add troubleshooting tips
- Share hardware modification techniques
- Contribute example projects

### 🔧 Code Contributions

#### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Set up development environment:
   - Arduino IDE 1.8.x OR DUY Web Editor
   - RK-002 with DUY firmware
   - Game Boy with test software

#### Code Guidelines
- Use clear, descriptive variable names
- Comment complex Game Boy timing code
- Keep functions focused and small
- Test on multiple Game Boy models if possible

#### Pull Request Process
1. Update documentation for any new features
2. Test thoroughly with real hardware
3. Ensure code compiles without warnings
4. Create clear commit messages
5. Submit pull request with detailed description

## Project Structure

```
DUYinoboy/
├── src/                 # Main source code
├── examples/            # Example programs
├── docs/               # Documentation
├── hardware/           # Schematics and guides
└── tools/              # Utilities and scripts
```

## Code Style

### C++ Style
```cpp
// Use descriptive names
#define GB_SERIAL_IN_PIN    1
#define GB_GROUND_PIN       6

// Comment timing-critical sections
void sendGameBoyByte(byte data) {
  // Game Boy link port requires specific timing
  for (int bit = 7; bit >= 0; bit--) {
    // Timing comment here
  }
}

// Use const for lookup tables
const byte PROGMEM midiToGameBoyNote[] = {
  // MIDI note mapping table
};
```

### Documentation Style
- Use clear headers and sections
- Include code examples for complex topics
- Add troubleshooting sections
- Reference official Game Boy documentation

## Testing

### Hardware Testing
- Test on original Game Boy (DMG)
- Test on Game Boy Color (CGB)
- Test on Game Boy Advance (AGB)
- Verify with LSDJ and Nanoloop

### Software Testing
- Compile without warnings
- Test all MIDI modes
- Verify timing accuracy
- Check memory usage

## Community Guidelines

### Be Respectful
- Welcome newcomers to Game Boy music
- Help troubleshoot hardware issues
- Share knowledge and resources
- Credit original work and contributors

### Stay Focused
- Keep discussions relevant to DUYinoboy/RK-002
- Direct general Game Boy questions to appropriate forums
- Link to external resources when helpful

## Resources

### Core References
- [Original ArduinoBoy Repository](https://github.com/trash80/Arduinoboy) - Foundation project
- [RK-002 Smart MIDI Cable](https://retrokits.com/shop/rk002/) - Hardware platform
- [DUY Web Editor](https://duy.retrokits.com/) - Official programming environment
- [Arduino IDE](https://www.arduino.cc/en/software) - Alternative development environment

### Game Boy Development
- [Game Boy Development Community](https://gbdev.io/) - Hardware documentation
- [Pan Docs](https://gbdev.io/pandocs/) - Complete Game Boy hardware reference
- [Game Boy Link Cable Details](https://gbdev.io/pandocs/External_Connectors.html) - Connection specifications

### Music Software
- [Little Sound DJ Official](https://www.littlesounddj.com/) - Primary music software
- [LSDJ Documentation](https://www.littlesounddj.com/lsd/latest/documentation/LSDj_9_2_6.pdf) - Complete manual
- [LSDJ Wiki](https://littlesounddj.fandom.com/wiki/Little_Sound_Dj_Wiki) - Community documentation
- [Nanoloop Official](http://www.nanoloop.com/) - Alternative music software
- [mGB Repository](https://github.com/trash80/mGB) - MIDI Game Boy firmware

### Community Forums
- [Chip Music Community](https://chipmusic.org/) - Game Boy music forums
- [Arduino Forum](https://forum.arduino.cc/) - Programming help
- [Retrokits Support](https://retrokits.com/) - Hardware support

## Recognition

Contributors will be recognized in:
- README.md credits section
- Release notes for significant contributions
- Documentation for tutorial contributions

Thank you for helping make Game Boy music creation more accessible! 🎮🎵
