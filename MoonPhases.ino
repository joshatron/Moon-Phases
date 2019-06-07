//Time for the moon period: 29 days, 12 hours, 44 minutes, 3 seconds
unsigned long moonPeriod = 2551443000;
//Time in a day
unsigned long offsetAmount = 86400000;
//Number of days to offset
int offset = 0;

unsigned long time = 0;

//Pins for lights from left to right
int statePins[6] = {9,3,10,5,6,11};
//Pin for button
//Button is configured as a pull down, so read HIGH for pressed
int buttonPin = 8;

//Brightness control, from 0 to 1
double brightnessMultiplier = 1;
//Brightnesses at each stage from left to right lights
int transitionBrightnesses[6][12] = {
 {255,255,255,255,255,255,0,0,0,0,0,0},
 {255,255,255,255,255,0,0,0,0,0,0,255},
 {255,255,255,255,0,0,0,0,0,0,255,255},
 {255,255,255,0,0,0,0,0,0,255,255,255},
 {255,255,0,0,0,0,0,0,255,255,255,255},
 {255,0,0,0,0,0,0,255,255,255,255,255}
};

enum ButtonState {MODE, OFFSET, BRIGHTNESS};
ButtonState currentState = MODE;
int stateIndicator = 0;

void setup() {
  for(int i = 0; i < 6; i++) {
    pinMode(statePins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);
}

void applyMoonState(double percentThrough) {
  int before = int(percentThrough * 12);
  int after = before + 1;
  double transition = percentThrough * 12 - before;
  before = before % 12;
  after = after % 12;
  for(int i = 0; i < 6; i++) {
    int brightness = transitionBrightnesses[i][before] * (1 - transition) + transitionBrightnesses[i][after] * transition;
    brightness = brightness * brightnessMultiplier;
    analogWrite(statePins[i], brightness);
  }
}

void runModeState() {
  if(digitalRead(buttonPin) == HIGH) {
    stateIndicator = (stateIndicator + 1) % 3;
  }
  else {
    switch(stateIndicator) {
      case 1:
        currentState = BRIGHTNESS;
        stateIndicator = 0;
        return;
      case 2:
        currentState = OFFSET;
        stateIndicator = 0;
        return;
    }
  }
  
  if(stateIndicator == 0) {
    //Using millis() - time makes it work even with the overflow that millis makes
    if(millis() - time > moonPeriod) {
      time = millis();
    }

    //(Time elapsed in period + adding halfway through the state + the offset based on button input) / the period of the state
    //The halfway through state is so the starting point is right at the full moon to make a good reference point
    double current = ((millis() - time) + (moonPeriod / 24.) + (offsetAmount * offset)) / moonPeriod;
    while(current > 1) {
      current--;
    }

    applyMoonState(current);
  }
  else {
    for(int i = 0; i < 6; i++) {
      if(i == stateIndicator - 1) {
        analogWrite(statePins[i], 255);
      }
      else {
        analogWrite(statePins[i], 0);
      }
    }
  }

  if(digitalRead(buttonPin) == HIGH) {
    delay(1000);
  }
}

void loop() {
  if(currentState == MODE) {
    runModeState();
  }
}
