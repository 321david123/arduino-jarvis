#!/usr/bin/env python3
"""
JARVIS Brain - Computer Side Controller
Controls the ESP32 Jarvis via Bluetooth Serial
Processes voice commands and sends responses

Requirements:
    pip install pyserial speechrecognition pyaudio pyttsx3
"""

import serial
import time
import threading
import speech_recognition as sr
import pyttsx3
from datetime import datetime

class JarvisBrain:
    def __init__(self, port='/dev/cu.usbserial-0001', baudrate=115200):
        """Initialize Jarvis Brain computer controller"""
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.running = False
        
        # Voice recognition
        self.recognizer = sr.Recognizer()
        self.microphone = sr.Microphone()
        
        # Text-to-speech
        self.tts_engine = pyttsx3.init()
        self.tts_engine.setProperty('rate', 150)
        
        # Wake word
        self.wake_word = "jarvis"
        self.listening = False
        
        print("🤖 JARVIS Brain Initializing...")
        
    def connect(self):
        """Connect to ESP32 via serial"""
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)  # Wait for connection
            print(f"✅ Connected to ESP32 on {self.port}")
            return True
        except Exception as e:
            print(f"❌ Failed to connect: {e}")
            print(f"💡 Try: ls /dev/cu.* | grep usb")
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
        self.tts_engine.say(text)
        self.tts_engine.runAndWait()
    
    def set_face(self, expression):
        """Set ESP32 face expression"""
        valid_expressions = ['idle', 'happy', 'excited', 'thinking', 
                           'listening', 'speaking', 'scanning']
        if expression in valid_expressions:
            self.send_command(f"face:{expression}")
    
    def listen_for_wake_word(self):
        """Listen for wake word in background"""
        with self.microphone as source:
            self.recognizer.adjust_for_ambient_noise(source)
            print("🎤 Listening for wake word 'Jarvis'...")
            
            while self.running:
                try:
                    audio = self.recognizer.listen(source, timeout=1, phrase_time_limit=3)
                    text = self.recognizer.recognize_google(audio).lower()
                    
                    if self.wake_word in text:
                        print(f"👂 Wake word detected: '{text}'")
                        self.send_command("face:listening")
                        self.handle_wake_word()
                        
                except sr.WaitTimeoutError:
                    pass
                except sr.UnknownValueError:
                    pass
                except sr.RequestError as e:
                    print(f"❌ Speech recognition error: {e}")
                except Exception as e:
                    print(f"⚠️  Error: {e}")
    
    def handle_wake_word(self):
        """Handle commands after wake word detected"""
        self.speak("Yes sir?")
        self.send_command("face:listening")
        
        with self.microphone as source:
            try:
                print("🎤 Listening for command...")
                audio = self.recognizer.listen(source, timeout=5, phrase_time_limit=5)
                command = self.recognizer.recognize_google(audio).lower()
                print(f"💬 You said: {command}")
                
                self.process_command(command)
                
            except sr.WaitTimeoutError:
                self.speak("I didn't hear anything")
                self.send_command("face:idle")
            except sr.UnknownValueError:
                self.speak("I didn't understand that")
                self.send_command("face:idle")
            except Exception as e:
                print(f"❌ Error: {e}")
                self.send_command("face:idle")
    
    def process_command(self, command):
        """Process voice commands"""
        self.send_command("face:thinking")
        
        # Time commands
        if "time" in command or "what time" in command:
            current_time = datetime.now().strftime("%I:%M %p")
            self.send_command("face:speaking")
            self.speak(f"It's {current_time}")
        
        # Date commands
        elif "date" in command or "what day" in command:
            current_date = datetime.now().strftime("%B %d, %Y")
            self.send_command("face:speaking")
            self.speak(f"Today is {current_date}")
        
        # LED control
        elif "lights on" in command or "led on" in command:
            self.send_command("led on")
            self.send_command("face:happy")
            self.speak("Lights on")
        
        elif "lights off" in command or "led off" in command:
            self.send_command("led off")
            self.send_command("face:idle")
            self.speak("Lights off")
        
        # Status commands
        elif "status" in command or "how are you" in command:
            self.send_command("status")
            time.sleep(0.5)
            response = self.read_response()
            self.send_command("face:speaking")
            self.speak("All systems operational")
        
        # Emotions
        elif "happy" in command:
            self.send_command("face:happy")
            self.speak("Feeling happy!")
        
        elif "excited" in command:
            self.send_command("face:excited")
            self.speak("I'm excited!")
        
        # Scan
        elif "scan" in command:
            self.send_command("face:scanning")
            self.speak("Scanning environment")
            time.sleep(3)
            self.send_command("face:idle")
        
        # Unknown command
        else:
            self.send_command("face:thinking")
            self.speak("I'm not sure how to help with that")
        
        # Return to idle
        time.sleep(1)
        self.send_command("face:idle")
    
    def monitor_serial(self):
        """Monitor serial connection in background"""
        while self.running:
            response = self.read_response()
            if response:
                # Process ESP32 messages
                if "audio level" in response.lower():
                    # Could trigger face reactions based on audio
                    pass
            time.sleep(0.1)
    
    def run(self):
        """Main run loop"""
        if not self.connect():
            return
        
        self.running = True
        
        # Start serial monitor thread
        serial_thread = threading.Thread(target=self.monitor_serial, daemon=True)
        serial_thread.start()
        
        # Start with idle face
        time.sleep(1)
        self.send_command("face:idle")
        
        print("\n" + "="*50)
        print("🤖 JARVIS BRAIN IS ONLINE")
        print("="*50)
        print(f"📡 Connected to ESP32")
        print(f"🎤 Say 'Jarvis' to activate")
        print(f"⌨️  Type 'help' for keyboard commands")
        print("="*50 + "\n")
        
        # Start voice recognition thread
        voice_thread = threading.Thread(target=self.listen_for_wake_word, daemon=True)
        voice_thread.start()
        
        # Keyboard command loop
        try:
            while self.running:
                user_input = input().strip().lower()
                
                if user_input == 'quit' or user_input == 'exit':
                    break
                elif user_input == 'help':
                    self.show_help()
                elif user_input.startswith('say '):
                    text = user_input[4:]
                    self.speak(text)
                elif user_input.startswith('face '):
                    expression = user_input[5:]
                    self.set_face(expression)
                elif user_input == 'status':
                    self.send_command("status")
                elif user_input == 'led on':
                    self.send_command("led on")
                elif user_input == 'led off':
                    self.send_command("led off")
                else:
                    self.send_command(user_input)
                
        except KeyboardInterrupt:
            print("\n⚠️  Interrupted")
        finally:
            self.running = False
            self.send_command("face:idle")
            self.disconnect()
            print("👋 JARVIS Brain shutting down...")
    
    def show_help(self):
        """Show available commands"""
        print("\n📋 JARVIS Brain Commands:")
        print("  Voice: Say 'Jarvis' followed by a command")
        print("  Keyboard commands:")
        print("    say <text>     - Make JARVIS speak")
        print("    face <expr>    - Change face (idle/happy/excited/thinking/listening/speaking/scanning)")
        print("    led on/off     - Control LED")
        print("    status         - Get ESP32 status")
        print("    help           - Show this help")
        print("    quit           - Exit program")
        print()

def main():
    """Main entry point"""
    import sys
    
    # Get port from command line or use default
    port = sys.argv[1] if len(sys.argv) > 1 else '/dev/cu.usbserial-0001'
    
    brain = JarvisBrain(port=port)
    brain.run()

if __name__ == "__main__":
    main()

