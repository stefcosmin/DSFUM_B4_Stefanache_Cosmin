int p = 13;
int m = 0;

void setup() {
  Serial.begin(9600);
  pinMode(p, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    int v = Serial.parseInt();
    if (v == 1) {
      m = 1;
      Serial.println("pornit");
    } else if (v == 2) {
      m = 2;
      Serial.println("oprit");
    } else if (v == 3) {
      m = 3;
      Serial.println("blink");
    }
  }

  if (m == 1) {
    digitalWrite(p, 1);
  } else if (m == 2) {
    digitalWrite(p, LOW);
  } else if (m == 3) {
    digitalWrite(p, 1);
    delay(200);
    digitalWrite(p, 0);
    delay(200);
  }
}
