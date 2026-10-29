#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte buttonPin = 2;
const byte RledPin = 9;
const byte GledPin = 10;
const byte BledPin = 11;
const byte lightPin = A0;

const int minMood = 0;
const int maxMood = 20;
int mood = 10; 

const unsigned long debounceTime = 35;
const unsigned long idleTime = 5000;
const unsigned long normalDecayTime = 1000;
const unsigned long darkHoldTime = 3000;
const unsigned long darkDecayTime = 750;
const unsigned long veryDarkDecayTime = 1000;

const bool darkReadingIsHigh = true;
const int darkThreshold = 600;
const int veryDarkThreshold = 800;

unsigned long lastPetTime = 0;
unsigned long lastMoodChangeTime = 0;
unsigned long lastDebounceTime = 0;
unsigned long darkSince = 0;
unsigned long lastLightReport = 0;

int lastButtonReading = LOW;
int stableButtonState = LOW;
int lightValue = 0;

bool wasDark = false;
bool isDark = false;
bool isVeryDark = false;
bool displayNeedsUpdate = true;


void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();


  unsigned long now = millis();
  lastPetTime = now;
  lastMoodChangeTime = now;

  updateLedColor();
  updateLcd();
}

void loop() {
  unsigned long now = millis();

  checkButton(now);
  checkLightSensor(now);
  updateMoodOverTime(now);

  if (displayNeedsUpdate) {
    updateLcd();
    displayNeedsUpdate = false;
  }
}

void checkButton(unsigned long now) {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonReading) {
    lastDebounceTime = now;
    lastButtonReading = reading;
  }

  if (now - lastDebounceTime > debounceTime) {
    if (reading != stableButtonState) {
      stableButtonState = reading;


      if (stableButtonState == HIGH) {
        petToy(now);
      }
    }
  }
}

void petToy(unsigned long now) {
  if (mood < maxMood) {
    mood++;
    displayNeedsUpdate = true;
    updateLedColor();
  }

  lastPetTime = now;
  lastMoodChangeTime = now;
}

void checkLightSensor(unsigned long now) {
  lightValue = analogRead(lightPin);

  if (darkReadingIsHigh) {
    isDark = (lightValue >= darkThreshold);
    isVeryDark = (lightValue >= veryDarkThreshold);
  } else {
    isDark = (lightValue <= darkThreshold);
    isVeryDark = (lightValue <= veryDarkThreshold);
  }

  if (isDark && !wasDark) {
    darkSince = now;
  }

  if (!isDark) {
    darkSince = 0;
  }

  wasDark = isDark;

  if (now - lastLightReport >= 500) {
    Serial.print("Light AO: ");
    Serial.println(lightValue);
    lastLightReport = now;
  }
}

void updateMoodOverTime(unsigned long now) {
  unsigned long darkDuration = 0;
  if (isDark) {
    darkDuration = now - darkSince;
  }

  if (isDark && darkDuration >= darkHoldTime) {
    if (isVeryDark) {
      if (now - lastMoodChangeTime >= veryDarkDecayTime) {
        changeMoodBy(-2);
        lastMoodChangeTime = now;
      }
    } else {
      if (now - lastMoodChangeTime >= darkDecayTime) {
        changeMoodBy(-1);
        lastMoodChangeTime = now;
      }
    }
  } else {
    if (now - lastPetTime >= idleTime &&
        now - lastMoodChangeTime >= normalDecayTime) {
      changeMoodBy(-1);
      lastMoodChangeTime = now;
    }
  }
}

void changeMoodBy(int amount) {
  int newMood = mood + amount;

  if (newMood < minMood) {
    newMood = minMood;
  }
  if (newMood > maxMood) {
    newMood = maxMood;
  }

  if (newMood != mood) {
    mood = newMood;
    displayNeedsUpdate = true;
    updateLedColor();
  }
}

void updateLedColor() {
  int redValue;
  int greenValue;
  int blueValue;

  if (mood < 10) {
    redValue = 0;
    greenValue = map(mood, 0, 10, 0, 255);
    blueValue = map(mood, 0, 10, 255, 0);
  } else {
    redValue = map(mood, 10, 20, 0, 255);
    greenValue = 255;
    blueValue = 0;
  }

  analogWrite(RledPin, 255 - redValue);
  analogWrite(GledPin, 255 - greenValue);
  analogWrite(BledPin, 255 - blueValue);
}

void updateLcd() {
  const char *moodName;

  if (mood <= 6) {
    moodName = "SAD:(";
  } else if (mood <= 13) {
    moodName = "NEUTRAL";
  } else {
    moodName = "HAPPY:D";
  }

  lcd.setCursor(0, 0);
  lcd.print("Mochi: ");
  lcd.print(moodName);

  int nextColumn = 6 + strlen(moodName);
  while (nextColumn < 13) {
    lcd.print(' ');
    nextColumn++;
  }

  int filledBlocks = map(mood, minMood, maxMood, 0, 10);
  lcd.setCursor(0, 1);
  lcd.print('[');

  for (int i = 0; i < 10; i++) {
    if (i < filledBlocks) {
      lcd.print('*');
    } else {
      lcd.print('-');
    }
  }

  lcd.print("] ");
  lcd.print(mood / 10);
  lcd.print(mood % 10);
  lcd.print(' ');
}
