#include <Wire.h>

// Define the I2C address for the Arduino
const int i2c_address = 0x04;

const int buttonPin = 12; // Button connected to digital pin 2
int buttonState = 0; 

void setup() {
  pinMode(buttonPin, INPUT);
  
  // Start the I2C Bus as Slave on address 0x04
  Wire.begin(i2c_address);
  
  // Define a receive event handler function
  Wire.onReceive(receiveEvent);

  // Define a request event handler function
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // Start serial communication for debugging
  Serial.println("I2C Slave Device Ready");
}

const int maxArguments = 10; // Maximum number of arguments

String serializeData(int messageType, const int* arguments, int numberOfArguments) {
    String serializedData = String(messageType); // Start with the message type

    for (int i = 0; i < numberOfArguments; ++i) {
        serializedData += ","; // Add a comma separator
        serializedData += String(arguments[i]); // Append the argument
    }

    return serializedData;
}

void loop() {
  
  buttonState = digitalRead(buttonPin); // Read the state of the button

  if (buttonState == HIGH) {   // If the button is pressed
  	Serial.println("uooo");
    int arguments[maxArguments] = {1}; // Example arguments
    String data = serializeData(274, arguments, 1); // Serialize data

    Wire.beginTransmission(0x8); // Begin transmission to the device with address 8
    Wire.write(data.c_str()); // Send a string
    Wire.endTransmission(); // End transmission

    Serial.println("Message sent");
    delay(6000); // Wait for a second
    arguments[maxArguments] = {1}; // Example arguments
    data = serializeData(275, arguments, 1); // Serialize data

    Wire.beginTransmission(0x8); // Begin transmission to the device with address 8
    Wire.write(data.c_str()); // Send a string
    Wire.endTransmission(); // End transmission
    
  }

}

// Function to be called when data is received from master
void receiveEvent(int howMany) {
  while (Wire.available()) { // Loop through all received bytes
    char c = Wire.read(); // Receive byte as a character
    Serial.print(c); // Print the character to the serial monitor
  }
  Serial.println(); // Newline for serial output
}

// Function to be called when data is requested by master
void requestEvent() {
  // Respond with a message
  Wire.write("Hello Master");
}
