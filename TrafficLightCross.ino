const int buttonPin = 1;
const int redPin = A1;
const int yellowPin = A2;
const int greenPin = A3;

int mode = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    mode = 1;
    delay(100);
  }

  if (mode == 0) {
    // Idle mode
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
    delay(3000);  // Red LED for 3 seconds

    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, LOW);
    delay(1000);  // Yellow LED for 1 second

    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
    delay(3000);  // Green LED for 3 seconds
  } else {
    // Button-triggered mode
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
    
    delay(5000);

    // Switching back to idle mode after button-triggered green
    mode = 0;
  }
}
