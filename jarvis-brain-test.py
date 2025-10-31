#!/usr/bin/env python3
"""
JARVIS Brain - TEST MODE (No voice recognition)
Controls the ESP32 Jarvis via Serial
Text-to-speech and keyboard commands only

This version works WITHOUT pyaudio/speechrecognition!
"""

import serial
import time
import threading
from datetime import datetime

# Try to import TTS, but make it optional
try:
    import pyttsx3
    TTS_AVAILABLE = True
except ImportError:
    TTS_AVAILABLE = False
    print("⚠️  pyttsx3 not installed - text-to-speech disabled")

class JarvisBrainTest:
    def __init__(self, port='/dev/cu.usbserial-0001', baudrate=115200):
        """Initialize Jarvis Brain test controller"""
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.running = False
        
        # Text-to-speech (if available)
        if TTS_AVAILABLE:
            try:
                self.tts_engine = pyttsx3.init()
                self.tts_engine.setProperty('rate', 150)
                print("✅ Text-to-speech enabled")
            except Exception as e:
                print(f"⚠️  TTS initialization failed: {e}")
                TTS_AVAILABLE = False
        
        print("🤖 JARVIS Brain Test Mode Initializing...")
        print("📝 Keyboard commands only (no voice recognition)")
        
    def connect(self):
        """Connect to ESP32 via serial"""
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)  # Wait for connection
            print(f"✅ Connected to ESP32 on {self.port}")
            return True
        except Exception as e:
            print(f"❌ Failed to connect: {e}")
            print(f"💡 Available ports:")
            import glob
            ports = glob.glob('/dev/cu.*')
            for p in ports:
                print(f"   - {p}")
            return False
    
    def disconnect(self):
        """Disconnect from ESP32"""
        if self.serial and self.serial.is_open:
            self.serial.close()
            print("🔌 Disconnected from ESP32")
    
    def send_command(self, command):
        """Send command to ESP32"""
        if self.serial and self.serial.is_open:
            self.serial.write(f"{command}\n".encode())
            print(f"📤 Sent: {command}")
            return True
        else:
            print(f"❌ Not connected - can't send: {command}")
            return False
    
    def read_response(self):
        """Read response from ESP32"""
        if self.serial and self.serial.is_open and self.serial.in_waiting:
            response = self.serial.readline().decode().strip()
            if response:
                print(f"📥 ESP32: {response}")
                return response
        return None
    
    def speak(self, text):
        """Speak text via computer speakers"""
        print(f"🔊 JARVIS: {text}")
        if TTS_AVAILABLE and hasattr(self, 'tts_engine'):
            try:
                self.tts_engine.say(text)
                self.tts_engine.runAndWait()
            except Exception as e:
                print(f"⚠️  TTS error: {e}")
    
    def set_face(self, expression):
        """Set ESP32 face expression"""
        valid_expressions = ['idle', 'happy', 'excited', 'thinking', 
                           'listening', 'speaking', 'scanning']
        if expression in valid_expressions:
            self.send_command(f"face:{expression}")
            print(f"😊 Face set to: {expression}")
        else:
            print(f"❌ Invalid expression. Use: {', '.join(valid_expressions)}")
    
    def process_command(self, command):
        """Process text commands"""
        command = command.lower().strip()
        
        # Time commands
        if "time" in command:
            current_time = datetime.now().strftime("%I:%M %p")
            self.send_command("face:speaking")
            self.speak(f"It's {current_time}")
            time.sleep(2)
            self.send_command("face:idle")
        
        # Date commands
        elif "date" in command:
            current_date = datetime.now().strftime("%B %d, %Y")
            self.send_command("face:speaking")
            self.speak(f"Today is {current_date}")
            time.sleep(2)
            self.send_command("face:idle")
        
        # LED control
        elif "lights on" in command or command == "led on":
            self.send_command("led on")
            self.send_command("face:happy")
            self.speak("Lights on")
            time.sleep(1)
            self.send_command("face:idle")
        
        elif "lights off" in command or command == "led off":
            self.send_command("led off")
            self.send_command("face:idle")
            self.speak("Lights off")
        
        # Status commands
        elif "status" in command or "how are you" in command:
            self.send_command("status")
            time.sleep(0.5)
            self.send_command("face:speaking")
            self.speak("All systems operational")
            time.sleep(1)
            self.send_command("face:idle")
        
        # Emotions
        elif "happy" in command:
            self.send_command("face:happy")
            self.speak("Feeling happy!")
            time.sleep(2)
            self.send_command("face:idle")
        
        elif "excited" in command:
            self.send_command("face:excited")
            self.speak("I'm excited!")
            time.sleep(2)
            self.send_command("face:idle")
        
        # Scan
        elif "scan" in command:
            self.send_command("face:scanning")
            self.speak("Scanning environment")
            time.sleep(3)
            self.send_command("face:idle")
        
        # Unknown command
        else:
            print(f"❓ Unknown command: {command}")
            self.speak("I'm not sure how to help with that")
    
    def monitor_serial(self):
        """Monitor serial connection in background"""
        while self.running:
            try:
                self.read_response()
            except Exception as e:
                print(f"⚠️  Serial monitor error: {e}")
            time.sleep(0.1)
    
    def run(self):
        """Main run loop"""
        if not self.connect():
            print("\n❌ Cannot start without ESP32 connection")
            print("💡 Make sure:")
            print("   1. ESP32 is connected via USB")
            print("   2. Arduino sketch is uploaded")
            print("   3. Correct port is selected")
            return
        
        self.running = True
        
        # Start serial monitor thread
        serial_thread = threading.Thread(target=self.monitor_serial, daemon=True)
        serial_thread.start()
        
        # Start with idle face
        time.sleep(1)
        self.send_command("face:idle")
        
        print("\n" + "="*60)
        print("🤖 JARVIS BRAIN TEST MODE - ONLINE")
        print("="*60)
        print(f"📡 Connected to ESP32 on {self.port}")
        print(f"⌨️  Type commands below")
        print(f"🔊 TTS: {'Enabled' if TTS_AVAILABLE else 'Disabled'}")
        print("="*60)
        self.show_help()
        print("="*60 + "\n")
        
        # Keyboard command loop
        try:
            while self.running:
                try:
                    user_input = input("JARVIS> ").strip()
                    
                    if not user_input:
                        continue
                    
                    cmd_lower = user_input.lower()
                    
                    if cmd_lower in ['quit', 'exit', 'q']:
                        break
                    
                    elif cmd_lower == 'help':
                        self.show_help()
                    
                    elif user_input.startswith('say '):
                        text = user_input[4:]
                        self.speak(text)
                    
                    elif user_input.startswith('face '):
                        expression = user_input[5:].lower()
                        self.set_face(expression)
                    
                    elif cmd_lower == 'test all':
                        self.run_test_sequence()
                    
                    elif cmd_lower == 'status':
                        self.send_command("status")
                    
                    elif cmd_lower == 'led on':
                        self.send_command("led on")
                    
                    elif cmd_lower == 'led off':
                        self.send_command("led off")
                    
                    else:
                        # Try to process as natural command
                        self.process_command(user_input)
                    
                except EOFError:
                    break
                    
        except KeyboardInterrupt:
            print("\n⚠️  Interrupted")
        finally:
            self.running = False
            self.send_command("face:idle")
            self.disconnect()
            print("👋 JARVIS Brain shutting down...")
    
    def run_test_sequence(self):
        """Run through all face expressions"""
        print("\n🧪 Running test sequence...")
        
        expressions = ['idle', 'happy', 'excited', 'thinking', 
                      'listening', 'speaking', 'scanning']
        
        for expr in expressions:
            print(f"\n  Testing: {expr}")
            self.send_command(f"face:{expr}")
            self.speak(f"Testing {expr} face")
            time.sleep(3)
        
        print("\n  Testing LED...")
        self.send_command("led on")
        time.sleep(1)
        self.send_command("led off")
        
        self.send_command("face:idle")
        print("\n✅ Test sequence complete!\n")
    
    def show_help(self):
        """Show available commands"""
        print("\n📋 JARVIS Brain Commands:")
        print("\n  Quick Commands:")
        print("    help           - Show this help")
        print("    quit           - Exit program")
        print("    test all       - Run full test sequence")
        print("    status         - Get ESP32 status")
        print("\n  Face Control:")
        print("    face idle      - Neutral face")
        print("    face happy     - Happy expression")
        print("    face excited   - Excited expression")
        print("    face thinking  - Thinking expression")
        print("    face listening - Listening expression")
        print("    face speaking  - Speaking expression")
        print("    face scanning  - Scanning animation")
        print("\n  LED Control:")
        print("    led on         - Turn LED on")
        print("    led off        - Turn LED off")
        print("\n  Speech:")
        print("    say <text>     - Make JARVIS speak")
        print("\n  Natural Commands (try these!):")
        print("    what time is it")
        print("    what's the date")
        print("    lights on / lights off")
        print("    be happy / be excited")
        print("    scan")
        print()

def main():
    """Main entry point"""
    import sys
    
    # Get port from command line or use default
    port = sys.argv[1] if len(sys.argv) > 1 else '/dev/cu.usbserial-0001'
    
    print("="*60)
    print("🧪 JARVIS BRAIN - TEST MODE")
    print("="*60)
    print("This version works WITHOUT voice recognition!")
    print("Use keyboard commands to control JARVIS")
    print("="*60 + "\n")
    
    brain = JarvisBrainTest(port=port)
    brain.run()

if __name__ == "__main__":
    main()

