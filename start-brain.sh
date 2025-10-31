#!/bin/bash
# Quick start script for JARVIS Brain

echo "🤖 Starting JARVIS Brain..."
echo ""

# Find ESP32 port
PORT=$(ls /dev/cu.* | grep -i usb | head -n 1)

if [ -z "$PORT" ]; then
    echo "❌ ESP32 not found!"
    echo "💡 Please connect your ESP32 via USB"
    exit 1
fi

echo "✅ Found ESP32 at: $PORT"
echo ""

# Check if Python packages are installed
echo "📦 Checking Python packages..."
pip3 list | grep -q pyserial || echo "⚠️  Missing: pyserial"
pip3 list | grep -q SpeechRecognition || echo "⚠️  Missing: SpeechRecognition"
pip3 list | grep -q pyttsx3 || echo "⚠️  Missing: pyttsx3"
pip3 list | grep -q PyAudio || echo "⚠️  Missing: PyAudio"

echo ""
echo "💡 If packages are missing, run:"
echo "   pip3 install -r requirements.txt"
echo ""
echo "🚀 Starting..."
echo ""

# Run the brain
python3 jarvis-brain.py "$PORT"

