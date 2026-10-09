const int piniLed[] = {8, 9, 10, 11, 12, 13};
const int numLeduri = 6;

const unsigned long perioadaUs = 2000;

float tensiuni[numLeduri] = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
float pasiTensiune[numLeduri] = {-0.01, 0.02, -0.03, 0.04, -0.05, 0.06};

unsigned long timpInceputCiclu = 0;

void setup() {
  for (int i = 0; i < numLeduri; i++) {
    pinMode(piniLed[i], OUTPUT);
  }
  timpInceputCiclu = micros();
}

void loop() {
  unsigned long timpCurent = micros();
  unsigned long timpTrecut = timpCurent - timpInceputCiclu;

  if (timpTrecut >= perioadaUs) {
    timpInceputCiclu = timpCurent;
    timpTrecut = 0;

    for (int i = 0; i < numLeduri; i++) {
      tensiuni[i] += pasiTensiune[i];

      if (tensiuni[i] <= 0.0) {
        tensiuni[i] = 0.0;
        pasiTensiune[i] = -pasiTensiune[i];
      } else if (tensiuni[i] >= 5.0) {
        tensiuni[i] = 5.0;
        pasiTensiune[i] = -pasiTensiune[i];
      }
    }
  }

  for (int i = 0; i < numLeduri; i++) {
    unsigned long timp = (tensiuni[i] / 5.0) * perioadaUs;
    if (timpTrecut < timp) {
      digitalWrite(piniLed[i], 1);
    } else {
      digitalWrite(piniLed[i], 0);
    }
  }
}
