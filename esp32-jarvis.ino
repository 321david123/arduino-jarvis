#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "ROCHA2.4";
const char* password = "sanluispotosi";

BluetoothSerial SerialBT;
const int LED_PIN = 2;
WiFiServer server(80);

unsigned long prevMillis = 0, prevBlink = 0, prevBTKeepAlive = 0;
int blinkState = 0, phase = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nESP32 Jarvis Test\n");
  blinkLED(5, 200);
  testWiFi();
  testBluetooth();
  phase = 3;
  Serial.println("Ready!\n");
}

void loop() {
  unsigned long now = millis();
  
  if (phase >= 1) handleWiFiClient();
  if (phase >= 2) handleBluetooth();
  
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
  Serial.println(" bytes\n");
}
