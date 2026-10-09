const int pinIesire = 13;

const unsigned long perioadaUs = 2000; 

float tensiune = 5.0;
float pasTensiune = -0.1;

void setup() {
  pinMode(pinIesire, OUTPUT);
}

void loop() {

  unsigned long timpOnUs = (tensiune / 5.0) * perioadaUs;

  for (int i = 0; i < 10; i++) {
    unsigned long timpInceput = micros();

    while (micros() - timpInceput < perioadaUs) {
      if (micros() - timpInceput < timpOnUs) {
        digitalWrite(pinIesire, 1);
      } else {
        digitalWrite(pinIesire, 0);
      }
    }
  }


  tensiune += pasTensiune;


  if (tensiune <= 0.0) {
    tensiune = 0.0;
    pasTensiune = 0.1;
  } else if (tensiune >= 5.0) {
    tensiune = 5.0;
    pasTensiune = -0.1;
  }
}
