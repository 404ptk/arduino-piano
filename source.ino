#include <Wire.h>
#include <LedControl.h>  // MAX7219

// MAX7219 pin definitions
#define MAX_DIN 11
#define MAX_CLK 13
#define MAX_CS  10

// Create LedControl object
LedControl lc = LedControl(MAX_DIN, MAX_CLK, MAX_CS, 1);

// 8x8 font for notes
byte charC[] = {0b00111100,0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b00111100,0};
byte charD[] = {0b01111110,0b10000010,0b10000010,0b10000010,0b10000010,0b10000010,0b01111110,0};
byte charE[] = {0b11111111,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b11111111,0};
byte charF[] = {0b11111111,0b10000000,0b10000000,0b11110000,0b10000000,0b10000000,0b10000000,0};
byte charG[] = {0b00111110,0b01000001,0b01000001,0b01001001,0b01001001,0b01001001,0b00111111,0};
byte charBlank[] = {0,0,0,0,0,0,0,0};


void initDisplay() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

// Show note letter
void showNoteChar(char n) {
  byte* data = charBlank;

  switch (n) {
    case 'C': data = charC; break;
    case 'D': data = charD; break;
    case 'E': data = charE; break;
    case 'F': data = charF; break;
    case 'G': data = charG; break;
    default:  data = charBlank; break;
  }

  for (int col = 0; col < 8; col++) {
    lc.setColumn(0, col, data[col]);
  }
}

// Note button pins
int cNote = 9;  // C
int dNote = 8;  // D
int eNote = 7;  // E
int fNote = 6;  // F
int gNote = 5;  // G

// Control pins
int Piezo = 2;
int recordButton = A3;
int playButton = A2;

int redLED = A1;     // WAS 10
int greenLED = A0;   // WAS 11

// Note frequencies
double c = 261.63;
double d = 293.66;
double e = 329.63;
double f = 349.23;
double g = 392;
double a = 440;

// Structure for storing melody
#define MAX_NOTES 100
struct Note {
  double frequency;
  unsigned long duration;
};

Note melody[MAX_NOTES];
int noteCount = 0;
bool isRecording = false;
bool recordButtonPressed = false;
bool playButtonPressed = false;
unsigned long noteStartTime = 0;
double currentFrequency = 0;
bool noteIsPressed = false;  // Track if any note button is currently pressed

void setup() {
  // Note buttons
  pinMode(cNote, INPUT);
  pinMode(dNote, INPUT);
  pinMode(eNote, INPUT);
  pinMode(fNote, INPUT);
  pinMode(gNote, INPUT);
  
  // Control buttons
  pinMode(recordButton, INPUT_PULLUP);
  pinMode(playButton, INPUT_PULLUP);

  // LEDs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  pinMode(Piezo, OUTPUT);

  Serial.begin(9600);

  initDisplay();
  showNoteChar(' '); // blank on start

  Serial.println("Piano ready!");
}

