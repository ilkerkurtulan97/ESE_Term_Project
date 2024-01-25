#line 2 "basic.ino"

// Adapted from:
// https://github.com/mmurdoch/arduinounit/blob/master/examples/basic/basic.ino

#include <AUnit.h>

#include <Wire.h>

const int car_i2c_offset = 0x40;

bool emergencyFlag = false;

enum TrafficLight {
    Green,
    Yellow,
    Red
};

unsigned long task1LastSwitchTime = 0;
unsigned long task2LastSwitchTime = 0;

int numberOfRoutes = 2;
int trafficData[2] = {0,0};
int emergencyData[2] = {0,0};
int trafficLightState[2] = {Red,Green};
int trafficLightOffset = 0x20;

const int maxArguments = 10;

int numberOfVehicles = 2;

enum State {
    Idle,
    StandardOperations,
    Emergency
};

State currentState = Idle;

int deserializeData(const String& receivedData, int* arguments, int& numberOfArguments) {
    int startIndex = 0;
    int endIndex = receivedData.indexOf(',');
    int messageType;
    
    
    if (endIndex != -1) { // Check if there is at least one comma
        messageType = receivedData.substring(startIndex, endIndex).toInt(); // Extract message type

        startIndex = endIndex + 1;
        while ((endIndex = receivedData.indexOf(',', startIndex)) != -1 && numberOfArguments < maxArguments) {
            arguments[numberOfArguments++] = receivedData.substring(startIndex, endIndex).toInt(); // Extract argument
            startIndex = endIndex + 1;
        }

        // Add the last argument after the last comma
        if (startIndex < receivedData.length() && numberOfArguments < maxArguments) {
            arguments[numberOfArguments++] = receivedData.substring(startIndex).toInt();
        }
    } else {
        // If there are no commas, treat the entire string as a single integer (messageType)
        messageType = receivedData.toInt();
    }

    return messageType;
}
void task1(int* args, int numArgs) {
    // Check if 1 second has passed
  if(currentState != Emergency)
  {
    Wire.beginTransmission(0x20); // Begin transmission to the device with address 0x20
    if (trafficLightState[0] == Green) {
      Wire.write("2"); // Send "2" to turn it red
      trafficLightState[0] = Red; // Change state to red
    } else {
      Wire.write("1"); // Send "1" to turn it green
      trafficLightState[0] = Green; // Change state to green
    }
    Wire.endTransmission(); // End transmission
  }
  

}

void task2(int* args, int numArgs) {
    // Check if 1 second has passed
  if(currentState != Emergency)
  {
    Wire.beginTransmission(0x21); // Begin transmission to the device with address 0x21
    if (trafficLightState[1] == Green) {
      Wire.write("2"); // Send "2" to turn it red
      trafficLightState[1] = Red; // Change state to red
    } else {
      Wire.write("1"); // Send "1" to turn it green
      trafficLightState[1] = Green; // Change state to green
    }
    Wire.endTransmission(); // End transmission
  }

}

// Define a structure for tasks
struct Task {
    unsigned long interval; // interval in milliseconds
    signed long lastRun;  // last run time
    void (*func)(int*, int); // pointer to the function
    int args[maxArguments];  // Array to store arguments
    int numArgs;             // Number of arguments
};

// Initialize tasks
Task tasks[10]; 

// Number of tasks
byte numTasks = 2;



int routeCalculation(int desiredRoute)
{
    delay(100);
    int suggestedRoute = 0;
  
    //check if desiredRoute is optimal
    if(trafficData[desiredRoute-1] == 0 && emergencyData[desiredRoute-1] == 0)
    {
      return desiredRoute ;
    }
    
    //find a new route

    for(int i = 0 ; i < numberOfRoutes; i++)
    {
      if(trafficData[i] == 0 && emergencyData[i] == 0)
      {
        suggestedRoute = i + 1 ;
        break;

      }
    }
  
    return suggestedRoute;
}

void routingFunction(int* args, int numArgs) {
  
  if(currentState != Emergency)
  {
    int a = routeCalculation(args[1]);

      
    Wire.beginTransmission(car_i2c_offset + args[0]);
    Wire.write(String(a).c_str()); // Send "2" to turn it red
    Wire.endTransmission(); // End transmission
  }
}

void addRoutingTask(int* args, int numArgs) {
  
    // Add the emergency function to the task list
      if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks].interval = 0; // Run the emergency function immediately
        tasks[numTasks].lastRun = 0;
        tasks[numTasks].func = routingFunction;
        tasks[numTasks].numArgs = numArgs;
        for (int i = 0; i < numArgs; ++i) {
            tasks[numTasks].args[i] = args[i];
        }
        
        numTasks++; // Increment the number of tasks
    }
    
}

