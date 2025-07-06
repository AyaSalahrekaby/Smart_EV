#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <Servo.h>

#include "MAX30100_PulseOximeter.h"
#include "Adafruit_GFX.h"
#include "OakOLED.h"
#define REPORTING_PERIOD_MS 1000
OakOLED oled;

PulseOximeter pox;

uint32_t tsLastReport = 0;

// Servo movement variables
int startAngle = 110;
int normalSpO2End = 70;
int criticalSpO2End = 45;

int currentPos = 110;
int targetPos = 70; // initially
bool movingDown = true;  // direction of servo
unsigned long lastMoveTime = 0;
int moveSpeed = 15;  // milliseconds between steps
float lastSpO2 = 99;  // Store last SpO₂ read

#define air_volume      A2
#define exhalation_time A1
#define air_speed       A0
#define servo_pin       5
double angle_rad = PI / 180.0;
double angle_deg = 180.0 / PI;
double BPM;
double breathDuration;
double servar;
double currentTime = 0;
double lastTime = 0;

Servo servo;

#define BUTTON_PIN 2
volatile int buttonState = 0;
volatile int lastButtonState = 0;
bool isManual = false;

void onBeatDetected()
{

  //oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
  oled.display();
}
double getLastTime() {
  return currentTime = millis() / 1000.0 - lastTime;
}

void checkButton() {
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {  // LOW because of INPUT_PULLUP
    isManual = !isManual;
    
  }

  lastButtonState = buttonState;
}

void runManualMode() {
  lastTime = millis() / 1000.0;

  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print("V:");
  oled.println(7 * round(((analogRead(air_volume)) * 10) / 1000)); 

  oled.setCursor(0, 16);
  oled.print("ET:");
  oled.println(1.5 + 0.2 * round(((analogRead(exhalation_time)) * 20) / 1000)); 

  oled.setCursor(0, 30);
  oled.print("SP:");
  oled.println(0 + 0.21 * round(((analogRead(air_speed)) * 10) / 1000)); 

  oled.setCursor(0, 45);
  oled.print("BPM:");
  oled.print(round(BPM));

  oled.display();

  servo.write(110);
  _delay(0.8 + 0.2 * round(((analogRead(exhalation_time)) * 20) / 1000));

  servar = 110;
  while (!(servar < (110 - (7 * round(((analogRead(air_volume)) * 10) / 1000))))) {
    servo.write(servar);
    _delay(0 + 0.003 * round(((analogRead(air_speed)) * 10) / 1000));
    servar += -1;
  }

  servo.write(110);
  breathDuration = getLastTime();
  BPM = (breathDuration > 0) ? 60.0 / breathDuration : 0;

}

void runAutomaticMode() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float heartRate = pox.getHeartRate();
    float spo2 = pox.getSpO2();
    lastSpO2 = spo2;  // save for servo logic

    Serial.print("Heart BPM: ");
    Serial.print(heartRate);
    Serial.print("  -----  ");
    Serial.print("Oxygen Percent: ");
    Serial.println(spo2);

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    oled.println("Heart BPM");
    oled.setCursor(0, 16);
    oled.println(heartRate);

    oled.setCursor(0, 30);
    oled.println("SpO2");
    oled.setCursor(0, 45);
    oled.println(spo2);

    oled.display();
    tsLastReport = millis();
  }

  // Move servo smoothly
  if (millis() - lastMoveTime > moveSpeed) {
    lastMoveTime = millis();

    int endAngle = (lastSpO2 > 90) ? normalSpO2End : criticalSpO2End;

    if (movingDown) {
      if (currentPos > endAngle) {
        currentPos--;
        servo.write(currentPos);
      } else {
        movingDown = false;  // change direction when reaching end
      }
    } else {
      if (currentPos < startAngle) {
        currentPos++;
        servo.write(currentPos);
      } else {
        movingDown = true;  // change direction when reaching start
      }
    }
  }

}

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  oled.begin();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0, 0);
  oled.println("Initializing pulse oximeter..");
  oled.display();
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    Serial.println("FAILED");
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("FAILED");
    oled.display();
    for (;;);
  } else {
    Serial.println("SUCCESS");
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("SUCCESS");
    oled.display();
  }

  pox.setOnBeatDetectedCallback(onBeatDetected);

  pinMode(air_volume, INPUT);
  pinMode(exhalation_time, INPUT);
  pinMode(air_speed, INPUT);
  servo.attach(servo_pin);
  servo.write(startAngle);
  currentPos = startAngle;
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN) , checkButton, CHANGE);
}

void loop() {
  pox.update();
  if (isManual) {
    runManualMode();
  } else {
    runAutomaticMode();
  }

}

void _delay(float seconds) {
  pox.update();
  long endTime = millis() + seconds * 1000;
  while (millis() < endTime) _loop();
}

void _loop() {
  // Placeholder
  pox.update();
}
