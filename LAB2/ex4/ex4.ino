int led = 9;
int brightness = 255;
int fadeAmount = -5;
int speed = 30;               
unsigned long start = 0;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  start = millis();
}

void loop() {
  if (Serial.available()) {
    int val = Serial.parseInt();
    if (val > 0) speed = val;  
  }

  if (millis() - start >=speed) {
    start = millis();          

    analogWrite(led, brightness);

    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 255) {
      brightness=255;
    }
  }
}