void loop() {
  // Handle RECORD button
  if (digitalRead(recordButton) == LOW && !recordButtonPressed) {
    recordButtonPressed = true;
    isRecording = !isRecording;
    
    if (isRecording) {
      noteCount = 0;
      currentFrequency = 0;
      noteIsPressed = false;
      digitalWrite(redLED, HIGH);  // Turn on red LED
      Serial.println("*** RECORDING STARTED ***");
    } else {
      // Save last note if still playing
      if (currentFrequency > 0 && noteCount < MAX_NOTES) {
        melody[noteCount].frequency = currentFrequency;
        melody[noteCount].duration = millis() - noteStartTime;
        noteCount++;
      }
      digitalWrite(redLED, LOW);   // Turn off red LED
      Serial.print("*** RECORDING FINISHED - saved ");
      Serial.print(noteCount);
      Serial.println(" notes ***");
      currentFrequency = 0;
      noteIsPressed = false;
    }
    delay(200);
  }
  if (digitalRead(recordButton) == HIGH) recordButtonPressed = false;

  // ---------------- PLAY ----------------
  if (digitalRead(playButton) == LOW && !playButtonPressed) {
    playButtonPressed = true;
    playMelody();
    delay(200);
  }
  if (digitalRead(playButton) == HIGH) playButtonPressed = false;

  // ---------------- NOTES ----------------
  double freq = 0;
  char noteChar = ' ';
  bool buttonPressed = false;

  if (digitalRead(cNote) == 1) {
    freq = c; noteChar = 'C'; buttonPressed = true;
  } else if (digitalRead(dNote) == 1) {
    freq = d; noteChar = 'D'; buttonPressed = true;
  } else if (digitalRead(eNote) == 1) {
    freq = e; noteChar = 'E'; buttonPressed = true;
  } else if (digitalRead(fNote) == 1) {
    freq = f; noteChar = 'F'; buttonPressed = true;
  } else if (digitalRead(gNote) == 1) {
    freq = g; noteChar = 'G'; buttonPressed = true;
  }

  // Show note on display
  showNoteChar(noteChar);

  // Sound and recording logic
  if (freq > 0) {
    // Button is pressed
    tone(Piezo, freq);
    
    if (isRecording) {
      // If this is a new note press (not continuation)
      if (!noteIsPressed) {
        // Save previous note if exists
        if (currentFrequency > 0 && noteCount < MAX_NOTES) {
          melody[noteCount].frequency = currentFrequency;
          melody[noteCount].duration = millis() - noteStartTime;
          noteCount++;
          Serial.print("Recorded note ");
          Serial.print(noteCount);
          Serial.print(": ");
          Serial.println(noteChar);
        }
        // Start new note
        currentFrequency = freq;
        noteStartTime = millis();
        noteIsPressed = true;
      } else if (freq != currentFrequency) {
        // Different note pressed - save previous and start new
        if (noteCount < MAX_NOTES) {
          melody[noteCount].frequency = currentFrequency;
          melody[noteCount].duration = millis() - noteStartTime;
          noteCount++;
          Serial.print("Recorded note ");
          Serial.print(noteCount);
          Serial.print(": ");
          Serial.println(noteChar);
        }
        currentFrequency = freq;
        noteStartTime = millis();
      }
    } else {
      // Normal play mode (not recording)
      if (!noteIsPressed) {
        Serial.print("Playing note: ");
        Serial.println(noteChar);
        noteIsPressed = true;
      } else if (freq != currentFrequency) {
        // Different note pressed
        Serial.print("Playing note: ");
        Serial.println(noteChar);
        currentFrequency = freq;
      }
      currentFrequency = freq;
    }
  } else {
    // No button pressed
    noTone(Piezo);
    
    if (isRecording && noteIsPressed) {
      // Save the note that was just released
      if (currentFrequency > 0 && noteCount < MAX_NOTES) {
        melody[noteCount].frequency = currentFrequency;
        melody[noteCount].duration = millis() - noteStartTime;
        noteCount++;
        
        // Find note name for logging
        char lastNote = '?';
        if (currentFrequency == c) lastNote = 'C';
        else if (currentFrequency == d) lastNote = 'D';
        else if (currentFrequency == e) lastNote = 'E';
        else if (currentFrequency == f) lastNote = 'F';
        else if (currentFrequency == g) lastNote = 'G';
        
        Serial.print("Recorded note ");
        Serial.print(noteCount);
        Serial.print(": ");
        Serial.println(lastNote);
      }
      currentFrequency = 0;
      noteIsPressed = false;
    } else if (!isRecording && noteIsPressed) {
      // Normal mode - note released
      currentFrequency = 0;
      noteIsPressed = false;
    }
  }
  
  delay(10);
}

void playMelody() {
  if (noteCount == 0) {
    Serial.println("No recorded melody!");
    // Audio signal
    tone(Piezo, 200, 100);
    delay(150);
    tone(Piezo, 200, 100);
    return;
  }
  
  Serial.println("*** PLAYBACK (Tempo 0.5s) ***");
  digitalWrite(greenLED, HIGH);  // Turn on green LED
  noTone(Piezo);
  delay(500);
  
  for (int i = 0; i < noteCount; i++) {
    char noteChar = ' ';

    if (melody[i].frequency == c) noteChar = 'C';
    if (melody[i].frequency == d) noteChar = 'D';
    if (melody[i].frequency == e) noteChar = 'E';
    if (melody[i].frequency == f) noteChar = 'F';
    if (melody[i].frequency == g) noteChar = 'G';

    showNoteChar(noteChar);
    
    // Print currently playing note
    Serial.print("Playing note: ");
    Serial.println(noteChar);

    tone(Piezo, melody[i].frequency);
    delay(400);
    noTone(Piezo);
    delay(100);
  }

  digitalWrite(greenLED, LOW);
  showNoteChar(' ');
  Serial.println("*** PLAYBACK FINISHED ***");
}
