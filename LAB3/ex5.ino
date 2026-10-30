int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

const int butonA1 = A1;
const int butonA2 = A2;
const int butonA3 = A3;

String textScroller = "dECEbAL_Si_FiUL_SAU_LOCUiESC_PE_PLAiUL_dACiEi";
int pozitie = 0;
bool oprit = false;

//fct pt mapare caractere la segmente display
byte obtineCodCaracter(char c) {
  switch (c) {
    case '0': case 'O': case 'o': return ~0x3F;
    case '1': case 'i': case 'I': return ~0x06;
    case '2': return ~0x5B;
    case '3': return ~0x4F;
    case '4': return ~0x66;
    case '5': case 'S': case 's': return ~0x6D;
    case '6': return ~0x7D;
    case '7': return ~0x07;
    case '8': return ~0x7F;
    case '9': return ~0x6F;
    case 'A': case 'a': return ~0x77;
    case 'b': return ~0x7C;
    case 'C': case 'c': return ~0x39;
    case 'd': return ~0x5E;
    case 'E': case 'e': return ~0x79;
    case 'F': case 'f': return ~0x71;
    case 'L': case 'l': return ~0x38;
    case 'J': case 'j': return ~0x0E;
    case 'P': case 'p': return ~0x73;
    case 'U': case 'u': return ~0x3E;
    case '_': return ~0x08;
    case ' ': default: return ~0x00; //gol / stins
  }
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(butonA1, INPUT);
  pinMode(butonA2, INPUT);
  pinMode(butonA3, INPUT);
}

void loop() {
  //verif starea butoanelor pt pauza/pornire cu debounce
  if (digitalRead(butonA1) == HIGH || digitalRead(butonA2) == HIGH || digitalRead(butonA3) == HIGH) {
    oprit = !oprit;
    delay(300); //debounce
  }

  //afis fereastra curenta de 4 caractere
  afiseazaFereastra(pozitie);

  //aacă nu e pus pe pauza, avansam textul la un anumit interval de timp
  if (!oprit) {
    static unsigned long timpAnterior = 0;
    unsigned long timpCurent = millis();
    if (timpCurent - timpAnterior >= 350) {
      timpAnterior = timpCurent;
      pozitie++;
      if (pozitie > textScroller.length() - 4) {
        pozitie = 0; //reia scrolul de la capat
      }
    }
  }
}

//fct multiplexare pt cele 4 cifre/caractere
void afiseazaFereastra(int indexStart) {
  char c3 = textScroller.charAt(indexStart);     // Poziția 3 (st)
  char c2 = textScroller.charAt(indexStart + 1); // Poziția 2
  char c1 = textScroller.charAt(indexStart + 2); // Poziția 1
  char c0 = textScroller.charAt(indexStart + 3); // Poziția 0 (dr)

  byte caractere[4] = {
    obtineCodCaracter(c0),
    obtineCodCaracter(c1),
    obtineCodCaracter(c2),
    obtineCodCaracter(c3)
  };
  
  byte mastiPozitii[4] = {~0x01, ~0x02, ~0x04, ~0x08};

  for (int i = 0; i < 4; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, caractere[i]);
    shiftOut(dataPin, clockPin, LSBFIRST, mastiPozitii[i]);
    digitalWrite(latchPin, HIGH);
    delay(3);
  }
}