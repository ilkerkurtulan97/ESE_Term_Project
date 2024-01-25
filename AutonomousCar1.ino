unsigned const int A = 13;
unsigned const int B = 12;
unsigned const int C = 11;
unsigned const int D = 10;
unsigned const int E = 9;
unsigned const int F = 8;
unsigned const int G = 7;
unsigned const int H = 6;

#include <Wire.h>

// Define the I2C address for the Arduino
const int car_i2c_offset = 0x40;
const int car_i2c_id = 0x2;

const int car_i2c_channel =car_i2c_offset + car_i2c_id;


void setup(void)
{
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(H, OUTPUT);
  
  delay(car_i2c_id);
  // Start the I2C Bus as Slave on address 0x04
  Wire.begin(car_i2c_channel);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  // Define a receive event handler function
  Wire.onReceive(receiveEvent);

  // Define a request event handler function
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // Start serial communication for debugging
  Serial.println("I2C Slave Device Ready");
}

//My Functions

void zero(void) {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void one(void) {
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void two(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, LOW);
  digitalWrite(H, LOW);
}

void three(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void four(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void five(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void six(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void seven(void) {
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void eight(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
}

void nine(void) {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(H, LOW);
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
  int arguments[maxArguments] = {car_i2c_id, 1}; // Example arguments
  String data = serializeData(100, arguments, 2); // Serialize data
  
  Wire.beginTransmission(0x8); // Begin transmission to the device with address 8
  Wire.write(data.c_str()); // Send a string
  Wire.endTransmission(); // End transmission

  delay(2000); // Wait for a second
}

// Function to be called when data is received from master
void receiveEvent(int howMany) {
    String receivedData = "";  // Store received characters as a string

    while (Wire.available()) {
        char c = Wire.read();  // Receive byte as a character
        receivedData += c;     // Append the character to the string
    }
	
  if(receivedData == "1")
  {
    one();
  }
  if(receivedData == "2")
  {
    two();
  }
  if(receivedData == "0")
  {
    zero();
  }
}

// Function to be called when data is requested by master
void requestEvent() {
  // Respond with a message
  Wire.write("Hello Master");
}


