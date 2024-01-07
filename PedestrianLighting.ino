const int button1Pin = 2;     // Button to activate fastGreenMode
const int button2Pin = 6;     // Button to reset to idleMode
const int redLightPin = 3;    // Red traffic LED
const int yellowLightPin = 4; // Yellow traffic LED
const int greenLightPin = 5;  // Green traffic LED

bool fastModeActive = false;

void setup() {
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(redLightPin, OUTPUT);
  pinMode(yellowLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
}

void loop() {
  if (digitalRead(button1Pin) == HIGH) {

    fastModeActive = true;
    while (fastModeActive) {
      fastGreenMode();
      
      if (digitalRead(button2Pin) == HIGH) {
        fastModeActive = false;
      }
    }
  }
  
  idleMode();
}

void idleMode() {
  digitalWrite(redLightPin, HIGH);
  delay(5000);
  digitalWrite(redLightPin, LOW);

  digitalWrite(yellowLightPin, HIGH);
  delay(3000);
  digitalWrite(yellowLightPin, LOW);

  digitalWrite(greenLightPin, HIGH);
  delay(5000);
  digitalWrite(greenLightPin, LOW);
}

void fastGreenMode() {
  digitalWrite(redLightPin, HIGH);
  delay(300);
  digitalWrite(redLightPin, LOW);

  digitalWrite(yellowLightPin, HIGH);
  delay(300);
  digitalWrite(yellowLightPin, LOW);

  digitalWrite(greenLightPin, HIGH);
  delay(300);
  digitalWrite(greenLightPin, LOW);
}
