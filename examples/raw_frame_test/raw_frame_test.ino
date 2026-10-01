#define SIGNAL_PIN 2

volatile unsigned long lastTime=0;
volatile byte lastState=LOW;
volatile bool frameActive=false, frameReady=false;
volatile byte bitCount=0, bits[17];

void signalISR(){
  unsigned long now=micros(), duration=now-lastTime;
  byte newState=digitalRead(SIGNAL_PIN), finishedLevel=lastState;
  lastState=newState; lastTime=now;

  if(finishedLevel==LOW && duration>25000UL && duration<40000UL){
    if(frameActive && bitCount==17 && !frameReady) frameReady=true;
    frameActive=true; bitCount=0; return;
  }
  if(!frameActive) return;
  if(finishedLevel==HIGH && duration>700UL && duration<1200UL){ bitCount=0; return; }

  if(finishedLevel==HIGH){
    byte value;
    if(duration>=50UL && duration<=130UL) value=0;
    else if(duration>=160UL && duration<=270UL) value=1;
    else return;
    if(bitCount<17) bits[bitCount++]=value;
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(SIGNAL_PIN,INPUT);
  lastState=digitalRead(SIGNAL_PIN); lastTime=micros();
  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN),signalISR,CHANGE);
  Serial.println(F("UNIVERSAL PARK SENSOR RAW TEST"));
  Serial.println(F("SIGNAL=D2 / Serial=115200"));
}

void loop(){
  if(!frameReady) return;
  byte b[17];
  noInterrupts();
  for(byte i=0;i<17;i++) b[i]=bits[i];
  frameReady=false;
  interrupts();

  Serial.print(F("FRAME: "));
  for(byte i=0;i<17;i++){
    Serial.print(b[i]);
    if(i==0||i==4||i==8||i==12) Serial.print(' ');
  }

  if(b[0]==1&&b[1]==0&&b[2]==0&&b[3]==0&&b[4]==0){
    uint16_t raw=0;
    for(byte i=5;i<17;i++){ raw=(raw<<1)|b[i]; }
    if(raw<0x800){
      byte ch=raw>>9;
      uint16_t dr=raw&0x1FF;
      Serial.print(F(" | SENSOR: "));
      Serial.print((char)('A'+ch));
      Serial.print(F(" | RAW: ")); Serial.print(raw);
      Serial.print(F(" | MESAFE: ")); Serial.print(dr/2.0,1);
      Serial.print(F(" cm"));
    }
  }
  Serial.println();
}
