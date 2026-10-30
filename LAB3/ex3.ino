int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

int buttonIncPin = A1;
int buttonDecPin = A2;
int buttonResetPin = A3;

int counter = 0;

byte cifre[] = {
  ~0x3F, // 0
  ~0x06, // 1
  ~0x5B, // 2
  ~0x4F, // 3
  ~0x66, // 4
  ~0x6D, // 5
  ~0x7D, // 6
  ~0x07, // 7
  ~0x7F, // 8
  ~0x6F  // 9
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(buttonIncPin, INPUT_PULLUP);
  pinMode(buttonDecPin, INPUT_PULLUP);
  pinMode(buttonResetPin, INPUT_PULLUP);
}

void loop() {
  //citire butoane
  if (digitalRead(buttonIncPin) == LOW) {
    if (counter < 9999) {
      counter++;
    }
    delay(200); //debounce
  }

  if (digitalRead(buttonDecPin) == LOW) {
    if (counter > 0) {
      counter--;
    }
    delay(200);
  }

  if (digitalRead(buttonResetPin) == LOW) {
    counter = 0;
    delay(200);
  }

  //afisare continua
  afiseazaCounter(counter);
}

void afiseazaCounter(int valoare) {
  int mii = valoare / 1000;
  int sute = (valoare / 100) % 10;
  int zeci = (valoare / 10) % 10;
  int unitati = valoare % 10;

  //gestionare zerouri nesemnificative
  byte miiByte = (mii == 0) ? 0xFF : cifre[mii];
  byte suteByte = (valoare < 100) ? 0xFF : cifre[sute];
  if (valoare < 1000 && mii == 0) suteByte = 0xFF; // Eliminare leading zero pentru sute
  byte zeciByte = (valoare < 10) ? 0xFF : cifre[zeci];
  byte unitatiByte = cifre[unitati];

  int cifreAfisate[4] = {unitatiByte, zeciByte, suteByte, miiByte};
  byte mastiPozitii[4] = {~0x01, ~0x02, ~0x04, ~0x08};

  for (int i = 0; i < 4; i++) {
    //daca o pozitie superioara nu se afiseaza-> sarim peste mască sau se trimite un byte care dezactiveaza digitul
    if (cifreAfisate[i] != 0xFF) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, cifreAfisate[i]);
      shiftOut(dataPin, clockPin, LSBFIRST, mastiPozitii[i]);
      digitalWrite(latchPin, HIGH);
      delay(4);
    }
  }
}