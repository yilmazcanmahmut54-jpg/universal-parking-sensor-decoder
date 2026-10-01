/*
  Universal Parking Sensor ECU Decoder
  Tested: Arduino
  SIGNAL: D2 (INT0)
  Serial: 115200

  Empirically reverse-engineered protocol.
*/

#define SIGNAL_PIN 2

volatile unsigned long lastEdgeUs = 0;
volatile uint8_t lastState = LOW;
volatile bool collecting = false;
volatile uint8_t bitCount = 0;
volatile uint32_t workingFrame = 0;
volatile uint32_t readyFrame = 0;
volatile bool frameReady = false;

float distanceCm[4] = {0,0,0,0};
unsigned long lastSensorMs[4] = {0,0,0,0};

void signalISR() {
  unsigned long now = micros();
  unsigned long duration = now - lastEdgeUs;
  uint8_t newState = digitalRead(SIGNAL_PIN);
  uint8_t finishedLevel = lastState;

  lastState = newState;
  lastEdgeUs = now;

  // Frame separator / Cerceve ayirici
  if (finishedLevel == LOW && duration >= 25000UL && duration <= 40000UL) {
    if (collecting && bitCount == 17 && !frameReady) {
      readyFrame = workingFrame;
      frameReady = true;
    }
    collecting = true;
    bitCount = 0;
    workingFrame = 0;
    return;
  }

  if (!collecting) return;

  // Header
  if (finishedLevel == HIGH && duration >= 700UL && duration <= 1200UL) {
    bitCount = 0;
    workingFrame = 0;
    return;
  }

  if (finishedLevel != HIGH || bitCount >= 17) return;

  uint8_t value;
  if (duration >= 50UL && duration <= 130UL) value = 0;
  else if (duration >= 160UL && duration <= 270UL) value = 1;
  else return;

  workingFrame = (workingFrame << 1) | value;
  bitCount++;
}

bool decodeFrame(uint32_t frame) {
  // 17-bit frame: active prefix must be 1 0000
  uint8_t prefix = (frame >> 12) & 0x1F;
  if (prefix != 0x10) return false;

  uint16_t raw = frame & 0x0FFF;

  // Known active channel space: 0x000..0x7FF
  if (raw >= 0x800) return false;

  uint8_t channel = raw >> 9;       // 0=A, 1=B, 2=C, 3=D
  uint16_t distanceRaw = raw & 0x01FF;

  distanceCm[channel] = distanceRaw / 2.0f;
  lastSensorMs[channel] = millis();
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(SIGNAL_PIN, INPUT);

  lastState = digitalRead(SIGNAL_PIN);
  lastEdgeUs = micros();

  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), signalISR, CHANGE);

  Serial.println(F("Universal Parking Sensor Decoder"));
  Serial.println(F("A/B/C/D sensor data - 115200 baud"));
}

void loop() {
  uint32_t frame = 0;
  bool available = false;

  noInterrupts();
  if (frameReady) {
    frame = readyFrame;
    frameReady = false;
    available = true;
  }
  interrupts();

  if (available) decodeFrame(frame);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 250) {
    lastPrint = millis();

    const unsigned long staleMs = 1000;
    const char names[4] = {'A','B','C','D'};

    for (uint8_t i=0; i<4; i++) {
      Serial.print(names[i]);
      Serial.print('=');

      if (lastSensorMs[i] != 0 && millis() - lastSensorMs[i] <= staleMs) {
        Serial.print(distanceCm[i], 1);
        Serial.print(F(" cm"));
      } else {
        Serial.print(F("---"));
      }

      if (i < 3) Serial.print(F(" | "));
    }
    Serial.println();
  }
}
