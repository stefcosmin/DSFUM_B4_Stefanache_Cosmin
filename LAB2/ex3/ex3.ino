int led = 13;
int brightness = 255;
int fadeAmount = -5;
int period = 20;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  int onTime = (brightness * period) / 255;
  int offTime = period - onTime;
  
  for (int i = 0; i < 2; i++) {
    if (onTime > 0) {
      digitalWrite(led, HIGH);
      delayMicroseconds(onTime * 1000);
    }
    if (offTime > 0) {
      digitalWrite(led, LOW);
      delayMicroseconds(offTime * 1000);
    }
  }
  
  brightness += fadeAmount;
  
  if (brightness <= 0) {
    brightness = 255;
  }
}
