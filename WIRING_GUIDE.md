# ESP32 Jarvis - Wiring Guide

## Components Required
- ESP32 Development Board
- INMP441 I2S Microphone Module (3 pieces included)
- 0.96" OLED Display (128x64, SSD1306, I2C)
- Jumper wires (Female-to-Female)

---

## Wiring Connections

### INMP441 Microphone → ESP32
Connect the INMP441 microphone to your ESP32 as follows:

| INMP441 Pin | ESP32 Pin | Description |
|-------------|-----------|-------------|
| **VDD**     | **3.3V**  | Power supply |
| **GND**     | **GND**   | Ground |
| **SCK**     | **GPIO14**| Serial Clock (BCLK) |
| **WS**      | **GPIO15**| Word Select (LRCLK) |
| **SD**      | **GPIO32**| Serial Data (DOUT) |
| **L/R**     | **GND**   | Left/Right channel select (GND = Left) |

### OLED Display → ESP32
Connect the 0.96" OLED display to your ESP32:

| OLED Pin | ESP32 Pin | Description |
|----------|-----------|-------------|
| **VCC**  | **3.3V** or **5V** | Power supply |
| **GND**  | **GND**  | Ground |
| **SCL**  | **GPIO22**| I2C Clock |
| **SDA**  | **GPIO21**| I2C Data |

---

## Pin Summary (ESP32)

Here's a complete overview of all pins used:

```
ESP32 Pin Usage:
- GPIO2  : Built-in LED (status indicator)
- GPIO14 : I2S SCK (Microphone clock)
- GPIO15 : I2S WS (Microphone word select)
- GPIO21 : I2C SDA (OLED data)
- GPIO22 : I2C SCL (OLED clock)
- GPIO32 : I2S SD (Microphone data)
- 3.3V   : Power for INMP441 and OLED
- GND    : Common ground
```

---

## Assembly Instructions

### Step 1: Prepare Your Workspace
1. Disconnect ESP32 from power
2. Lay out all components
3. Identify the correct pins on each component

### Step 2: Connect the OLED Display
1. Connect **VCC** (OLED) to **3.3V** (ESP32)
2. Connect **GND** (OLED) to **GND** (ESP32)
3. Connect **SCL** (OLED) to **GPIO22** (ESP32)
4. Connect **SDA** (OLED) to **GPIO21** (ESP32)

**Note:** The OLED can also work with 5V if preferred.

### Step 3: Connect the INMP441 Microphone
1. Connect **VDD** (INMP441) to **3.3V** (ESP32)
2. Connect **GND** (INMP441) to **GND** (ESP32)
3. Connect **SCK** (INMP441) to **GPIO14** (ESP32)
4. Connect **WS** (INMP441) to **GPIO15** (ESP32)
5. Connect **SD** (INMP441) to **GPIO32** (ESP32)
6. Connect **L/R** (INMP441) to **GND** (ESP32) - This sets it to LEFT channel

### Step 4: Double-Check Connections
- Verify all power connections (3.3V and GND)
- Ensure no pins are shorted
- Check that wire colors match your notes

---

## Software Setup

### Required Libraries

Install these libraries through Arduino IDE Library Manager:
1. **Adafruit GFX Library** (by Adafruit)
2. **Adafruit SSD1306** (by Adafruit)
3. **WiFi** (built-in with ESP32)
4. **BluetoothSerial** (built-in with ESP32)

### Installation Steps

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for and install:
   - "Adafruit GFX Library"
   - "Adafruit SSD1306"
4. Select your board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
5. Select the correct COM port: **Tools → Port → [Your ESP32 Port]**
6. Upload the sketch

---

## Testing

### Power-On Test
1. Connect ESP32 to USB
2. Open Serial Monitor (115200 baud)
3. You should see:
   ```
   ESP32 Jarvis Test
   
   OLED: OK
   I2S Mic: OK
   WiFi: Connected!
   IP: [Your IP Address]
   BT: OK
   Ready!
   ```

### OLED Display Test
The OLED should display:
- **JARVIS** title at the top
- WiFi status and signal strength
- IP address
- Bluetooth status
- Audio level bar at the bottom

### Microphone Test
1. Make some noise near the microphone
2. Watch the audio level bar on the OLED display
3. The Serial Monitor should show audio level readings every 10 seconds
4. The built-in LED should react to loud sounds

### WiFi Test
1. Note the IP address from Serial Monitor or OLED
2. Open a web browser
3. Navigate to: `http://[ESP32_IP_ADDRESS]`
4. You should see the Jarvis control panel

### Bluetooth Test
1. Pair your phone/computer with "ESP32-Jarvis"
2. Use a Bluetooth terminal app
3. Send commands like:
   - `status` - Get system status
   - `led on` - Turn LED on
   - `led off` - Turn LED off

---

## Troubleshooting

### OLED Not Working
- **Check I2C address:** Run an I2C scanner sketch to verify address is 0x3C
- **Check connections:** Verify SDA and SCL are not swapped
- **Try 5V:** If 3.3V doesn't work, try connecting VCC to 5V
- **Check soldering:** Some OLED modules require header pins to be soldered

### Microphone Not Working
- **Check orientation:** INMP441 microphone hole should face outward
- **Verify L/R pin:** Must be connected to GND for left channel
- **Check Serial output:** Audio levels should change when making noise
- **Test with different pins:** If GPIO32 doesn't work, try GPIO33

### WiFi Connection Issues
- **Check credentials:** Verify SSID and password in code
- **Check signal:** Move closer to WiFi router
- **Check Serial Monitor:** Look for connection error messages

### Bluetooth Issues
- **Unpair and re-pair:** Remove device from Bluetooth settings and pair again
- **Check device name:** Look for "ESP32-Jarvis"
- **Restart ESP32:** Power cycle the device

---

## Features

### Current Capabilities
✅ **Visual Status Display** - Real-time system info on OLED  
✅ **Audio Level Monitoring** - Live audio visualization  
✅ **WiFi Control** - Web-based control interface  
✅ **Bluetooth Control** - Wireless command interface  
✅ **Voice Activity Detection** - LED reacts to sound  

### Future Enhancements (Ideas)
- Voice command recognition
- Audio recording and streaming
- Wake word detection ("Hey Jarvis")
- Audio spectrum analyzer display
- Multiple display screens (swipe between views)
- Temperature/humidity sensor integration

---

## Safety Notes

⚠️ **Important Safety Information:**
- Always disconnect power before making wiring changes
- Do not exceed 3.3V on GPIO pins
- Keep components away from water
- Use proper power supply (500mA minimum recommended)
- Avoid short circuits between power and ground

---

## Support & Resources

### Datasheets
- [INMP441 Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/INMP441.pdf)
- [SSD1306 OLED Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)

### Useful Links
- [ESP32 I2S Audio Tutorial](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html)
- [Adafruit GFX Graphics Library](https://learn.adafruit.com/adafruit-gfx-graphics-library)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)

---

## License
This project is provided as-is for educational and hobbyist purposes.

**Built with ❤️ for the maker community**

