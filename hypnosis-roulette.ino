/*
 * ESP32 Hypnosis Roulette - Random Phrase Display with Spiral Animation
 * 
 * Features:
 * - Shows random phrases from embedded array (no SPIFFS needed!)
 * - Draws animated moving spiral (hypnosis effect) in background
 * - Countdown timer until next phrase change
 * - Roulette animation when countdown hits zero
 * - Fast random text changes that slow down before final selection
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Timing Configuration
#define COUNTDOWN_SECONDS 15
#define ROULETTE_DURATION 3000  // 3 seconds for roulette animation

// State Management
enum State {
  DISPLAY_PHRASE,
  ROULETTE_SPIN
};

State currentState = DISPLAY_PHRASE;
unsigned long lastUpdate = 0;
unsigned long stateStartTime = 0;
int countdown = COUNTDOWN_SECONDS;

// Spiral Animation
float spiralAngle = 0;
float spiralOffset = 0;

// Phrase Management - All phrases embedded in code
const char* phrases[] = {
  "Take a sip of anything near you.",
  "Spin around and point at someone — they're your teammate now.",
  "Ask the next person you see for a high five.",
  "Say the first word that comes to your mind.",
  "Do your best dance move for 5 seconds.",
  "Say something nice about the person on your left.",
  "Pretend to be famous for 10 seconds.",
  "Trade something you have for a piece of candy.",
  "Ask someone to teach you a dance move.",
  "Give a random compliment.",
  "Act like a celebrity doing an interview.",
  "Ask someone to rate your costume 1-10.",
  "Pretend to trip dramatically.",
  "Say \"cheers\" to nobody in particular.",
  "Make a toast about absolutely nothing.",
  "Start clapping slowly until someone joins.",
  "Do a fake movie trailer voice.",
  "Ask \"so what's your deepest secret?\" to a stranger.",
  "Whisper \"it's our little secret now.\"",
  "Start a mini dance-off with anyone nearby.",
  "Ask someone their favorite Halloween candy.",
  "Say \"you look like trouble\" to someone.",
  "Wink at the next person who looks at you.",
  "Ask someone to tell you a fun fact.",
  "Say \"I dare you to dare me.\"",
  "Pretend to be lost and ask for directions.",
  "Do a random handshake with someone.",
  "Say \"you seem suspicious\" to a random person.",
  "Ask \"what's your villain name?\"",
  "Pretend you're a magician for 10 seconds.",
  "Give your best evil laugh.",
  "Say \"I've had just enough candy for this.\"",
  "Ask someone their favorite song and rate it 1–10.",
  "Say \"you again? destiny, I guess.\"",
  "Take a selfie with someone new.",
  "Start chanting \"party! party! party!\" once.",
  "Say \"wow you smell like a main character.\"",
  "Ask \"truth or dare?\" to anyone near you.",
  "Give someone an imaginary award.",
  "Say \"I've seen you in a dream once.\"",
  "Do your best model walk.",
  "Ask someone \"what's your superpower?\"",
  "Make your most dramatic pose.",
  "Say \"you dropped this—your vibe.\"",
  "Pretend you're giving a TED talk.",
  "Say \"you look like you give good hugs.\"",
  "Ask \"if you were a drink, what would you be?\"",
  "Tell someone \"I like your face. Keep it.\"",
  "Say \"what's your favorite lie?\"",
  "Compliment someone's shoes intensely.",
  "Say \"you look like a secret.\"",
  "Tell someone \"don't move. You look perfect right now.\"",
  "Ask \"what's your red flag?\"",
  "Say \"let's take a fake candid picture.\"",
  "Ask \"what's the most random thing you've done today?\"",
  "Tell someone \"I dare you to smile.\"",
  "Say \"I think we'd survive a zombie apocalypse together.\"",
  "Tell someone \"you give off main character energy.\"",
  "Ask \"two truths and a lie—go.\"",
  "Whisper \"you didn't see me here.\"",
  "Ask \"if we switch outfits, who wins?\"",
  "Say \"if you were candy, what kind would you be?\"",
  "Ask \"what song should be playing right now?\"",
  "Say \"your vibe just increased the room temperature.\"",
  "Pretend you just recognized someone famous.",
  "Say \"plot twist: we're best friends now.\"",
  "Ask \"if I were a ghost, would you be scared?\"",
  "Say \"you owe me a dance later.\"",
  "Ask \"what's the most rebellious thing you've done?\"",
  "Pretend you're in a commercial for 5 seconds.",
  "Say \"I like your confidence. It's suspicious.\"",
  "Tell someone \"I like that laugh. Don't change it.\"",
  "Ask \"would you rather fly or read minds?\"",
  "Say \"I have good taste — I just proved it by talking to you.\"",
  "Pretend to be paparazzi taking fake photos.",
  "Say \"what's your go-to drink?\"",
  "Tell someone \"you'd win a staring contest.\"",
  "Ask \"what's the last lie you told?\"",
  "Say \"I challenge you to a dance battle.\"",
  "Pretend you're giving relationship advice on a talk show.",
  "Say \"I saw you smile — that's illegal here.\"",
  "Ask \"how much candy is too much candy?\"",
  "Say \"I'm writing a book. You're chapter one.\"",
  "Tell someone \"you seem dangerously fun.\"",
  "Ask \"what's your biggest flex?\"",
  "Say \"you just unlocked bonus content.\"",
  "Ask \"if we had to run away right now, where would we go?\"",
  "Say \"you'd make a good partner in crime.\"",
  "Tell someone \"I'll remember this moment forever (probably).\"",
  "Ask \"are you mysterious on purpose or by accident?\"",
  "Say \"you seem like you know a secret.\"",
  "Tell someone \"if this were a movie, this would be the meet-cute.\"",
  "Ask \"how old were you when you last believed in ghosts?\"",
  "Say \"I think I saw you in my dream once.\"",
  "Tell someone \"you look like you smell expensive.\"",
  "Ask \"truth or dare?\" and pick dare for them.",
  "Say \"if I disappear tonight, tell my story.\"",
  "Tell someone \"you'd totally survive in a horror movie.\"",
  "Say \"if I had a dollar for every cool person here… I'd have one.\"",
  "Ask \"what's your favorite midnight snack?\"",
  "Say \"you look like you'd steal fries but share dessert.\"",
  "Tell someone \"I'd trust you with my playlist.\"",
  "Ask \"what's the most random skill you have?\"",
  "Say \"I'm allergic to bad vibes.\"",
  "Tell someone \"your energy is chaotic — I like it.\"",
  "Ask \"what's the most spontaneous thing you've done?\"",
  "Say \"I think you just raised the party's IQ.\"",
  "Tell someone \"you look like you have great stories.\"",
  "Ask \"what's one thing on your bucket list?\"",
  "Say \"I challenge you to make someone laugh in 10 seconds.\"",
  "Tell someone \"I like your energy, don't ruin it.\"",
  "Say \"I'll pretend I didn't see that.\"",
  "Ask \"what's the best decision you ever made?\"",
  "Say \"you look like trouble, but fun trouble.\"",
  "Tell someone \"you'd look great in slow motion.\"",
  "Ask \"if you could teleport, where first?\"",
  "Say \"you seem like someone who wins arguments.\"",
  "Tell someone \"if there was a vibe competition, you'd medal.\"",
  "Say \"you remind me of a song I can't stop replaying.\"",
  "Ask \"what's your comfort movie?\"",
  "Say \"I'm legally required to say hi.\"",
  "Tell someone \"you give strong main character vibes.\"",
  "Say \"you should be a movie extra at least.\"",
  "Ask \"what was your first Halloween costume?\"",
  "Tell someone \"I think we'd make a dangerous duo.\"",
  "Say \"don't move, I'm manifesting this moment.\"",
  "Ask \"what's your karaoke song?\"",
  "Say \"I'm not flirting, I'm networking.\"",
  "Tell someone \"you look like fun dressed up as a human.\"",
  "Say \"who invited you? I want to thank them.\"",
  "Ask \"if you could skip to tomorrow, would you?\"",
  "Say \"you look like someone who wins arguments.\"",
  "Tell someone \"your smile just paid my rent.\"",
  "Ask \"what's your dream city?\"",
  "Say \"you're like caffeine with better taste.\"",
  "Tell someone \"I think this is the start of something chaotic.\"",
  "Say \"you owe me a story now.\"",
  "Ask \"if you had to rename yourself, what would it be?\"",
  "Say \"you look like you've survived worse parties.\"",
  "Tell someone \"I'm officially adopting your vibe.\"",
  "Say \"you're either an introvert or a mystery.\"",
  "Ask \"what's your favorite compliment to get?\"",
  "Say \"I think I just saw sparks. Could be static though.\"",
  "Tell someone \"you'd look great on camera.\"",
  "Say \"I dare you to dance with me later.\"",
  "Ask \"who brought the best energy tonight?\"",
  "Tell someone \"I think you just passed the vibe check.\"",
  "Say \"let's agree this conversation never happened.\"",
  "Ask \"what's your toxic trait? Be honest.\"",
  "Say \"you look like good luck in human form.\"",
  "Tell someone \"you're too cool to be real.\"",
  "Say \"I'm blaming you if this night gets weird.\"",
  "Ask \"what's your go-to dance move?\"",
  "Say \"you're officially my favorite stranger.\"",
  "Tell someone \"I'd follow you into questionable decisions.\"",
  "Say \"don't panic, it's only a vibe inspection.\"",
  "Ask \"how many bad ideas make a good story?\"",
  "Say \"you seem like the plot twist.\"",
  "Tell someone \"your energy's illegal in three states.\"",
  "Say \"you'd totally survive this night — I'm not so sure about me.\"",
  "Ask \"what's the most fun rule to break?\"",
  "Say \"you just unlocked party mode.\"",
  "Tell someone \"I think we're the chaos tonight.\"",
  "Say \"cheers to good decisions and better mistakes.\"",
  "Ask \"would you rather flirt or dance?\"",
  "Say \"you look like you have main-character insurance.\"",
  "Tell someone \"you'd look great under disco lights.\"",
  "Say \"if you disappear, I'm blaming the music.\"",
  "Ask \"what's your favorite bad idea?\"",
  "Say \"I think you'd pass the vibe test twice.\"",
  "Tell someone \"I like your face, it's trustworthy.\"",
  "Say \"if this night had a soundtrack, what song would it be?\"",
  "Tell someone \"don't worry, I'm professionally awkward.\"",
  "Say \"I'm convinced we'd get in trouble together.\"",
  "Ask \"what's your best excuse for being late?\"",
  "Say \"you look like a spontaneous decision.\"",
  "Tell someone \"I think we just became the subplot.\"",
  "Say \"I'm pretending we've known each other for years now.\"",
  "Ask \"what's your favorite kind of chaos?\"",
  "Say \"you look like you tell good stories.\"",
  "Tell someone \"your laugh should be copyrighted.\"",
  "Say \"I'll remember this moment unless snacks appear.\"
};

const int phraseCount = sizeof(phrases) / sizeof(phrases[0]);
int currentPhraseIndex = 0;
String currentPhrase = "";

// Roulette Animation
int rouletteSpeed = 20;  // Initial delay between changes (ms)
unsigned long lastRouletteChange = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Hypnosis Roulette Starting ===");

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(F("  Hypnosis"));
  display.setCursor(0, 35);
  display.println(F("  Roulette!"));
  display.display();
  delay(2000);

  // Initialize first phrase
  randomSeed(analogRead(0));  // Random seed
  currentPhraseIndex = random(phraseCount);
  currentPhrase = phrases[currentPhraseIndex];
  
  Serial.printf("Loaded %d phrases\n", phraseCount);
  Serial.println("Starting with: " + currentPhrase);

  stateStartTime = millis();
  countdown = COUNTDOWN_SECONDS;
}

void loop() {
  unsigned long now = millis();

  switch (currentState) {
    case DISPLAY_PHRASE:
      handleDisplayPhrase(now);
      break;
      
    case ROULETTE_SPIN:
      handleRouletteSpin(now);
      break;
  }

  // Always draw the display
  drawDisplay(now);
}

void handleDisplayPhrase(unsigned long now) {
  // Update countdown every second
  if (now - lastUpdate >= 1000) {
    lastUpdate = now;
    countdown--;
    
    if (countdown <= 0) {
      // Start roulette animation
      currentState = ROULETTE_SPIN;
      stateStartTime = now;
      rouletteSpeed = 20;  // Start fast
      lastRouletteChange = now;
      Serial.println("Starting roulette spin!");
    }
  }
}

void handleRouletteSpin(unsigned long now) {
  unsigned long elapsed = now - stateStartTime;
  
  // Progressive slowdown of roulette
  float progress = (float)elapsed / ROULETTE_DURATION;
  progress = constrain(progress, 0.0, 1.0);
  
  // Exponential slowdown curve
  rouletteSpeed = 20 + (int)(progress * progress * 500);  // 20ms -> 520ms
  
  // Change phrase at current speed
  if (now - lastRouletteChange >= rouletteSpeed) {
    lastRouletteChange = now;
    currentPhraseIndex = random(phraseCount);
    currentPhrase = phrases[currentPhraseIndex];
  }
  
  // End roulette after duration
  if (elapsed >= ROULETTE_DURATION) {
    currentState = DISPLAY_PHRASE;
    stateStartTime = now;
    lastUpdate = now;
    countdown = COUNTDOWN_SECONDS;
    Serial.println("Phrase selected: " + currentPhrase);
  }
}

void drawDisplay(unsigned long now) {
  display.clearDisplay();
  
  // Draw animated spiral background
  drawSpiral();
  
  // Draw countdown timer
  if (currentState == DISPLAY_PHRASE) {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(SCREEN_WIDTH - 30, 0);
    display.print(countdown);
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH - 15, 16);
    display.print("s");
  } else {
    // Show "SPIN" during roulette
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH - 30, 0);
    display.print("SPIN!");
  }
  
  // Draw phrase (wrapped text)
  drawWrappedText(currentPhrase, 5, 20, SCREEN_WIDTH - 10);
  
  display.display();
}

void drawSpiral() {
  // Animated hypnotic spiral
  spiralAngle += 0.15;
  spiralOffset += 0.05;
  
  float centerX = SCREEN_WIDTH / 2;
  float centerY = SCREEN_HEIGHT / 2;
  
  // Draw multiple spiral arms
  for (int arm = 0; arm < 3; arm++) {
    float armOffset = arm * (TWO_PI / 3.0);
    
    for (float t = 0; t < 3 * PI; t += 0.3) {
      float angle = t + spiralAngle + armOffset;
      float radius = t * 3 + spiralOffset;
      
      int x = centerX + cos(angle) * radius;
      int y = centerY + sin(angle) * radius;
      
      if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
  }
}

void drawWrappedText(String text, int x, int y, int maxWidth) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Inverted for readability
  
  int cursorX = x;
  int cursorY = y;
  int lineHeight = 10;
  String word = "";
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    
    if (c == ' ' || i == text.length() - 1) {
      // Complete the word
      if (i == text.length() - 1 && c != ' ') {
        word += c;
      }
      
      // Check if word fits on current line
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(word, cursorX, cursorY, &x1, &y1, &w, &h);
      
      if (cursorX + w > x + maxWidth && cursorX > x) {
        // Move to next line
        cursorX = x;
        cursorY += lineHeight;
      }
      
      // Stop if we run out of vertical space
      if (cursorY + lineHeight > SCREEN_HEIGHT) {
        display.setCursor(x, cursorY);
        display.print("...");
        return;
      }
      
      // Draw the word
      display.setCursor(cursorX, cursorY);
      display.print(word);
      cursorX += w;
      
      // Add space
      if (c == ' ') {
        display.print(" ");
        cursorX += 6;  // Approximate space width
      }
      
      word = "";
    } else {
      word += c;
    }
  }
}


