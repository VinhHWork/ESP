#include <Arduino.h>

const int fanPin = 2;  // Replace with the GPIO pin connected to the fan's PWM input
const int motionPin = 4;  // Replace with the GPIO pin connected to the motion detector
const int lightPin = 5;  // Replace with the GPIO pin connected to the 5V LED

bool fanIsRunning = false;
bool lightIsOn = false;
unsigned long fanStartTime = 0;
unsigned long motionDetectedTime = 0;
unsigned long lightOnTime = 0;
unsigned long lastMotionTime = 0;

/**
 * Initializes the pin modes for the fan, motion detector, and LED light.
 * Sets the fan and light pins as OUTPUT and the motion detector pin as INPUT.
 */
void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(lightPin, OUTPUT);
}

void loop() {
  int motionState = digitalRead(motionPin);

  if (motionState == HIGH && !fanIsRunning) {
    // Motion detected and fan is not running, turn on fan and light
    digitalWrite(fanPin, HIGH);  // Start fan at low speed
    analogWrite(fanPin, 128);  // Set fan speed to 50%
    digitalWrite(lightPin, HIGH);  // Turn on 5V LED
    fanIsRunning = true;
    lightIsOn = true;
    fanStartTime = millis();
    motionDetectedTime = millis();
    lightOnTime = millis();
    lastMotionTime = millis();
  }

  if (motionState == LOW && lightIsOn) {
    // No motion detected and light is on, turn off light
    digitalWrite(lightPin, LOW);
    lightIsOn = false;
  }

  if (fanIsRunning && (millis() - motionDetectedTime) < 300000) {
    // Fan is running and motion was detected less than 5 minutes ago, increase fan speed
    analogWrite(fanPin, map(millis() - motionDetectedTime, 0, 300000, 128, 255));
  }

  if (fanIsRunning && (millis() - fanStartTime) >= 300000) {
    // Fan has been running for 5 minutes, set fan speed to 100%
    analogWrite(fanPin, 255);
  }

  if (fanIsRunning && (millis() - fanStartTime) >= 7200000) {
    // Fan has been running for 2 hours, turn it off
    digitalWrite(fanPin, LOW);
    fanIsRunning = false;
  }

  delay(100);
}