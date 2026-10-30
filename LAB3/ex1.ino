int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

byte cifre[] = {
  ~0x3F,
  ~0x06,
  ~0x5B,
  ~0x4F,
  ~0x66,
  ~0x6D,
  ~0x7D,
  ~0x07,
  ~0x7F,
  ~0x6F
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, cifre[i]);
    shiftOut(dataPin, clockPin, LSBFIRST, ~0x01);
    digitalWrite(latchPin, HIGH);
    delay(1000);
  }
}