#include <Arduino.h>

const int fanPin = 22;  // Replace with the GPIO pin connected to the fan's PWM input
const int motionPin = 34;  // Replace with the GPIO pin connected to the motion detector
const int lightPin = 24;  // Replace with the GPIO pin connected to the 5V LED

bool fanIsRunning = false;
bool lightIsOn = false;
unsigned long fanStartTime = 0;
unsigned long lightOnTime = 0;

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 bps
  Serial.println("Initializing setup...");

  pinMode(motionPin, INPUT);
  
  ledcSetup(0, 25000, 8);  // Set up PWM channel 0
  ledcAttachPin(fanPin, 0);  // Attach fan pin to PWM channel 0

  pinMode(lightPin, OUTPUT);
  Serial.println("Setup complete.");
}

void loop() { 
  int motionState = digitalRead(motionPin);
  // Check if motion is detected
  if (motionState == HIGH){
    // Turn on light if it's not already on
    if (!lightIsOn) {
      Serial.println("Motion detected, turning on light");
      digitalWrite(lightPin, HIGH);
      lightIsOn = true;
      lightOnTime = millis();
    }
    // Turn on fan if it's not already running
    if (!fanIsRunning) {
      Serial.println("Motion detected, turning on fan");
      ledcWrite(0, 128);  // Set fan speed to 50%
      fanIsRunning = true;
      fanStartTime = millis();
    }
  }

  if (motionState == LOW && lightIsOn) {
    if ((millis() - lightOnTime) >= 300000) {
      // Adds a delay of 5 minutes before turning off the light incases where object is not moving
      digitalWrite(lightPin, LOW);
      lightIsOn = false;
    }
  }
  if (fanIsRunning) {
    // Gradually increase fan speed over 5 minutes
    if ((millis() - fanStartTime) < 300000){
      // Fan is running and motion was detected less than 5 minutes ago, increase fan speed
      int fanSpeed = map(millis() - fanStartTime, 0, 300000, 128, 255);
      ledcWrite(0, fanSpeed);
    }
    if ((millis() - fanStartTime) >= 300000) {
      ledcWrite(0, 255);
    }
    if ((millis() - fanStartTime) >= 3600000) {
      // Fan has been running for 1 hours, turn it off
      ledcWrite(0, 0);
      fanIsRunning = false;
    }
  }

  delay(100);
}
