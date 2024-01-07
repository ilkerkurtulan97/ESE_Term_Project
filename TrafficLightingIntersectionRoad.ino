const int northRed = 2, northYellow = 3, northGreen = 4;
const int eastRed = 5, eastYellow = 6, eastGreen = 7;
const int southRed = 8, southYellow = 9, southGreen = 10;
const int westRed = 11, westYellow = 12, westGreen = 13;

const int nightButtonPin = A0;
const int emergencyButtonPin = A1;

unsigned long previousMillis = 0;
const long standardGreenDuration = 3000;// Standard duration for green light in IDLE mode
const long nightGreenDuration = 5000;// Faster green light duration in Nighttime mode
const long emergencyDuration = 3000;// Duration of Emergency mode

enum Mode { IDLE, NIGHT, EMERGENCY };
Mode currentMode = IDLE;

void setup() {
  pinMode(northRed, OUTPUT);
  pinMode(northYellow, OUTPUT);
  pinMode(northGreen, OUTPUT);
  pinMode(eastRed, OUTPUT);
  pinMode(eastYellow, OUTPUT);
  pinMode(eastGreen, OUTPUT);
  pinMode(southRed, OUTPUT);
  pinMode(southYellow, OUTPUT);
  pinMode(southGreen, OUTPUT);
  pinMode(westRed, OUTPUT);
  pinMode(westYellow, OUTPUT);
  pinMode(westGreen, OUTPUT);

  pinMode(nightButtonPin, INPUT);
  pinMode(emergencyButtonPin, INPUT);

}

void loop() {
  if (currentMode != EMERGENCY) {
    currentMode = IDLE;
  }

  //Check for emergency button press
  if (digitalRead(emergencyButtonPin) == HIGH) {
    currentMode = EMERGENCY;
    previousMillis = millis();
  }

  //Current mode
  switch (currentMode) {
    case IDLE:
      handleIdleMode();
      break;
    case NIGHT:
      handleNightMode();
      break;
    case EMERGENCY:
      if (millis() - previousMillis <= emergencyDuration) {
        handleEmergencyMode();
      } else {
        //After emergency duration, return to IDLE mode
        currentMode = IDLE;
        
      }
      break;
  }

  //Check for night button press
  if (currentMode != EMERGENCY && digitalRead(nightButtonPin) == HIGH) {
    currentMode = NIGHT;
  }
}


void handleIdleMode() {
  for(int i=2; i<14; i++){
  	digitalWrite(i, HIGH);
    delay(1500);
    digitalWrite(i, LOW);
  }
}

void handleNightMode() {
  
}

void handleEmergencyMode() {
  digitalWrite(northGreen, HIGH);
  digitalWrite(southGreen, HIGH);
  digitalWrite(eastRed, HIGH);
  digitalWrite(westRed, HIGH);
  //Other lights are off
  digitalWrite(northRed, LOW);
  digitalWrite(northYellow, LOW);
  digitalWrite(southRed, LOW);
  digitalWrite(southYellow, LOW);
  digitalWrite(eastYellow, LOW);
  digitalWrite(eastGreen, LOW);
  digitalWrite(westYellow, LOW);
  digitalWrite(westGreen, LOW);
}
