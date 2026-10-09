const int pinIesire = 13;
const unsigned long perioadaUs = 2000;

float tensiune = 0.0;
unsigned long timpInceput = 0;

void setup() {
  pinMode(pinIesire, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  if (Serial.available() > 0) {
    float val = Serial.parseFloat();
    if (val >= 0.0 && val <= 5.0) {
      tensiune = val;
    }
  }

  unsigned long timp = (tensiune / 5.0) * perioadaUs;

  unsigned long timpCurent = micros();
  unsigned long timpTrecut = timpCurent - timpInceput;

  if (timpTrecut >= perioadaUs) {
    timpInceput = timpCurent;
    timpTrecut = 0;
  }

  if (timpTrecut < timp) {
    digitalWrite(pinIesire, HIGH);
  } else {
    digitalWrite(pinIesire, LOW);
  }
}
