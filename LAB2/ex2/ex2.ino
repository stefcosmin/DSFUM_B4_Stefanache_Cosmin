int i=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int x=8;x<=13;x++){pinMode(x,OUTPUT);}
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    i = Serial.parseInt();
    
  }
    for(int led=0;led<6;led++)
    {
      digitalWrite(led+8,(i>>led)&1);
    }
    
}
