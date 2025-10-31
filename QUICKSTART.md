# 🚀 Quick Start Guide - ESP32 Jarvis

Get your ESP32 Jarvis up and running in 15 minutes!

## ✅ Pre-Flight Checklist

### Hardware (What you just received!)
- [ ] ESP32 Development Board
- [ ] INMP441 Microphone Module (you have 3!)
- [ ] 0.96" OLED Display Module
- [ ] USB cable for ESP32
- [ ] Jumper wires (should come with the modules)

---

## 📋 Step-by-Step Setup

### STEP 1: Install Software (5 minutes)

1. **Download Arduino IDE**
   - Go to: https://www.arduino.cc/en/software
   - Download and install for your OS
   - Open Arduino IDE

2. **Add ESP32 Board Support**
   - Open: `File → Preferences`
   - Add this URL to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Open: `Tools → Board → Board Manager`
   - Search: "ESP32"
   - Install: "ESP32 by Espressif Systems"

3. **Install Libraries**
   - Open: `Sketch → Include Library → Manage Libraries`
   - Search and install:
     - ✅ "Adafruit GFX Library"
     - ✅ "Adafruit SSD1306"

---

### STEP 2: Wire Everything (5 minutes)

**Don't connect power yet!**

#### OLED Display Connections:
```
OLED Display → ESP32
┌─────────────────────┐
│ VCC  →  3.3V        │
│ GND  →  GND         │
│ SCL  →  GPIO22      │
│ SDA  →  GPIO21      │
└─────────────────────┘
```

#### INMP441 Microphone Connections:
```
INMP441 Mic → ESP32
┌─────────────────────┐
│ VDD  →  3.3V        │
│ GND  →  GND         │
│ SCK  →  GPIO14      │
│ WS   →  GPIO15      │
│ SD   →  GPIO32      │
│ L/R  →  GND         │ (Important!)
└─────────────────────┘
```

**Pro Tip:** Use different colored wires for power (red), ground (black), and signals (other colors).

---

### STEP 3: Configure Code (2 minutes)

1. **Open the sketch**
   - Open `esp32-jarvis.ino` in Arduino IDE

2. **Update WiFi credentials**
   - Find these lines near the top:
   ```cpp
   const char* ssid = "ROCHA2.4";
   const char* password = "sanluispotosi";
   ```
   - Change to YOUR WiFi name and password

3. **Select Board**
   - Go to: `Tools → Board → ESP32 Arduino → ESP32 Dev Module`
   - Go to: `Tools → Port → [Your ESP32 COM port]`

---

### STEP 4: Upload & Test (3 minutes)

1. **Connect ESP32 to computer via USB**

2. **Upload the code**
   - Click the Upload button (→ arrow icon)
   - Wait for "Done uploading"

3. **Open Serial Monitor**
   - Click the magnifying glass icon (top-right)
   - Set baud rate to: **115200**
   - You should see:
   ```
   ESP32 Jarvis Test
   
   OLED: OK
   I2S Mic: OK
   WiFi: Connected!
   IP: 192.168.x.x
   BT: OK
   Ready!
   ```

4. **Check the OLED Display**
   - Should show:
     - "JARVIS" at the top
     - WiFi status
     - IP address
     - Bluetooth status
     - Audio level bar

---

## 🎉 You're Done! Now Test It

### Test 1: Audio Detection
1. Make noise near the microphone
2. Watch the audio bar on the OLED
3. LED should flash on loud sounds
4. Serial Monitor shows audio levels every 10 seconds

### Test 2: Web Control
1. Note the IP address from OLED or Serial Monitor
2. Open a web browser on your phone/computer
3. Go to: `http://[ESP32_IP_ADDRESS]`
4. Try the ON/OFF/BLINK buttons

### Test 3: Bluetooth
1. On your phone, go to Bluetooth settings
2. Look for "ESP32-Jarvis" and pair
3. Download a Bluetooth Terminal app (like "Serial Bluetooth Terminal")
4. Connect to ESP32-Jarvis
5. Send commands:
   - `status` - See system info
   - `led on` - Turn LED on
   - `led off` - Turn LED off

---

## 🔧 Troubleshooting

### Upload Failed?
- **Try holding the BOOT button** on ESP32 while clicking Upload
- Check USB cable (some cables are power-only)
- Try a different USB port
- Make sure correct COM port is selected

### OLED Blank?
- Double-check wiring (SDA and SCL might be swapped)
- Try connecting VCC to 5V instead of 3.3V
- Check if OLED screen is facing you (not backwards)

### No Audio Detected?
- Verify L/R pin is connected to GND
- Check microphone orientation (hole facing outward)
- Make LOUD noises (clap, whistle)
- Check Serial Monitor for "I2S Mic: OK"

### WiFi Won't Connect?
- Verify SSID and password are correct
- Make sure it's a 2.4GHz network (not 5GHz)
- Move closer to router
- Check Serial Monitor for error messages

---

## 📱 What the Display Shows

```
┌──────────────────────────┐
│      J A R V I S         │ ← Title
├──────────────────────────┤
│ WiFi: -45dBm             │ ← Signal strength
│ IP: 192.168.1.100        │ ← Your IP
│ BT: Connected            │ ← BT status
│ Audio:                   │
│ ┌────────────────────┐   │ ← Audio bar
│ │████████           │   │   (fills with sound)
└──────────────────────────┘
```

---

## 🎯 Next Steps

Once everything is working:

1. **Build an enclosure**
   - 3D print a case
   - Or use a small project box
   - Mount with standoffs or hot glue

2. **Improve audio detection**
   - Adjust sensitivity in code
   - Add audio spectrum analyzer
   - Implement wake word detection

3. **Add more sensors**
   - Temperature/humidity
   - Motion detection
   - Light sensor

4. **Integrate with home automation**
   - MQTT protocol
   - Home Assistant
   - Google Home / Alexa

---

## 📚 More Information

- **Detailed Wiring:** See `WIRING_GUIDE.md`
- **Full Documentation:** See `README.md`
- **Library Setup:** See `LIBRARIES.txt`

---

## 🆘 Need Help?

Common issues and solutions:

| Problem | Solution |
|---------|----------|
| Compilation error | Check LIBRARIES.txt, install missing libraries |
| Upload failed | Hold BOOT button during upload |
| OLED blank | Check I2C wiring (SDA/SCL) |
| No audio | Verify L/R pin to GND |
| WiFi failed | Check credentials, use 2.4GHz |

---

## ✨ Success!

If you see the JARVIS display and the audio bar responding to sound, **congratulations!** You've successfully built your ESP32 Jarvis assistant.

**Share your build!** Tag #ESP32Jarvis on social media.

---

**Ready to Upload?** Just follow the steps above and you'll be up and running in no time! 🚀

