# Smart Pill Box 💊

A smart medication reminder system designed to help elderly patients manage their daily medication schedule.

## Overview

The Smart Pill Box is a university project that combines electronics, programming, 3D printing and artificial intelligence. It alerts the patient at scheduled medication times, automatically detects if the pill box was opened, and sends a Telegram notification to a caregiver if the medication is not taken within 30 minutes.

## Features

- 💡 Red LED lights up at each scheduled medication time
- 🔊 Active buzzer sounds for 10 seconds as an alert
- 🔍 Reed switch automatically detects when the lid is opened
- 📱 Telegram notification sent to caregiver if medication is missed
- 🕐 Real-time date and time displayed on LCD screen
- ✅ Buzzer confirmation when medication is taken

## Medication Schedule

| Slot   | Time  | LED Pin |
|--------|-------|---------|
| Morning | 08:00 | Pin 9  |
| Noon    | 13:00 | Pin 10 |
| Evening | 20:00 | Pin 11 |

## Hardware Components

- Arduino Uno
- 3x Red LEDs + 220 Ohm resistors
- 1x Reed switch (Pin 2)
- 1x Active buzzer (Pin 8)
- 1x LCD 16x2 with I2C module (A4/A5)
- Breadboard + jumper wires
- Custom 3D printed pill box (7 days x 3 slots = 21 compartments)

## How It Works

## Setup

### Arduino
1. Open `SPBarduino.ino` in Arduino IDE
2. Upload to Arduino Uno via USB
3. Make sure port is set to the correct COM port

### Python
1. Install dependencies:
```bash
pip install pyserial requests
```
2. Edit `pillbox.py` and set your:
   - `PORT` (e.g. COM7)
   - `TELEGRAM_TOKEN`
   - `TELEGRAM_CHAT_ID`
3. Run:
```bash
python pillbox.py
```

## AI Tools Used

| Tool | Role |
|------|------|
| Claude AI (Anthropic) | Code generation, debugging, documentation |
| Wokwi Simulator | Circuit and code simulation before physical build |

## Project Structure

## University Project
**Year:** 2025/2026  
**Field:** 1ACIF
