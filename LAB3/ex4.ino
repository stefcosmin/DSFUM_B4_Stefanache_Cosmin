int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

//codurile pentru literele A, b, C, d, E, F (Active-Low)
byte litere[] = {
  ~0x77, // A
  ~0x7C, // b
  ~0x39, // C
  ~0x5E, // d
  ~0x79, // E
  ~0x71  // F
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < 6; i++) {
    unsigned long startTime = millis();
    
    //nentine litera afisata pe poz din st pt 5s
    while (millis() - startTime < 5000) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, litere[i]);
      shiftOut(dataPin, clockPin, LSBFIRST, ~0x08); //masca pt poz cea mai din st
      digitalWrite(latchPin, HIGH);
      delay(4);
    }
  }
}