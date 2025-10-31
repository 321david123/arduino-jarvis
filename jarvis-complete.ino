/*
 * ESP32 JARVIS - Complete Voice Assistant
 * Features:
 * - Animated face expressions on OLED
 * - I2S microphone audio detection
 * - WiFi and Bluetooth control
 * - Computer brain integration
 * - Voice-reactive animations
 */

#include <WiFi.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>

// WiFi Configuration
const char* ssid = "ROCHA2.4";
const char* password = "sanluispotosi";

// Hardware Setup
BluetoothSerial SerialBT;
const int LED_PIN = 2;
WiFiServer server(80);

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2S Microphone (INMP441)
#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0
#define BUFFER_SIZE 512
int32_t samples[BUFFER_SIZE];

// Timing
unsigned long prevMillis = 0, prevBlink = 0, prevBTKeepAlive = 0;
unsigned long prevDisplay = 0, prevAudio = 0, prevExpression = 0;
int blinkState = 0, phase = 0;

// Audio Detection
float audioLevel = 0;
int peakAudio = 0;
bool voiceDetected = false;
int voiceThreshold = 3000;

// Face Animation
enum FaceExpression {
  FACE_IDLE,
  FACE_HAPPY,
  FACE_EXCITED,
  FACE_THINKING,
  FACE_LISTENING,
  FACE_SPEAKING,
  FACE_SCANNING
};

FaceExpression currentFace = FACE_IDLE;
int animationStep = 0;
bool autoAnimate = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\n🤖 ESP32 JARVIS Starting...\n");
  
  // Initialize OLED
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("❌ OLED: Failed!");
  } else {
    Serial.println("✅ OLED: OK");
    showBootScreen();
  }
  
  // Initialize I2S Microphone
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  
  if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) == ESP_OK) {
    i2s_set_pin(I2S_PORT, &pin_config);
    Serial.println("✅ I2S Mic: OK");
  } else {
    Serial.println("❌ I2S Mic: Failed!");
  }
  
  // Connect WiFi
  testWiFi();
  
  // Start Bluetooth
  testBluetooth();
  
  phase = 3;
  Serial.println("✅ JARVIS Ready!\n");
  currentFace = FACE_IDLE;
  updateDisplay();
}

void loop() {
  unsigned long now = millis();
  
  // Handle connections
  if (phase >= 1) handleWiFiClient();
  if (phase >= 2) handleBluetooth();
  
  // Read audio
  if (now - prevAudio >= 50) {
    prevAudio = now;
    readAudio();
    
    // Detect voice activity
    if (audioLevel > voiceThreshold) {
      if (!voiceDetected) {
        voiceDetected = true;
        if (currentFace == FACE_IDLE) {
          currentFace = FACE_LISTENING;
        }
      }
    } else {
      if (voiceDetected) {
        voiceDetected = false;
      }
    }
  }
  
  // Update display
  if (now - prevDisplay >= 50) {
    prevDisplay = now;
    updateDisplay();
    animationStep++;
  }
  
  // Auto-return to idle
  if (voiceDetected && currentFace == FACE_LISTENING) {
    prevExpression = now;
  } else if (now - prevExpression >= 5000 && currentFace != FACE_IDLE) {
    currentFace = FACE_IDLE;
  }
  
  // LED status blink
  int interval = (phase == 1) ? 1000 : 200;
  if (now - prevBlink >= interval) {
    prevBlink = now;
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }
  
  // Bluetooth keepalive
  if (SerialBT.hasClient() && (now - prevBTKeepAlive >= 10000)) {
    prevBTKeepAlive = now;
    SerialBT.println("JARVIS Ready");
  }
  
  // Status report
  if (now - prevMillis >= 10000) {
    prevMillis = now;
    printStatus();
  }
}

void showBootScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 15);
  display.println("JARVIS");
  display.setTextSize(1);
  display.setCursor(15, 40);
  display.println("Initializing...");
  display.display();
  delay(1500);
}

void testWiFi() {
  Serial.print("📡 WiFi: ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Connected!");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    server.begin();
    phase = 1;
  } else {
    Serial.println("❌ Failed!");
  }
}

void testBluetooth() {
  SerialBT.enableSSP();
  if (SerialBT.begin("ESP32-Jarvis", true)) {
    Serial.println("✅ BT: Ready");
    phase = 2;
  } else {
    Serial.println("❌ BT: Failed");
  }
}