void emergencyFunction(int* args, int numArgs) {

    emergencyData[args[0]-1] = 1;

    for(int i = 0; i < numberOfVehicles; i++)
    {
        int a = routeCalculation(args[0]);
        Wire.beginTransmission(car_i2c_offset + i + 1); // Begin transmission to the device with address 0x21
        Wire.write(String(a).c_str()); // Send "2" to turn it red
        Wire.endTransmission(); // End transmission
    }


    Wire.beginTransmission(trafficLightOffset + args[0] -1); // Begin transmission to the device with address 0x21
    Wire.write("1");
    trafficLightState[args[0]-1] = Green;
    Wire.endTransmission(); // End transmission

    for(int i = 0 ; i < numberOfRoutes ; i++)
    {
      if(args[0] -1 == i ) continue;
      Wire.beginTransmission(trafficLightOffset + i ); // Begin transmission to the device with address 0x21
      Wire.write("2");
      trafficLightState[i] = Red;
      Wire.endTransmission(); // End transmission
    }
    

    

}


void addEmergencyTask(int* args, int numArgs) {
    // Add the emergency function to the task list
      if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks].interval = 0; // Run the emergency function immediately
        tasks[numTasks].lastRun = 0;
        tasks[numTasks].func = emergencyFunction;
        tasks[numTasks].numArgs = numArgs;
        for (int i = 0; i < numArgs; ++i) {
            tasks[numTasks].args[i] = args[i];
        }
        numTasks++; // Increment the number of tasks
    }
    
}

void receiveEvent(int howMany) {
    String receivedData = "";  // Store received characters as a string

    while (Wire.available()) {
        char c = Wire.read();  // Receive byte as a character
        receivedData += c;     // Append the character to the string
    }

    int arguments[maxArguments];
    int numberOfArguments = 0;

    int messageType = deserializeData(receivedData, arguments, numberOfArguments);

    // Check for the specific condition (e.g., x equals 0x112)
    if (messageType == 0x112) {
        // Enter emergency state
        addEmergencyTask(arguments,numberOfArguments);
        emergencyFlag = true; // Set the emergency flag
    }else if(messageType == 100)
    {

      addRoutingTask(arguments,numberOfArguments);
    }else if(messageType == 0x113)
    {
        emergencyData[arguments[0]-1] = 0;
        emergencyFlag = false;
    }
   
}


void mainLoop()
{
  unsigned long currentMillis = millis();
  
  switch (currentState) {
  case Idle:
      task1(NULL,0);
      task2(NULL,0);
    
      currentState = StandardOperations;
      break;
  case StandardOperations:
    
      if(emergencyFlag == true)
        {
          currentState = Emergency;
          Serial.println("emergency");
        }
    
      break;
    case Emergency:

    for(byte i = 0; i < numTasks; i++) {
      if ((currentMillis - tasks[i].lastRun) >= tasks[i].interval) {
        tasks[i].func(tasks[i].args, tasks[i].numArgs); // Pass arguments to the function
        tasks[i].lastRun = currentMillis;

        if (tasks[i].interval == 0) {
          for (byte j = i; j < numTasks - 1; j++) {
            tasks[j] = tasks[j + 1];  // Shift tasks to the left
          }
          numTasks--;
          continue;  // Continue from the next task
        }
      }
    }
      
      if(emergencyFlag == false)
        {
          currentState = StandardOperations;
          Serial.println("transitionback");
        }
      break;
  }
  
  delay(1);
}

test(mainLoop_StateTransition) {
    currentState = StandardOperations;

    emergencyFlag = true;
    mainLoop();

    // Check if emergencyData[1] is set to 1
    assertEqual(currentState, Emergency);
}


test(emergencyFunction_SetsEmergencyData) {
    int arguments[1] = {1}; // Argument to pass
    int a = 1;
    int numArgs = a; // Number of arguments

    // Reset emergencyData to its initial state
    emergencyData[0] = 0;
    emergencyData[1] = 0;

    // Call the emergency function
    emergencyFunction(arguments, numArgs);

    // Check if emergencyData[1] is set to 1
    assertEqual(emergencyData[a-1], 1);
}

test(deserializeData_CorrectParsing) {
    String testData = "100,1,2";
    int arguments[maxArguments];
    int numberOfArguments = 0;
    int expectedMessageType = 100;
    int expectedArguments[2] = {1, 2};
    
    int messageType = deserializeData(testData, arguments, numberOfArguments);

    assertEqual(messageType, expectedMessageType);
    assertEqual(numberOfArguments, 2);
    for (int i = 0; i < numberOfArguments; i++) {
        assertEqual(arguments[i], expectedArguments[i]);
    }
}

test(task2_TogglesTrafficLight) {
    currentState = StandardOperations;
    trafficLightState[1] = Red; // Initially red

    task2(NULL, 0); // Call task2

    assertEqual(trafficLightState[1], Green); // Expect to be green after task2
}

test(addEmergencyTask_AddsTaskCorrectly) {
    int arguments[1] = {1};
    int numArgs = 1;
    numTasks = 0; // Reset numTasks

    addEmergencyTask(arguments, numArgs);
    tasks[0].func(tasks[0].args, tasks[0].numArgs); // Execute the function

    // Check the expected side effects of emergencyFunction
    assertEqual(emergencyData[0], 1); // Assuming emergencyFunction sets emergencyData[0] to 1

}


//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // wait for stability on some boards to prevent garbage Serial
#endif
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while (!Serial); // for the Arduino Leonardo/Micro only
#if defined(EPOXY_DUINO)
  Serial.setLineModeUnix();
#endif

}

void loop() {
  aunit::TestRunner::run();
}
