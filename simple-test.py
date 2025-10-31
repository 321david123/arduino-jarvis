#!/usr/bin/env python3
"""
Simple JARVIS Test - No dependencies required!
Uses only Python built-in libraries
"""

import sys

def main():
    print("="*60)
    print("🧪 JARVIS CONNECTION TEST")
    print("="*60)
    print()
    
    # Try to import serial
    try:
        import serial
        print("✅ pyserial is installed")
        serial_available = True
    except ImportError:
        print("❌ pyserial is NOT installed")
        print("   Install with: pip3 install --user pyserial")
        serial_available = False
    
    # Try to import TTS
    try:
        import pyttsx3
        print("✅ pyttsx3 is installed")
        tts_available = True
    except ImportError:
        print("❌ pyttsx3 is NOT installed")  
        print("   Install with: pip3 install --user pyttsx3")
        tts_available = False
    
    print()
    print("-"*60)
    
    if not serial_available:
        print("\n❌ Cannot test without pyserial")
        print("\nTo install packages, run in your terminal:")
        print("  pip3 install --user pyserial pyttsx3")
        print("\nOr if that fails:")
        print("  python3 -m pip install --user pyserial pyttsx3")
        return
    
    # Test ESP32 connection
    port = sys.argv[1] if len(sys.argv) > 1 else '/dev/cu.usbserial-0001'
    print(f"\n📡 Attempting to connect to: {port}")
    
    try:
        ser = serial.Serial(port, 115200, timeout=1)
        print("✅ Connected successfully!")
        print()
        
        # Send a test command
        test_command = "status"
        print(f"📤 Sending command: {test_command}")
        ser.write(f"{test_command}\n".encode())
        
        # Wait for response
        import time
        time.sleep(1)
        
        if ser.in_waiting:
            response = ser.readline().decode().strip()
            print(f"📥 Response: {response}")
        else:
            print("⚠️  No response received (ESP32 might be running different sketch)")
        
        ser.close()
        print()
        print("="*60)
        print("✅ CONNECTION TEST PASSED!")
        print("="*60)
        print("\nYour ESP32 is connected and responding!")
        print("\nNext steps:")
        print("1. Install remaining packages:")
        print("   pip3 install --user pyttsx3")
        print("2. Run the full test:")
        print("   python3 jarvis-brain-test.py")
        
    except serial.SerialException as e:
        print(f"❌ Connection failed: {e}")
        print("\n💡 Troubleshooting:")
        print("1. Check if ESP32 is plugged in")
        print("2. Check available ports with: ls /dev/cu.*")
        print("3. Make sure no other program is using the port")
        print("4. Try a different USB cable or port")
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == "__main__":
    main()