void handleWiFiClient() {
  WiFiClient client = server.available();
  if (client) {
    String req = "";
    while (client.available()) {
      char c = client.read();
      req += c;
      if (c == '\n') break;
    }
    while (client.available()) client.read();
    
    // Handle commands
    if (req.indexOf("/on") > 0) {
      digitalWrite(LED_PIN, HIGH);
    } else if (req.indexOf("/off") > 0) {
      digitalWrite(LED_PIN, LOW);
    } else if (req.indexOf("/happy") > 0) {
      currentFace = FACE_HAPPY;
      prevExpression = millis();
    } else if (req.indexOf("/excited") > 0) {
      currentFace = FACE_EXCITED;
      prevExpression = millis();
    }
    
    // Send response
    client.print("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print("<html><body style='font-family:Arial;text-align:center;background:#000;color:#0f0'>");
    client.print("<h1>🤖 JARVIS Control</h1>");
    client.print("<p><a href='/happy' style='color:#0f0'>😊 HAPPY</a> | ");
    client.print("<a href='/excited' style='color:#0f0'>🤩 EXCITED</a></p>");
    client.print("<p><a href='/on' style='color:#0f0'>💡 LED ON</a> | ");
    client.print("<a href='/off' style='color:#0f0'>⚫ LED OFF</a></p>");
    client.print("</body></html>");
    
    client.stop();
  }
}

void handleBluetooth() {
  if (SerialBT.available()) {
    String msg = SerialBT.readString();
    msg.trim();
    Serial.print("📱 BT: ");
    Serial.println(msg);
    
    // Face commands
    if (msg == "face:idle") {
      currentFace = FACE_IDLE;
      SerialBT.println("Face: Idle");
    } else if (msg == "face:happy") {
      currentFace = FACE_HAPPY;
      prevExpression = millis();
      SerialBT.println("Face: Happy");
    } else if (msg == "face:excited") {
      currentFace = FACE_EXCITED;
      prevExpression = millis();
      SerialBT.println("Face: Excited");
    } else if (msg == "face:thinking") {
      currentFace = FACE_THINKING;
      prevExpression = millis();
      SerialBT.println("Face: Thinking");
    } else if (msg == "face:listening") {
      currentFace = FACE_LISTENING;
      prevExpression = millis();
      SerialBT.println("Face: Listening");
    } else if (msg == "face:speaking") {
      currentFace = FACE_SPEAKING;
      prevExpression = millis();
      SerialBT.println("Face: Speaking");
    } else if (msg == "face:scanning") {
      currentFace = FACE_SCANNING;
      prevExpression = millis();
      SerialBT.println("Face: Scanning");
    }
    // LED commands
    else if (msg == "led on") {
      digitalWrite(LED_PIN, HIGH);
      SerialBT.println("LED ON");
    } else if (msg == "led off") {
      digitalWrite(LED_PIN, LOW);
      SerialBT.println("LED OFF");
    }
    // Status
    else if (msg == "status") {
      SerialBT.print("Uptime: ");
      SerialBT.print(millis()/1000);
      SerialBT.println("s");
      SerialBT.print("Audio: ");
      SerialBT.println(audioLevel);
    }
    // Restart
    else if (msg == "restart") {
      ESP.restart();
    }
  }
}

void readAudio() {
  size_t bytes_read = 0;
  i2s_read(I2S_PORT, &samples, sizeof(samples), &bytes_read, portMAX_DELAY);
  
  int samples_read = bytes_read / sizeof(int32_t);
  int64_t sum = 0;
  int peak = 0;
  
  for (int i = 0; i < samples_read; i++) {
    int32_t sample = samples[i] >> 14;
    int abs_sample = abs(sample);
    sum += (int64_t)sample * sample;
    if (abs_sample > peak) peak = abs_sample;
  }
  
  audioLevel = sqrt(sum / samples_read);
  peakAudio = peak;
}

void updateDisplay() {
  display.clearDisplay();
  
  // Draw face based on current expression
  switch(currentFace) {
    case FACE_IDLE:
      drawFaceIdle();
      break;
    case FACE_HAPPY:
      drawFaceHappy();
      break;
    case FACE_EXCITED:
      drawFaceExcited();
      break;
    case FACE_THINKING:
      drawFaceThinking();
      break;
    case FACE_LISTENING:
      drawFaceListening();
      break;
    case FACE_SPEAKING:
      drawFaceSpeaking();
      break;
    case FACE_SCANNING:
      drawFaceScanning();
      break;
  }
  
  // Show audio level indicator (small)
  int audioBar = map(audioLevel, 0, 10000, 0, 20);
  audioBar = constrain(audioBar, 0, 20);
  if (audioBar > 0) {
    display.fillRect(108, 0, audioBar, 2, SSD1306_WHITE);
  }
  
  display.display();
}

// Face drawing functions
void drawFaceIdle() {
  drawEyes(35, 28, 93, 28, 10, 10);
  display.drawLine(50, 48, 78, 48, SSD1306_WHITE);
  drawCorners();
}

void drawFaceHappy() {
  drawEyes(35, 30, 93, 30, 9, 7);
  // Smile
  for(int i = 0; i < 3; i++) {
    display.drawLine(45+i, 46, 50+i, 51, SSD1306_WHITE);
    display.drawLine(78-i, 51, 83-i, 46, SSD1306_WHITE);
  }
  display.drawLine(50, 51, 78, 51, SSD1306_WHITE);
  drawCorners();
}

void drawFaceExcited() {
  int bounce = (animationStep % 20) < 10 ? 1 : 0;
  drawEyes(35, 26-bounce, 93, 26-bounce, 12, 12);
  // Big smile
  for(int i = 0; i < 4; i++) {
    display.drawLine(42+i, 45, 50+i, 52, SSD1306_WHITE);
    display.drawLine(78-i, 52, 86-i, 45, SSD1306_WHITE);
  }
  display.drawLine(50, 52, 78, 52, SSD1306_WHITE);
}

void drawFaceThinking() {
  int offset = 2;
  drawEyes(35+offset, 26, 93+offset, 26, 10, 10);
  display.drawLine(52, 50, 76, 48, SSD1306_WHITE);
  // Thought dots
  int dotPos = (animationStep / 10) % 3;
  for(int i = 0; i <= dotPos; i++) {
    display.fillCircle(12 + i*6, 12 + i*4, 1+i, SSD1306_WHITE);
  }
}

void drawFaceListening() {
  drawEyes(35, 28, 93, 28, 11, 12);
  display.fillCircle(64, 48, 2, SSD1306_WHITE);
  // Sound waves
  int wave = (animationStep / 2) % 25;
  for(int i = 0; i < 2; i++) {
    int w = wave + (i * 10);
    if (w < 25) {
      display.drawLine(8+w, 30, 10+w, 28, SSD1306_WHITE);
      display.drawLine(10+w, 28, 12+w, 30, SSD1306_WHITE);
      display.drawLine(116-w, 30, 118-w, 32, SSD1306_WHITE);
      display.drawLine(118-w, 32, 120-w, 30, SSD1306_WHITE);
    }
  }
}

void drawFaceSpeaking() {
  drawEyes(35, 28, 93, 28, 10, 10);
  int mouthY = 47 + (animationStep % 8 < 4 ? 1 : -1);
  display.drawLine(52, mouthY, 60, 50, SSD1306_WHITE);
  display.drawLine(60, 50, 68, 50, SSD1306_WHITE);
  display.drawLine(68, 50, 76, mouthY, SSD1306_WHITE);
}

void drawFaceScanning() {
  int scanLine = (animationStep / 2) % 20;
  // Scanning eyes
  display.drawRect(30, 23, 12, 12, SSD1306_WHITE);
  display.drawRect(86, 23, 12, 12, SSD1306_WHITE);
  for(int i = 0; i < 2; i++) {
    int y = 25 + scanLine + (i * 6);
    if (y < 33) {
      display.drawLine(32, y, 40, y, SSD1306_WHITE);
      display.drawLine(88, y, 96, y, SSD1306_WHITE);
    }
  }
  // Scanning mouth
  for(int i = 0; i < 5; i++) {
    display.fillCircle(48 + i*8, 48, 1, SSD1306_WHITE);
  }
}

void drawEyes(int lx, int ly, int rx, int ry, int w, int h) {
  display.fillCircle(lx, ly, w/2, SSD1306_WHITE);
  display.fillCircle(rx, ry, w/2, SSD1306_WHITE);
  display.fillCircle(lx+1, ly, w/4, SSD1306_BLACK);
  display.fillCircle(rx+1, ry, w/4, SSD1306_BLACK);
}

void drawCorners() {
  display.fillCircle(2, 2, 1, SSD1306_WHITE);
  display.fillCircle(125, 2, 1, SSD1306_WHITE);
  display.fillCircle(2, 61, 1, SSD1306_WHITE);
  display.fillCircle(125, 61, 1, SSD1306_WHITE);
}

void printStatus() {
  Serial.println("📊 Status:");
  Serial.print("   Uptime: ");
  Serial.print(millis()/1000);
  Serial.println("s");
  Serial.print("   Audio: ");
  Serial.print(audioLevel);
  Serial.print(" | Peak: ");
  Serial.println(peakAudio);
  Serial.print("   Face: ");
  Serial.println(getFaceName());
  Serial.println();
}

String getFaceName() {
  switch(currentFace) {
    case FACE_IDLE: return "Idle";
    case FACE_HAPPY: return "Happy";
    case FACE_EXCITED: return "Excited";
    case FACE_THINKING: return "Thinking";
    case FACE_LISTENING: return "Listening";
    case FACE_SPEAKING: return "Speaking";
    case FACE_SCANNING: return "Scanning";
    default: return "Unknown";
  }
}

