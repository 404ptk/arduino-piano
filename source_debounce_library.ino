#include <Wire.h>
#include <Bounce2.h>

#define HT_ADDR 0x70

// Segments for letters - mapped to 7-segment display
uint8_t segBlank = 0x00;
uint8_t segC = 0b00111001;  // C
uint8_t segD = 0b01011110;  // D (lowercase)
uint8_t segE = 0b01111001;  // E
uint8_t segF = 0b01110001;  // F
uint8_t segG = 0b01111101;  // G

// Initialize HT16K33
void initDisplay() {
  Wire.begin();

  Wire.beginTransmission(HT_ADDR);
  Wire.write(0x21);     // enable oscillator
  Wire.endTransmission();

  Wire.beginTransmission(HT_ADDR);
  Wire.write(0x81);     // enable display
  Wire.endTransmission();

  Wire.beginTransmission(HT_ADDR);
  Wire.write(0xEF);     // max brightness
  Wire.endTransmission();
}

// Send segments to 4 digits
void sendSegments(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) {
  Wire.beginTransmission(HT_ADDR);
  Wire.write(0x00); // RAM address

  Wire.write(d0); Wire.write(0x00);
  Wire.write(d1); Wire.write(0x00);
  Wire.write(d2); Wire.write(0x00);
  Wire.write(d3); Wire.write(0x00);

  Wire.endTransmission();
}

// Show note letter
void showNoteChar(char n) {
  uint8_t s;

  switch(n) {
    case 'C': s = segC; break;
    case 'D': s = segD; break;
    case 'E': s = segE; break;
    case 'F': s = segF; break;
    case 'G': s = segG; break;
    default:  s = segBlank;
  }

  sendSegments(s, segBlank, segBlank, segBlank);
}

// Note button pins
int cNote = 9;  // C
int dNote = 8;  // D
int eNote = 7;  // E
int fNote = 6;  // F
int gNote = 5;  // G

// Control pins
int Piezo = 2;
int recordButton = A3;  // RECORD button
int playButton = A2;    // PLAY button
int redLED = 10;        // Red LED - recording
int greenLED = 11;      // Green LED - playback

// Bounce objects for each button
Bounce cBounce = Bounce();
Bounce dBounce = Bounce();
Bounce eBounce = Bounce();
Bounce fBounce = Bounce();
Bounce gBounce = Bounce();
Bounce recordBounce = Bounce();
Bounce playBounce = Bounce();

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
unsigned long noteStartTime = 0;
double currentFrequency = 0;
bool noteIsPressed = false;  // Track if any note button is currently pressed

void setup() {
  // Note buttons - attach and setup debouncing
  cBounce.attach(cNote, INPUT);
  cBounce.interval(5); // 5ms debounce
  
  dBounce.attach(dNote, INPUT);
  dBounce.interval(5);
  
  eBounce.attach(eNote, INPUT);
  eBounce.interval(5);
  
  fBounce.attach(fNote, INPUT);
  fBounce.interval(5);
  
  gBounce.attach(gNote, INPUT);
  gBounce.interval(5);
  
  // Control buttons - INPUT_PULLUP
  recordBounce.attach(recordButton, INPUT_PULLUP);
  recordBounce.interval(25); // longer debounce for control buttons
  
  playBounce.attach(playButton, INPUT_PULLUP);
  playBounce.interval(25);

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
  // Update all bounce objects
  cBounce.update();
  dBounce.update();
  eBounce.update();
  fBounce.update();
  gBounce.update();
  recordBounce.update();
  playBounce.update();

  // Handle RECORD button
  if (recordBounce.fell()) {  // Button pressed (LOW with pullup)
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
  }

  // Handle PLAY button
  if (playBounce.fell()) {  // Button pressed
    playMelody();
  }

  // ---------------- NOTES ----------------
  double freq = 0;
  char noteChar = ' ';
  bool buttonPressed = false;

  // Check which note button is pressed
  if (cBounce.read() == HIGH) {
    freq = c; noteChar = 'C'; buttonPressed = true;
  } else if (dBounce.read() == HIGH) {
    freq = d; noteChar = 'D'; buttonPressed = true;
  } else if (eBounce.read() == HIGH) {
    freq = e; noteChar = 'E'; buttonPressed = true;
  } else if (fBounce.read() == HIGH) {
    freq = f; noteChar = 'F'; buttonPressed = true;
  } else if (gBounce.read() == HIGH) {
    freq = g; noteChar = 'G'; buttonPressed = true;
  }

  // Show note on display
  showNoteChar(noteChar);

  // Sound and recording logic
  if (freq > 0) {
    // Button is pressed
    tone(Piezo, freq);
    
    if (isRecording) {
      // If this is a new note press
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
        // Different note pressed
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
      // Normal play mode
      if (!noteIsPressed) {
        Serial.print("Playing note: ");
        Serial.println(noteChar);
        noteIsPressed = true;
      } else if (freq != currentFrequency) {
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
      currentFrequency = 0;
      noteIsPressed = false;
    }
  }
}

void playMelody() {
  if (noteCount == 0) {
    Serial.println("No recorded melody!");
    tone(Piezo, 200, 100);
    delay(150);
    tone(Piezo, 200, 100);
    return;
  }
  
  Serial.println("*** PLAYBACK (Tempo 0.5s) ***");
  digitalWrite(greenLED, HIGH);
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
