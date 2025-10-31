/*
 * ESP32 Face Animation Demo
 * Shows animated face expressions on OLED display
 * Cycles through different emotions and states
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Animation timing
unsigned long lastFrame = 0;
int currentExpression = 0;
int animationStep = 0;
bool blinking = false;

// Eye positions
int leftEyeX = 35, leftEyeY = 25;
int rightEyeX = 93, rightEyeY = 25;
int eyeSize = 12;

void setup() {
  Serial.begin(115200);
  
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED failed!");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  
  Serial.println("Face Animation Demo Started!");
}

void loop() {
  unsigned long now = millis();
  
  // Change expression every 3 seconds
  if (now - lastFrame >= 3000) {
    lastFrame = now;
    currentExpression++;
    if (currentExpression > 6) currentExpression = 0;
    animationStep = 0;
  }
  
  display.clearDisplay();
  
  // Draw face based on current expression
  switch(currentExpression) {
    case 0:
      drawNeutral();
      break;
    case 1:
      drawHappy();
      break;
    case 2:
      drawExcited();
      break;
    case 3:
      drawThinking();
      break;
    case 4:
      drawListening();
      break;
    case 5:
      drawSpeaking();
      break;
    case 6:
      drawScanning();
      break;
  }
  
  // Add label
  drawLabel();
  
  display.display();
  animationStep++;
  delay(50);
}

void drawNeutral() {
  // Simple neutral face - JARVIS idle state
  drawEyes(35, 25, 93, 25, 12, 12, false);
  
  // Horizontal mouth
  display.drawLine(45, 50, 83, 50, SSD1306_WHITE);
  
  // Corner dots
  display.fillCircle(3, 3, 2, SSD1306_WHITE);
  display.fillCircle(124, 3, 2, SSD1306_WHITE);
  display.fillCircle(3, 60, 2, SSD1306_WHITE);
  display.fillCircle(124, 60, 2, SSD1306_WHITE);
}

void drawHappy() {
  // Happy eyes (smaller, content)
  drawEyes(35, 27, 93, 27, 10, 8, false);
  
  // Smile
  for(int i = 0; i < 3; i++) {
    display.drawLine(45 + i, 48, 50 + i, 53, SSD1306_WHITE);
    display.drawLine(78 - i, 53, 83 - i, 48, SSD1306_WHITE);
  }
  display.drawLine(50, 53, 78, 53, SSD1306_WHITE);
  
  // Happy corner arcs
  drawCornerArcs();
}

void drawExcited() {
  // Wide open eyes with animation
  int bounce = (animationStep % 20) < 10 ? 1 : 0;
  drawEyes(35, 23 - bounce, 93, 23 - bounce, 14, 14, false);
  
  // Big smile
  for(int i = 0; i < 4; i++) {
    display.drawLine(40 + i, 47, 48 + i, 54, SSD1306_WHITE);
    display.drawLine(80 - i, 54, 88 - i, 47, SSD1306_WHITE);
  }
  display.drawLine(48, 54, 80, 54, SSD1306_WHITE);
  
  // Excitement particles
  if (animationStep % 10 < 5) {
    display.fillCircle(15, 15 + (animationStep % 8), 2, SSD1306_WHITE);
    display.fillCircle(113, 20 + (animationStep % 6), 2, SSD1306_WHITE);
  }
}

void drawThinking() {
  // Looking up and to the side
  int offset = 3;
  drawEyes(35 + offset, 22, 93 + offset, 22, 11, 11, false);
  
  // Thoughtful mouth (slight curve)
  display.drawLine(50, 52, 78, 50, SSD1306_WHITE);
  
  // Thought bubbles
  int bubbleOffset = animationStep % 40;
  display.drawCircle(10, 10 + bubbleOffset / 4, 2, SSD1306_WHITE);
  display.drawCircle(15, 8 + bubbleOffset / 4, 3, SSD1306_WHITE);
  display.drawCircle(22, 6 + bubbleOffset / 4, 4, SSD1306_WHITE);
}

void drawListening() {
  // Focused eyes
  drawEyes(35, 25, 93, 25, 11, 13, false);
  
  // Attentive mouth (small)
  display.drawCircle(64, 50, 3, SSD1306_WHITE);
  
  // Sound waves animation
  int wave = animationStep % 30;
  for(int i = 0; i < 3; i++) {
    int offset = wave + (i * 10);
    if (offset < 30) {
      display.drawLine(5 + offset, 30, 10 + offset, 25, SSD1306_WHITE);
      display.drawLine(10 + offset, 25, 15 + offset, 30, SSD1306_WHITE);
      
      display.drawLine(113 - offset, 30, 118 - offset, 35, SSD1306_WHITE);
      display.drawLine(118 - offset, 35, 123 - offset, 30, SSD1306_WHITE);
    }
  }
}

void drawSpeaking() {
  // Normal eyes
  drawEyes(35, 25, 93, 25, 12, 12, false);
  
  // Animated mouth (speaking)
  int mouthHeight = 48 + (animationStep % 10 < 5 ? 2 : -2);
  display.drawLine(50, mouthHeight, 60, 52, SSD1306_WHITE);
  display.drawLine(60, 52, 68, 52, SSD1306_WHITE);
  display.drawLine(68, 52, 78, mouthHeight, SSD1306_WHITE);
  
  // Voice wave lines
  int waveX = 90 + (animationStep % 20);
  display.drawLine(waveX, 35, waveX + 3, 32, SSD1306_WHITE);
  display.drawLine(waveX + 3, 32, waveX + 6, 35, SSD1306_WHITE);
  
  waveX = 95 + (animationStep % 15);
  display.drawLine(waveX, 38, waveX + 4, 34, SSD1306_WHITE);
  display.drawLine(waveX + 4, 34, waveX + 8, 38, SSD1306_WHITE);
}

void drawScanning() {
  // Scanning eyes (horizontal lines moving)
  int scanLine = (animationStep % 24);
  
  // Left eye
  display.drawRect(28, 20, 14, 14, SSD1306_WHITE);
  for(int i = 0; i < 3; i++) {
    int y = 22 + scanLine + (i * 4);
    if (y < 32) {
      display.drawLine(30, y, 40, y, SSD1306_WHITE);
    }
  }
  
  // Right eye
  display.drawRect(86, 20, 14, 14, SSD1306_WHITE);
  for(int i = 0; i < 3; i++) {
    int y = 22 + scanLine + (i * 4);
    if (y < 32) {
      display.drawLine(88, y, 98, y, SSD1306_WHITE);
    }
  }
  
  // Scanning mouth (dots)
  display.fillCircle(50, 50, 1, SSD1306_WHITE);
  display.fillCircle(58, 50, 1, SSD1306_WHITE);
  display.fillCircle(66, 50, 1, SSD1306_WHITE);
  display.fillCircle(74, 50, 1, SSD1306_WHITE);
  
  // Scanning grid
  int gridOffset = animationStep % 64;
  for(int i = 0; i < 128; i += 16) {
    if ((i + gridOffset) % 64 < 32) {
      display.drawPixel(i, 10, SSD1306_WHITE);
      display.drawPixel(i, 54, SSD1306_WHITE);
    }
  }
}

void drawEyes(int leftX, int leftY, int rightX, int rightY, int width, int height, bool closed) {
  if (closed) {
    // Closed eyes (horizontal lines)
    display.drawLine(leftX - width/2, leftY, leftX + width/2, leftY, SSD1306_WHITE);
    display.drawLine(rightX - width/2, rightY, rightX + width/2, rightY, SSD1306_WHITE);
  } else {
    // Open eyes (circles)
    display.fillCircle(leftX, leftY, width/2, SSD1306_WHITE);
    display.fillCircle(rightX, rightY, width/2, SSD1306_WHITE);
    
    // Pupils (smaller circles in black to create effect)
    display.fillCircle(leftX + 1, leftY, width/4, SSD1306_BLACK);
    display.fillCircle(rightX + 1, rightY, width/4, SSD1306_BLACK);
  }
}

void drawCornerArcs() {
  // Decorative corner arcs
  display.drawLine(0, 5, 5, 0, SSD1306_WHITE);
  display.drawLine(123, 0, 128, 5, SSD1306_WHITE);
  display.drawLine(0, 59, 5, 64, SSD1306_WHITE);
  display.drawLine(123, 64, 128, 59, SSD1306_WHITE);
}

void drawLabel() {
  // Show current state
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  switch(currentExpression) {
    case 0: display.print("IDLE"); break;
    case 1: display.print("HAPPY"); break;
    case 2: display.print("EXCITED"); break;
    case 3: display.print("THINKING"); break;
    case 4: display.print("LISTENING"); break;
    case 5: display.print("SPEAKING"); break;
    case 6: display.print("SCANNING"); break;
  }
}

