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
int trafficLightState[2] = {Green,Red};
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
    unsigned long deadline;
    void (*func)(int*, int); // pointer to the function
    int args[maxArguments];  // Array to store arguments
    int numArgs;             // Number of arguments
};

// Initialize tasks
Task tasks[10]; 


void sortTasksByDeadline(int numTasks) {
  for (int i = 0; i < numTasks - 1; i++) {
        for (int j = 0; j < numTasks - i - 1; j++) {
            if (tasks[j].deadline > tasks[j + 1].deadline) {
                // Swap tasks[j] and tasks[j + 1]
                Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
  }
}



void setup() {
  Wire.begin(8); // Initialize as an I2C slave with address 8
  Wire.onReceive(receiveEvent); // Register the receive event function

  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("\nI2C Scanner");
  
  tasks[0] ={3000, 0, 3000 , task1,{} ,0};  // Run task1 every 1000ms (1 second)
  tasks[1] = {3000, 0, 3000 , task2,{},0} ;  // Run task2 every 2000ms (2 seconds)
  

  
}

// Number of tasks
byte numTasks = 2;

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
    unsigned long currentMillis = millis();
    // Add the emergency function to the task list
      if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks].interval = 0; // Run the emergency function immediately
        tasks[numTasks].lastRun = 0;
        tasks[numTasks].func = emergencyFunction;
        tasks[numTasks].numArgs = numArgs;
        tasks[numTasks].deadline = currentMillis + 500;
        
        for (int i = 0; i < numArgs; ++i) {
            tasks[numTasks].args[i] = args[i];
        }
        numTasks++; // Increment the number of tasks
    }
    
}

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
      unsigned long currentMillis = millis();
  
    // Add the emergency function to the task list
      if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks].interval = 0; // Run the emergency function immediately
        tasks[numTasks].lastRun = 0;
        tasks[numTasks].func = routingFunction;
        tasks[numTasks].numArgs = numArgs;
        tasks[numTasks].deadline = currentMillis + 1000;
        
        for (int i = 0; i < numArgs; ++i) {
            tasks[numTasks].args[i] = args[i];
        }
        
        numTasks++; // Increment the number of tasks
    }
    
}

unsigned long lastTask1Addition = 0;

// Function to add task1 and task2
void addPeriodicTasks() {
    // Add task1
    if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks] = {1000, millis(), millis() + 1000, task1, {}, 0}; // Adjust parameters as needed
        numTasks++;
    }
    
    // Add task2
    if (numTasks < sizeof(tasks) / sizeof(Task)) {
        tasks[numTasks] = {1000, millis(), millis() + 1000, task2, {}, 0}; // Adjust parameters as needed
        numTasks++;
    }
}

void loop() {
  
  unsigned long currentMillis = millis();
  
    if (currentMillis - lastTask1Addition >= 1000) {
      addPeriodicTasks(); // This adds both task1 and task2
      lastTask1Addition = currentMillis;
  }
  
  sortTasksByDeadline(numTasks);
  
  switch (currentState) {
    case Idle:
        task1(NULL,0);
        task2(NULL,0);

        currentState = StandardOperations;
        break;
    case StandardOperations:
        if(numTasks > 0)
        {
          // Execute the first task
          tasks[0].func(tasks[0].args, tasks[0].numArgs);

          // Shift the remaining tasks forward
          for (int i = 1; i < numTasks; i++) {
            tasks[i - 1] = tasks[i];
          }

          // Decrement the number of tasks
          numTasks--;
        }
        

        if(emergencyFlag == true)
          {
            currentState = Emergency;
            Serial.println("emergency");
          }

        break;
      case Emergency:

      if(numTasks > 0)
      {
        // Execute the first task
        tasks[0].func(tasks[0].args, tasks[0].numArgs);

        // Shift the remaining tasks forward
        for (int i = 1; i < numTasks; i++) {
          tasks[i - 1] = tasks[i];
        }

        // Decrement the number of tasks
        numTasks--;
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
