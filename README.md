# Arduino Piano

A digital music synthesizer with melody recording and playback functionality, built on the Arduino platform.

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)

## Table of Contents

- [About the Project](#about-the-project)
- [Features](#features)
- [Components](#components)
- [Wiring Diagram](#wiring-diagram)
- [Installation](#installation)
- [Usage Instructions](#usage-instructions)
- [Tinkercad Simulation](#tinkercad-simulation)
- [Authors](#authors)

## About the Project

**Arduino Piano** is a functional musical instrument based on Arduino that enables playing simple melodies as well as recording and playback. The project combines elements of digital electronics, microcontroller programming, and audio signal processing.

### Main Features:
- 5 buttons representing notes of the C major scale (C, D, E, F, G)
- Recording mode with visualization (red LED)
- Playback mode for saved melodies (green LED)
- 7-segment display showing the currently played note
- Sound generation through piezo buzzer
- Ability to save up to 100 notes

### Circuit Layout

![Arduino Piano Circuit](tinkercad.png)
*Figure 1: Complete circuit layout in Tinkercad showing all components and connections*

![Arduino Piano Schematic](schematic_view.png)
*Figure 2: Schematic diagram showing electrical connections and component relationships*

## Features

### 1. Normal Play Mode
- Press note buttons to play melodies in real-time
- Each note is displayed on the LED display
- Generation of sounds at appropriate frequencies (261-392 Hz)

### 2. Melody Recording
- Press the **RECORD** button to start recording
- The red LED lights up
- Play the melody - all notes are saved with duration
- Press **RECORD** again to finish

### 3. Melody Playback
- Press the **PLAY** button to play the saved melody
- The green LED lights up
- The device automatically plays all saved notes
- Visualization of each note on the display

### 4. Serial Port Debugging
- Real-time application state monitoring
- Button press logging
- Information about the number of saved notes

![Serial Monitor Console](console.png)
*Figure 3: Serial monitor output showing real-time logging of button presses, recording, and playback*

## Components

| Component | Quantity | Description |
|-----------|----------|-------------|
| Arduino Uno R3 (U1) | 1 | ATmega328P Microcontroller |
| Piezo Buzzer (PIEZO1) | 1 | Sound Generator |
| Push Buttons (S1-S5, SPLAY, S8) | 7 | 5x notes + RECORD + PLAY |
| 560Ω Resistors (R1-R6) | 6 | Button pull-down resistors |
| 1kΩ Resistors (R7-R8) | 2 | LED current limiting resistors |
| Red LED (D4) | 1 | Recording Indicator |
| Green LED (D5) | 1 | Playback Indicator |
| 7-segment LED Display (U2) | 1 | Red 4-digit I2C (0x70) HT16K33 controller |
| 250kΩ Potentiometer (Rpot2) | 1 | Optional - volume/brightness control |
| Breadboard + wires | 1 | Prototype assembly |

## Wiring Diagram

```
┌─────────────────────────────────────────┐
│           ARDUINO UNO/NANO              │
├─────────────────────────────────────────┤
│ D2  → Piezo Buzzer (Audio PWM)          │
│ D5  → Button Note G                     │
│ D6  → Button Note F                     │
│ D7  → Button Note E                     │
│ D8  → Button Note D                     │
│ D9  → Button Note C                     │
│ D10 → Red LED (Recording)               │
│ D11 → Green LED (Playback)              │
│ A2  → Button PLAY                       │
│ A3  → Button RECORD                     │
│ A4  → LED Display (SDA - I2C)           │
│ A5  → LED Display (SCL - I2C)           │
│ GND → Common Ground                     │
│ 5V  → Display and LED Power             │
└─────────────────────────────────────────┘
```

### Note Frequencies

| Note | Frequency | Pin |
|------|-----------|-----|
| C | 261.63 Hz | D9 |
| D | 293.66 Hz | D8 |
| E | 329.63 Hz | D7 |
| F | 349.23 Hz | D6 |
| G | 392.00 Hz | D5 |

## Installation

### Requirements
- Arduino IDE (version 1.8.x or newer)
- `Wire.h` library (built into Arduino IDE)
- USB cable for programming Arduino

### Installation Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/your-username/arduino-piano.git
   cd arduino-piano
   ```

2. **Open the project in Arduino IDE:**
   ```
   File → Open → source.ino
   ```

3. **Select the board:**
   ```
   Tools → Board → Arduino Uno (or Nano)
   ```

4. **Select the COM port:**
   ```
   Tools → Port → [select USB port]
   ```

5. **Upload the code:**
   ```
   Sketch → Upload (Ctrl+U)
   ```

6. **Open Serial Monitor:**
   ```
   Tools → Serial Monitor (9600 baud)
   ```

## Usage Instructions

### First Launch
1. Connect Arduino to the computer via USB
2. Wait for the message `"Piano ready!"` in the Serial Monitor
3. The device is ready to use

### Playing Melodies
```
1. Press note buttons (C, D, E, F, G)
2. Watch the LED display - it shows the current note
3. Listen to sounds from the piezo buzzer
```

### Recording
```
1. Press RECORD (red LED lights up)
2. Play the melody
3. Press RECORD again (LED turns off)
4. Serial Monitor shows: "*** RECORDING FINISHED - saved X notes ***"
```

### Playback
```
1. Press PLAY (green LED lights up)
2. The device plays the saved melody
3. After completion, the LED turns off
```

> **Note:** If there is no saved melody, you will hear a double error signal (200 Hz).

## Tinkercad Simulation

The project can be tested without physical components in the Autodesk Tinkercad environment:

**Link: [Open project in Tinkercad](https://www.tinkercad.com/things/eABRWKvcff0)**

### Simulation Advantages:
- Testing the circuit without hardware
- Virtual connection debugging
- Interactive 3D visualization
- Safe experimentation
