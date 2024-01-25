#include <Wire.h>

const int redLED = 13;
const int yellowLED = 12;
const int greenLED = 11;


// Define the I2C address for the Arduino
const int i2c_address = 0x20;

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  // Start the I2C Bus as Slave on address 0x04
  Wire.begin(i2c_address);
  
  // Define a receive event handler function
  Wire.onReceive(receiveEvent);

  // Define a request event handler function
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // Start serial communication for debugging
  Serial.println("I2C Slave Device Ready");
}

void turnRED()
{
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, HIGH);
  delay(1500);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, HIGH);
  
}

void turnGREEN()
{
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
}


void loop() {
	delay(1000);
}

void receiveEvent(int howMany) {
  	String receivedData = ""; // Store received characters as a string

    while (Wire.available()) {
        char c = Wire.read(); // Receive byte as a character
        receivedData += c; // Append the character to the string
    }

    // Now, parse the receivedData string to extract an integer
    int x;
    if (receivedData.length() > 0) {
        x = receivedData.toInt(); // Convert the string to an integer
    } else {
        // Handle an error condition if no data received or conversion fails
        x = -1; // For example, set x to -1 to indicate an error
    }

    Serial.println(x);

    // Check for the specific condition (e.g., x equals 0x112)
    if (x == 1) {
        turnGREEN();
    }
  	if (x == 2) {
        turnRED();
    }
}


// Function to be called when data is requested by master
void requestEvent() {
  // Respond with a message
  Wire.write("Hello Master");
}
