void setup() {
  // put your setup code here, to run once:
  for(int x=8;x<=13;x++){pinMode(x,OUTPUT);}
  for(int i=0;i<64;i++){
  for(int led=0;led<6;led++)      delay(100);
  {
    digitalWrite(led+8,(i>>led)&1);
  }
  }
}

void loop() {


    
}
