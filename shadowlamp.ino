const int buttonPin = 2; 
const int RledPin = 3;
const int GledPin = 6; 
const int BledPin = 5;

int buttonState = 0;
int ledcolor = 0; 

bool ButtonPressed = false;

int currentR = 255;
int currentG = 255;
int currentB = 255;

void setup() {
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if(buttonState == LOW && !ButtonPressed){
    ledcolor = ledcolor + 1;
    ButtonPressed = true;
    if(ledcolor >= 8) {
      ledcolor = 0;
    }
    delay(50);
  }
  
  if(buttonState == HIGH && ButtonPressed){
    ButtonPressed = false;
  }

  int targetR = 255;
  int targetG = 255;
  int targetB = 255;

  if (ledcolor == 0) {
    targetR = 255;
    targetG = 255;
    targetB = 255;
  }

  // red
  else if (ledcolor == 1) {
    targetR = 0;
    targetG = 255;
    targetB = 255;
  }

  // green
  else if (ledcolor == 2) {
    targetR = 255;
    targetG = 0;
    targetB = 255;
  }

  // blue
  else if (ledcolor == 3) {
    targetR = 255;
    targetG = 255;
    targetB = 0;
  }

  // yellow
  else if (ledcolor == 4) {
    targetR = 0;
    targetG = 0;
    targetB = 255;
  }

  //purple
  else if (ledcolor == 5) {
    targetR = 0;
    targetG = 255;
    targetB = 0;
  }

  //cyan
  else if (ledcolor == 6) {
    targetR = 255;
    targetG = 0;
    targetB = 0;
  }

  // white
  else if (ledcolor == 7) {
    targetR = 0;
    targetG = 0;
    targetB = 0;
  }

  if (currentR < targetR) currentR++;
  else if (currentR > targetR) currentR--;

  if (currentG < targetG) currentG++;
  else if (currentG > targetG) currentG--;

  if (currentB < targetB) currentB++;
  else if (currentB > targetB) currentB--;

  analogWrite(RledPin, currentR);
  analogWrite(GledPin, currentG);
  analogWrite(BledPin, currentB);

  delay(10);
}