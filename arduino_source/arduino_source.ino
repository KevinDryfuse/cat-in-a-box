#include <Arduino.h>
#include <Wire.h>                 // Must include Wire library for I2C
#include "SparkFun_MMA8452Q.h"    // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q
#include "DFRobotDFPlayerMini.h"

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

// Motor Driver Constants
#define DIRA 3
#define DIRB 4
#define ENABLE 5

DFRobotDFPlayerMini myDFPlayer;

#define VOLUME 30;

enum Intensity {
  SAFE,
  MINOR,
  MILD,
  VIOLENT
};

enum Direction {
  FLAT,
  UP,
  DOWN,
  LEFT,
  RIGHT
};

MMA8452Q accel;                   // create instance of the MMA8452 class

// Initialize all shake settings to a base "stable" state
boolean initialized = true;
Intensity intensity = SAFE;
float magnitude = 0;
float lastValue = magnitude; // Track the last value
bool possiblePeak = false; // Flag to indicate a possible peak
int declineCount = 0; // Count the number of declines after a possible peak

void setup() {
  
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  #if (defined ESP32)
    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D2, /*tx =*/D1);
  #else
    FPSerial.begin(9600);
  #endif

  Serial.begin(19200);
  Wire.begin(); // TODO: Is this needed?
  
  if (accel.begin() == false) {
    Serial.println("Accelerometer not connected. Please check connections and read the hookup guide.");
    while (1);
  }
  else {
    Serial.println("Accelerometer online.");
  }

  initialize_dfrobot_mp3_player();
  
}

void initialize_dfrobot_mp3_player() {

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(2000);
  myDFPlayer.volume(VOLUME);  //Set volume value. From 0 to 30
  playRandomFromFolder(5); // Play a startup sound to know it's working
  delay(5000); // Give it a few seconds to put the box down and stuff before it starts freaking out!

}


void move(Intensity intensity) {
  
  int revs = 0;

  if (intensity == VIOLENT) {
    revs = 5;
  } else if (intensity == MILD) {
    revs = 2;
  } else if (intensity == MINOR) {
    revs = 1;
  }

  int i;
  digitalWrite(ENABLE,HIGH); // enable on
  
  for (i=0;i<revs;i++) {
    digitalWrite(DIRA,HIGH); //one way
    digitalWrite(DIRB,LOW);
    delay(500);
    digitalWrite(DIRA,LOW);  //reverse
    digitalWrite(DIRB,HIGH);
    delay(500);
  }
  digitalWrite(ENABLE,LOW); // disable
}

Direction get_direction() {

    Direction direction = FLAT;

    if (accel.isRight() == true) {
      direction = RIGHT;
    }
    else if (accel.isLeft() == true) {
      direction = LEFT;
    }
    else if (accel.isUp() == true && init == true) {
      direction = UP;
      initialized = false;
    }
    else if (accel.isDown() == true) {
      direction = DOWN;
    }
    else if (accel.isFlat() == true) {
      direction = FLAT;
    }

  return direction;

}

void playRandomFromFolder(int folderNumber) {

  // I know this seems stupid, but the only way to get an accurate count of files in a folder is to call it multiple times.
  // Otherwise, it returns -1 or significantly more than actually exist.  
  int fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
  fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
  fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
  fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
  fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);

  int randomFile = random(1, fileCount + 1); // random file in folder

  myDFPlayer.playFolder(folderNumber, randomFile); // Play random file
}

void loop() {

  int randomMeow = random(1000000);

  // Generate a different random number between 0 and 999 for "scratch"
  int randomScratch = random(1000000);

  // Check if the random number for "meow" is 0 (1 in 1000 chance)
  if (randomMeow == 0) {
    Serial.println("MEOW 0");
      playRandomFromFolder(1);
  }
    // Check if the random number for "scratch" is 0 (1 in 1000 chance)
  if (randomScratch == 0) {
    Serial.println("SCRATCH 0");
    move(MINOR);
  }  
  else if (randomScratch == 1) {
    Serial.println("SCRATCH 1");
    move(MILD);
  }
  else if (randomScratch == 2) {
    Serial.println("SCRATCH 2");
    move(VIOLENT);
  }

  if (accel.available()) {      // Wait for new data from accelerometer
    // Acceleration of x, y, and z directions in g units

    float x = accel.getCalculatedX();
    float y = accel.getCalculatedY();
    float z = accel.getCalculatedZ();

    float magnitude = sqrt(x*x + y*y + z*z);

    // Define thresholds for shake intensity
    const float minorShakeThreshold = 1.3; // example threshold, adjust as needed
    const float mildShakeThreshold = 1.7;  // example threshold, adjust as needed
    const float violentShakeThreshold = 2.2; // example threshold, adjust as needed

    Intensity intensity = SAFE;

    // Check for a possible peak
    if (magnitude > lastValue) {
      possiblePeak = true;
      declineCount = 0; // Reset decline count
    } else if (possiblePeak && magnitude < lastValue) {
      declineCount++;
      if (declineCount == 2) {
        // A peak has been confirmed
        // Serial.print("Peak detected at: ");
        // Serial.println(lastValue);
        possiblePeak = false; // Reset possible peak
        declineCount = 0; // Reset decline count
      }
    }

    lastValue = magnitude; // Update the last value

    // Check the intensity
    if (lastValue > violentShakeThreshold) {
      intensity = VIOLENT;
    } else if (lastValue > mildShakeThreshold) {
      intensity = MILD;
    } else if (lastValue > minorShakeThreshold) {
      intensity = MINOR;
    } else {
      intensity = SAFE;
    }
    
    Direction direction = get_direction();

    if (direction == FLAT && intensity == SAFE) {}
    else if (direction != FLAT && intensity == SAFE) {
      Serial.print(lastValue);
      Serial.println(" - being an ass");
      playRandomFromFolder(1);
      move(MINOR);
      delay(2000);
    }
    else if (intensity == MINOR) {
      Serial.print(lastValue);
      Serial.println(" - minor shake");
      playRandomFromFolder(2);
      move(MINOR);
      delay(2000);
    }
    else if (intensity == MILD) {
      Serial.print(lastValue);
      Serial.println(" - mild shake");
      playRandomFromFolder(3);
      move(MILD);
      delay(2000);
    }
    else if (intensity == VIOLENT) {
      Serial.print(lastValue);
      Serial.println(" - violent shake");
      playRandomFromFolder(4);
      move(VIOLENT);
      delay(2000);
    }

  }
}