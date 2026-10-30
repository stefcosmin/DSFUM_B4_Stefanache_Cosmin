int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

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

const int TOTAL_RUNDE = 10;

void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long timpMinim = 0xFFFFFFFF;
  unsigned long timpMaxim = 0;
  unsigned long timpTotal = 0;
  int valMinima = 0;
  int valMaximativa = 0;

  for (int runda = 1; runda <= TOTAL_RUNDE; runda++) {
    int numarAfisat = random(1000, 10000);
    
    while (Serial.available() > 0) {
      Serial.read();
    }

    Serial.print("Runda ");
    Serial.print(runda);
    Serial.println(": Introduceți numărul afișat!");

    unsigned long startTimp = millis();
    String inputUtilizator = "";

    while (true) {
      afiseazaNumar(numarAfisat);

      if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
          if (inputUtilizator.length() >= 4) {
            break;
          }
        } else if (isDigit(c)) {
          inputUtilizator += c;
          if (inputUtilizator.length() == 4) {
            break;
          }
        }
      }
    }

    unsigned long sfarsitTimp = millis();
    unsigned long timpReactie = sfarsitTimp - startTimp;

    timpTotal += timpReactie;

    if (timpReactie < timpMinim) {
      timpMinim = timpReactie;
      valMinima = numarAfisat;
    }

    if (timpReactie > timpMaxim) {
      timpMaxim = timpReactie;
      valMaximativa = numarAfisat;
    }

    Serial.print("Timp inregistrat: ");
    Serial.print(timpReactie);
    Serial.println(" ms\n");

    delay(1000); //pauza intre runde
  }

  // Afișarea rezultatelor finale pe portul serial
  Serial.println("REZULTATE FINALE");
  Serial.print("Timp minim: ");
  Serial.print(timpMinim);
  Serial.print(" ms (obtinut pt val ");
  Serial.print(valMinima);
  Serial.println(")");

  Serial.print("Timpul maxim: ");
  Serial.print(timpMaxim);
  Serial.print(" ms (obtinut pt val ");
  Serial.print(valMaximativa);
  Serial.println(")");

  Serial.print("Timp mediu: ");
  Serial.print((float)timpTotal / TOTAL_RUNDE);
  Serial.println(" ms");

  while (true) {
	//blocare dupa 10 runde
  }
}

void afiseazaNumar(int num) {
  int mii = num / 1000;
  int sute = (num / 100) % 10;
  int zeci = (num / 10) % 10;
  int unitati = num % 10;

  int cifreAfisate[4] = {unitati, zeci, sute, mii};
  byte mastiPozitii[4] = {~0x01, ~0x02, ~0x04, ~0x08};

  for (int i = 0; i < 4; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, cifre[cifreAfisate[i]]);
    shiftOut(dataPin, clockPin, LSBFIRST, mastiPozitii[i]);
    digitalWrite(latchPin, HIGH);
    delay(4);
  }
}