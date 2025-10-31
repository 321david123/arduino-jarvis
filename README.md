# ESP32 Jarvis - Voice-Activated Assistant Project

A DIY voice-activated assistant built with ESP32, featuring an I2S microphone and OLED display for real-time visual feedback.

![Project Status](https://img.shields.io/badge/Status-Active-success)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## 🎯 Features

- **🎤 High-Quality Audio Input** - INMP441 I2S MEMS microphone with 24-bit precision
- **📺 Real-Time Visual Display** - 0.96" OLED showing system status and audio levels
- **📡 WiFi Control** - Web-based interface for remote control
- **🔷 Bluetooth Connectivity** - Wireless command interface via Bluetooth Serial
- **🔊 Audio Level Visualization** - Live audio monitoring with peak detection
- **⚡ Low Power Design** - Efficient power consumption for portable applications

## 📦 Hardware Requirements

### Core Components
- **ESP32 Development Board** (ESP32-WROOM-32 or compatible)
- **INMP441 I2S Microphone Module** (omnidirectional, I2S interface)
- **0.96" OLED Display** (128x64, I2C, SSD1306 driver)

### Additional Materials
- USB cable (for programming and power)
- Jumper wires (Female-to-Female)
- Optional: 3D printed enclosure
- Optional: External 3.3V power supply for portable use

### Where to Buy
All components are available on Amazon, Adafruit, or AliExpress:
- INMP441 Microphone: ~$10 for 3 pieces
- OLED Display: ~$8 each
- ESP32 Dev Board: ~$15 for 3 pieces

## 🔧 Installation

### 1. Hardware Setup
Follow the detailed wiring instructions in [WIRING_GUIDE.md](WIRING_GUIDE.md)

**Quick Reference:**
```
INMP441 → ESP32:
  VDD → 3.3V
  GND → GND
  SCK → GPIO14
  WS  → GPIO15
  SD  → GPIO32
  L/R → GND

OLED → ESP32:
  VCC → 3.3V
  GND → GND
  SCL → GPIO22
  SDA → GPIO21
```

### 2. Software Setup

#### Install Arduino IDE
1. Download from [Arduino.cc](https://www.arduino.cc/en/software)
2. Install ESP32 board support:
   - Open Arduino IDE
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Board Manager**
   - Search "ESP32" and install

#### Install Required Libraries
Open Arduino IDE Library Manager (**Sketch → Include Library → Manage Libraries**):
1. **Adafruit GFX Library**
2. **Adafruit SSD1306**

Built-in libraries (no installation needed):
- WiFi
- BluetoothSerial
- Wire
- driver/i2s

### 3. Configure and Upload

1. Open `esp32-jarvis.ino` in Arduino IDE
2. Update WiFi credentials in the code:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Select board: **Tools → Board → ESP32 Dev Module**
4. Select port: **Tools → Port → [Your COM Port]**
5. Click **Upload**

## 🚀 Usage

### Initial Startup
1. Connect ESP32 to power via USB
2. Open Serial Monitor (115200 baud)
3. Wait for initialization messages
4. Note the IP address displayed

### OLED Display
The display shows:
- **JARVIS** title
- WiFi connection status and signal strength
- IP address
- Bluetooth status
- Real-time audio level bar

### Web Interface
1. Connect to the same WiFi network
2. Open browser and navigate to ESP32's IP address
3. Control the LED and view system status

### Bluetooth Control
1. Pair with "ESP32-Jarvis"
2. Use a Bluetooth serial terminal app
3. Available commands:
   - `status` - Get system info
   - `led on` - Turn LED on
   - `led off` - Turn LED off
   - `restart` - Restart ESP32

### Audio Monitoring
- Speak or make noise near the microphone
- Watch the audio level bar on the OLED
- LED will flash on loud sounds
- Audio levels printed to Serial Monitor every 10 seconds

## 📊 System Architecture

```
┌─────────────────────────────────────────┐
│           ESP32 Main Controller          │
├─────────────────────────────────────────┤
│                                          │
│  ┌──────────┐  ┌──────────┐  ┌────────┐│
│  │   WiFi   │  │Bluetooth │  │  I2S   ││
│  │  Module  │  │  Serial  │  │ Audio  ││
│  └──────────┘  └──────────┘  └────────┘│
│                                          │
└─────────────────────────────────────────┘
       │              │              │
       ▼              ▼              ▼
  Web Server    BT Terminal    INMP441 Mic
                                     │
                                     ▼
                               Audio Processing
                                     │
                                     ▼
                               OLED Display
```

## 🎨 Display Modes

The OLED currently displays:
- **Line 1-2:** JARVIS title (large font)
- **Line 3:** WiFi signal strength
- **Line 4:** IP address
- **Line 5:** Bluetooth connection status
- **Line 6-7:** Audio level visualization bar

## 📝 Code Structure

```
esp32-jarvis.ino
├── setup()
│   ├── Initialize Serial
│   ├── Initialize OLED
│   ├── Initialize I2S Microphone
│   ├── Connect WiFi
│   └── Start Bluetooth
├── loop()
│   ├── Handle WiFi clients
│   ├── Handle Bluetooth commands
│   ├── Read audio samples
│   ├── Update OLED display
│   └── Status LED control
└── Helper Functions
    ├── readAudio() - Process microphone input
    ├── updateDisplay() - Refresh OLED screen
    ├── handleWiFiClient() - Process HTTP requests
    └── handleBluetooth() - Process BT commands
```

## 🔍 Troubleshooting

### Common Issues

**OLED shows nothing:**
- Check I2C connections (SDA/SCL not swapped)
- Verify I2C address (should be 0x3C)
- Try connecting VCC to 5V instead of 3.3V

**No audio levels detected:**
- Verify INMP441 connections
- Ensure L/R pin is connected to GND
- Check Serial Monitor for I2S initialization message
- Try speaking louder or closer to the microphone

**WiFi won't connect:**
- Verify SSID and password are correct
- Check WiFi signal strength
- Ensure using 2.4GHz network (ESP32 doesn't support 5GHz)

**Compilation errors:**
- Ensure all required libraries are installed
- Update ESP32 board package to latest version
- Check Arduino IDE is version 1.8.x or newer

## 🛠️ Customization

### Adjust Audio Sensitivity
In `readAudio()` function:
```cpp
if (audioLevel > 5000) {  // Change threshold here
  digitalWrite(LED_PIN, HIGH);
}
```

### Change Display Update Rate
In `loop()` function:
```cpp
if (now - prevDisplay >= 500) {  // Change from 500ms to desired interval
  prevDisplay = now;
  updateDisplay();
}
```

### Modify I2S Sample Rate
In `setup()` function:
```cpp
.sample_rate = 16000,  // Change to 8000, 16000, 22050, 44100, etc.
```

## 🚧 Future Development Ideas

- [ ] Implement wake word detection
- [ ] Add voice command recognition
- [ ] Audio recording to SD card
- [ ] FFT audio spectrum analyzer
- [ ] Multiple display screens
- [ ] Integration with home automation systems
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Battery power with deep sleep mode
- [ ] Temperature/humidity sensors
- [ ] NeoPixel LED ring for visual feedback

## 📚 Resources

### Documentation
- [WIRING_GUIDE.md](WIRING_GUIDE.md) - Detailed wiring instructions
- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [INMP441 Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/INMP441.pdf)
- [SSD1306 OLED Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

### Tutorials
- [ESP32 I2S Audio](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html)
- [Adafruit GFX Guide](https://learn.adafruit.com/adafruit-gfx-graphics-library)

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest new features
- Submit pull requests
- Share your builds

## 📄 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- Espressif for the amazing ESP32 platform
- Adafruit for their excellent graphics libraries
- The maker community for inspiration and support

---

**Made with ❤️ by David Martinez Rodriguez**

*If you found this project helpful, consider giving it a star! ⭐*

