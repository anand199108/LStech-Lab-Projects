#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TRIG_PIN 7
#define ECHO_PIN 8
#define VOLTAGE_PIN A0
#define BUTTON_PIN 4
#define LED_PWM_PIN 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 0 = Distance
// 1 = Voltage
// 2 = LED
byte screenMode = 0;

byte brightnessPercent = 0;

// Button
bool lastButton = HIGH;
unsigned long lastPress = 0;

// EEPROM
#define EEPROM_SCREEN 0
#define EEPROM_BRIGHTNESS 1


void setup() {

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PWM_PIN, OUTPUT);


  // -----------------------------
  // LOAD LAST SETTINGS
  // -----------------------------

  byte savedScreen = EEPROM.read(EEPROM_SCREEN);
  byte savedBrightness = EEPROM.read(EEPROM_BRIGHTNESS);


  if (savedScreen <= 2) {
    screenMode = savedScreen;
  } else {
    screenMode = 0;
  }


  if (savedBrightness == 0 ||
      savedBrightness == 25 ||
      savedBrightness == 50 ||
      savedBrightness == 75 ||
      savedBrightness == 100) {

    brightnessPercent = savedBrightness;

  } else {

    brightnessPercent = 0;
  }


  // Restore LED brightness
  int pwmValue = map(
    brightnessPercent,
    0,
    100,
    0,
    255
  );

  analogWrite(LED_PWM_PIN, pwmValue);


  // -----------------------------
  // OLED
  // -----------------------------

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    while (true) {
    }

  }


  // -----------------------------
  // STARTING SCREEN
  // -----------------------------

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);

  display.setCursor(25, 10);
  display.println("LStech");

  display.setCursor(40, 33);
  display.println("Lab");

  display.setTextSize(1);

  display.setCursor(23, 55);
  display.println("(AI,Robotics)");

  display.display();

  delay(5000);


  // -----------------------------
  // CREATED BY
  // -----------------------------

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(30, 0);
  display.println("Created By");

  display.setTextSize(2);

  display.setCursor(30, 25);
  display.println("Anand");

  display.setCursor(42, 48);
  display.println("Sir");

  display.display();

  delay(5000);
}


void loop() {

  bool button = digitalRead(BUTTON_PIN);


  // -----------------------------
  // BUTTON PRESSED
  // -----------------------------

  if (lastButton == HIGH && button == LOW) {

    if (millis() - lastPress > 300) {

      lastPress = millis();


      // Distance -> Voltage
      if (screenMode == 0) {

        screenMode = 1;

      }


      // Voltage -> LED
      else if (screenMode == 1) {

        screenMode = 2;

      }


      // LED brightness
      else {

        if (brightnessPercent == 0) {

          brightnessPercent = 25;

        }

        else if (brightnessPercent == 25) {

          brightnessPercent = 50;

        }

        else if (brightnessPercent == 50) {

          brightnessPercent = 75;

        }

        else if (brightnessPercent == 75) {

          brightnessPercent = 100;

        }

        else {

          brightnessPercent = 0;
          screenMode = 0;

        }


        int pwmValue = map(
          brightnessPercent,
          0,
          100,
          0,
          255
        );

        analogWrite(LED_PWM_PIN, pwmValue);
      }


      // -----------------------------
      // SAVE SETTINGS
      // -----------------------------

      EEPROM.update(
        EEPROM_SCREEN,
        screenMode
      );

      EEPROM.update(
        EEPROM_BRIGHTNESS,
        brightnessPercent
      );

    }
  }


  lastButton = button;


  // -----------------------------
  // DISPLAY
  // -----------------------------

  if (screenMode == 0) {

    showDistance();

  }

  else if (screenMode == 1) {

    showVoltage();

  }

  else {

    showLED();

  }

  delay(100);
}


// =================================
// DISTANCE
// =================================

void showDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);


  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );


  float distance = 0;


  if (duration > 0) {

    distance = duration * 0.0343 / 2;

  }


  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(1);

  display.setCursor(30, 0);
  display.println("LStech Lab");

  display.setTextSize(2);

  display.setCursor(15, 18);
  display.println("Distance");

  display.setCursor(20, 43);

  display.print(distance, 1);

  display.println(" cm");

  display.display();
}


// =================================
// VOLTAGE
// =================================

void showVoltage() {

  int sensorValue = analogRead(VOLTAGE_PIN);


  float voltage =
    sensorValue * (25.0 / 1023.0);


  // Calibration
  voltage = voltage * 1.025;


  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(1);

  display.setCursor(30, 0);
  display.println("LStech Lab");

  display.setTextSize(2);

  display.setCursor(20, 18);
  display.println("Voltage");

  display.setCursor(15, 43);

  display.print(voltage, 2);

  display.println(" V");

  display.display();
}


// =================================
// LED
// =================================

void showLED() {

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(1);

  display.setCursor(30, 0);
  display.println("LStech Lab");

  display.setTextSize(2);

  display.setCursor(15, 18);
  display.println("LED Light");

  display.setCursor(38, 43);

  display.print(brightnessPercent);

  display.println("%");

  display.display();
}