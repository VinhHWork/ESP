#include <Arduino.h>

const int fanPin = 2;  // Replace with the GPIO pin connected to the fan's PWM input
const int motionPin = 4;  // Replace with the GPIO pin connected to the motion detector
const int lightPin = 5;  // Replace with the GPIO pin connected to the 5V LED

bool fanIsRunning = false;
bool lightIsOn = false;
unsigned long fanStartTime = 0;
unsigned long motionDetectedTime = 0;
unsigned long lightOnTime = 0;
int prevMotionState = LOW;

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 bps
  Serial.println("Initializing setup...");

  ledcSetup(0, 25000, 8);  // Set up PWM channel 0
  Serial.println("PWM channel 0 set up at 25kHz with 8-bit resolution");

  ledcAttachPin(fanPin, 0);  // Attach fan pin to PWM channel 0
  Serial.println("Fan pin attached to PWM channel 0");

  pinMode(motionPin, INPUT);
  Serial.println("Motion pin set as INPUT");

  pinMode(lightPin, OUTPUT);
  Serial.println("Light pin set as OUTPUT");

  Serial.println("Setup complete.");
}

void loop() { 
  int motionState = digitalRead(motionPin);

  if (motionState == HIGH){
    if (!lightIsOn) {
      // Motion detected and light is not on, turn on light
      Serial.println("Motion detected, turning on light");
      digitalWrite(lightPin, HIGH);
      lightIsOn = true;
      lightOnTime = millis();
    }

    if (!fanIsRunning) {
      // Motion detected and fan is not running, turn on fan and light
      Serial.println("Motion detected, turning on fan");
      ledcWrite(0, 128);  // Set fan speed to 50%
      fanIsRunning = true;
      fanStartTime = millis();
    }
    // Reset motion detected time
    if (motionState != prevMotionState) {
      motionDetectedTime = millis();
    }
    // Store previous motion state
    prevMotionState = motionState;
  }

  if (motionState == LOW && lightIsOn) {
    if ((millis() - lightOnTime) >= 300000) {
      // Adds a delay of 5 minutes before turning off the light incases where object is not moving
      Serial.println("Light has been on for 5 minutes, turning it off");
      digitalWrite(lightPin, LOW);
      lightIsOn = false;
    }
  }
  if (fanIsRunning) {

    if ((millis() - motionDetectedTime) < 300000){
      // Fan is running and motion was detected less than 5 minutes ago, increase fan speed
      int fanSpeed = map(millis() - motionDetectedTime, 0, 300000, 128, 255);
      Serial.print(millis() - motionDetectedTime);
      Serial.println(fanSpeed);
      ledcWrite(0, fanSpeed);
    }
    if ((millis() - fanStartTime) >= 300000) {
      // Fan has been running for 5 minutes, set fan speed to 100%
      Serial.println("Fan has been running for 5 minutes, setting fan speed to 100%");
      ledcWrite(0, 255);
    }
    if ((millis() - fanStartTime) >= 7200000) {
      // Fan has been running for 2 hours, turn it off
      Serial.println("Fan has been running for 2 hours, turning it off");
      ledcWrite(0, 0);
      fanIsRunning = false;
    }
  }

  delay(100);
}
