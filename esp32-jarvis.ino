#include <WiFi.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>

const char* ssid = "ROCHA2.4";
const char* password = "sanluispotosi";

BluetoothSerial SerialBT;
const int LED_PIN = 2;
WiFiServer server(80);

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2S Microphone Configuration (INMP441)
#define I2S_WS 15      // Word Select (LRCLK)
#define I2S_SD 32      // Serial Data (DOUT)
#define I2S_SCK 14     // Serial Clock (BCLK)
#define I2S_PORT I2S_NUM_0
#define BUFFER_SIZE 512
int32_t samples[BUFFER_SIZE];

unsigned long prevMillis = 0, prevBlink = 0, prevBTKeepAlive = 0, prevDisplay = 0, prevAudio = 0;
int blinkState = 0, phase = 0;
float audioLevel = 0;
int peakAudio = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nESP32 Jarvis Test\n");
  
  // Initialize OLED Display
  Wire.begin(21, 22); // SDA=21, SCL=22 (standard ESP32 I2C pins)
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED: Failed!");
  } else {
    Serial.println("OLED: OK");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.println("JARVIS");
    display.setTextSize(1);
    display.setCursor(30, 45);
    display.println("Booting...");
    display.display();
    delay(1000);
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
    Serial.println("I2S Mic: OK");
  } else {
    Serial.println("I2S Mic: Failed!");
  }
  
  blinkLED(5, 200);
  testWiFi();
  testBluetooth();
  phase = 3;
  Serial.println("Ready!\n");
  updateDisplay();
}

void loop() {
  unsigned long now = millis();
  
  if (phase >= 1) handleWiFiClient();
  if (phase >= 2) handleBluetooth();
  
  // Sample audio from microphone
  if (now - prevAudio >= 50) {
    prevAudio = now;
    readAudio();
  }
  
  // Update OLED display
  if (now - prevDisplay >= 500) {
    prevDisplay = now;
    updateDisplay();
  }
  
  int interval = (phase == 1) ? 1000 : 100;
  if (now - prevBlink >= interval) {
    prevBlink = now;
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }
  
  // Bluetooth keepalive
  if (SerialBT.hasClient() && (now - prevBTKeepAlive >= 10000)) {
    prevBTKeepAlive = now;
    SerialBT.println("ESP32 Ready");
  }
  
  if (now - prevMillis >= 10000) {
    prevMillis = now;
    printStatus();
  }
}

void testWiFi() {
  Serial.print("WiFi: ");
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
    Serial.println("Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    server.begin();
    phase = 1;
  } else {
    Serial.println("Failed!");
  }
}

void testBluetooth() {
  SerialBT.enableSSP(); // Enable Secure Simple Pairing for better compatibility
  if (SerialBT.begin("ESP32-Jarvis", true)) { // true = isMaster = server mode
    Serial.println("BT: OK");
    Serial.println("Waiting for connection...");
    blinkLED(2, 200);
    phase = 2;
  } else {
    Serial.println("BT: Failed");
  }
}

void handleWiFiClient() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    String req = "";
    
    // Read first line only
    while (client.available()) {
      char c = client.read();
      req += c;
      if (c == '\n') break;
    }
    
    // Clear remaining data
    while (client.available()) client.read();
    
    // Handle commands
    if (req.indexOf("/on") > 0) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    } else if (req.indexOf("/off") > 0) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    } else if (req.indexOf("/blink") > 0) {
      blinkLED(3, 200);
      Serial.println("LED BLINK");
    }
    
    // Send response
    client.print("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print("<html><body style='font-family:Arial;text-align:center;background:#000;color:#0f0'>");
    client.print("<h1>ESP32 Jarvis</h1><p>IP:");
    client.print(WiFi.localIP());
    client.print(" | RSSI:");
    client.print(WiFi.RSSI());
    client.print("dBm</p><p><a href='/on' style='color:#0f0'>ON</a> | ");
    client.print("<a href='/off' style='color:#0f0'>OFF</a> | ");
    client.print("<a href='/blink' style='color:#0f0'>BLINK</a></p></body></html>");
    
    delay(1);
    client.stop();
    Serial.println("Client done");
  }
}

void handleBluetooth() {
  if (SerialBT.available()) {
    String msg = SerialBT.readString();
    msg.trim();
    Serial.print("BT: ");
    Serial.println(msg);
    
    if (msg == "status") {
      SerialBT.print("Uptime: ");
      SerialBT.print(millis()/1000);
      SerialBT.println("s");
      SerialBT.print("RSSI: ");
      SerialBT.print(WiFi.RSSI());
      SerialBT.println(" dBm");
    } else if (msg == "led on") {
      digitalWrite(LED_PIN, HIGH);
      SerialBT.println("LED ON");
    } else if (msg == "led off") {
      digitalWrite(LED_PIN, LOW);
      SerialBT.println("LED OFF");
    } else if (msg == "restart") {
      ESP.restart();
    } else {
      SerialBT.print("Got: ");
      SerialBT.println(msg);
    }
  }
}

void blinkLED(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
    delay(duration);
  }
}

void printStatus() {
  Serial.println("Status: OK");
  Serial.print("Uptime: ");
  Serial.print(millis()/1000);
  Serial.println("s");
  Serial.print("Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.print("Audio Level: ");
  Serial.print(audioLevel);
  Serial.print(" Peak: ");
  Serial.println(peakAudio);
  Serial.println();
}

void readAudio() {
  size_t bytes_read = 0;
  i2s_read(I2S_PORT, &samples, sizeof(samples), &bytes_read, portMAX_DELAY);
  
  int samples_read = bytes_read / sizeof(int32_t);
  
  // Calculate RMS (Root Mean Square) audio level
  int64_t sum = 0;
  int peak = 0;
  
  for (int i = 0; i < samples_read; i++) {
    int32_t sample = samples[i] >> 14; // Scale down from 32-bit
    int abs_sample = abs(sample);
    sum += (int64_t)sample * sample;
    if (abs_sample > peak) {
      peak = abs_sample;
    }
  }
  
  audioLevel = sqrt(sum / samples_read);
  peakAudio = peak;
  
  // Update LED brightness based on audio level
  if (audioLevel > 5000) {
    digitalWrite(LED_PIN, HIGH);
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  // Title
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println("JARVIS");
  
  // WiFi Status
  display.setTextSize(1);
  display.setCursor(0, 20);
  if (WiFi.status() == WL_CONNECTED) {
    display.print("WiFi: ");
    display.print(WiFi.RSSI());
    display.println("dBm");
    display.setCursor(0, 30);
    display.print("IP: ");
    display.println(WiFi.localIP().toString().substring(0, 15));
  } else {
    display.println("WiFi: Disconnected");
  }
  
  // Bluetooth Status
  display.setCursor(0, 40);
  display.print("BT: ");
  if (SerialBT.hasClient()) {
    display.println("Connected");
  } else {
    display.println("Ready");
  }
  
  // Audio Level Bar
  display.setCursor(0, 50);
  display.print("Audio:");
  
  // Draw audio level bar
  int barWidth = map(audioLevel, 0, 10000, 0, 60);
  barWidth = constrain(barWidth, 0, 60);
  display.drawRect(0, 58, 62, 6, SSD1306_WHITE);
  if (barWidth > 0) {
    display.fillRect(1, 59, barWidth, 4, SSD1306_WHITE);
  }
  
  // Show peak indicator
  display.setCursor(65, 50);
  if (peakAudio > 1000) {
    display.print("!");
  }
  
  display.display();
}
