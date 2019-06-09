//Pins for lights from left to right
int statePins[6] = {9,3,10,5,6,11};
//Button is configured as a pull down, so read HIGH for pressed
int buttonPin = 8;


unsigned long currentTimeFull;
unsigned long currentTime;
//Brightnesses at each stage from left to right lights
int transitionBrightnesses[6][12] = {
 {255,255,255,255,255,255,0,0,0,0,0,0},
 {255,255,255,255,255,0,0,0,0,0,0,255},
 {255,255,255,255,0,0,0,0,0,0,255,255},
 {255,255,255,0,0,0,0,0,0,255,255,255},
 {255,255,0,0,0,0,0,0,255,255,255,255},
 {255,0,0,0,0,0,0,255,255,255,255,255}
};

enum ButtonState {NORMAL, MODE, BRIGHTNESS, PERIOD, OFFSET};
ButtonState currentState;
int stateIndicator;
unsigned long stateTimer;

//The brightness is from 0 to 1
double brightnessMultiplier;

//Periods are 12 seconds, 12 minutes, 1 hour, 1 day, 12 days, and 1 full moon period
//Time for the full moon period: 29 days, 12 hours, 44 minutes, 3 seconds
unsigned long periods[6] = {12000, 720000, 3600000, 86400000, 1036800000, 2551443000};
int currentPeriod;

//Offsets are 1 second, 1 minute, 5 minutes, 2 hours, 14 hours, and 1 day
unsigned long offsets[6] = {1000, 60000, 300000, 7200000, 50400000, 86400000};
int offset;
int offsetFull;

void setup() {
  for(int i = 0; i < 6; i++) {
    pinMode(statePins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);

  currentState = NORMAL;
  brightnessMultiplier = 1;
  currentPeriod = 5;
  offset = 0;
  currentTime = millis();
}

/*
 * Run NORMAL state
 * If the button is pressed, go to MODE state
 * otherwise display the current moon phase
 */
void runNormalState() {
  if(digitalRead(buttonPin) == HIGH) {
    currentState = MODE;
    stateIndicator = 0;
    stateTimer = millis();
    return;
  }

  //Using millis() - currentTime makes it work even with the overflow that millis makes
  if(millis() - currentTime > periods[currentPeriod]) {
    currentTime = millis();
  }
  //Keep track of the full moon cycle time separately
  if(millis() - currentTimeFull > periods[5]) {
    currentTimeFull = millis();
  }

  double current = 0;
  if(currentPeriod == 5) {
    //Time 0 is the full moon
    current = ((millis() - currentTimeFull) + (offsets[5] * offsetFull)) / double(periods[5]);
    while(current > 1) {
      current--;
    }
  }
  else {
    //Time 0 is the full moon
    current = ((millis() - currentTime) + (offsets[currentPeriod] * offset)) / double(periods[currentPeriod]);
    //Should only happen because of offsets
    while(current > 1) {
      current--;
    }
  }

  int before = int(current * 12);
  int after = before + 1;
  double transition = current * 12 - before;
  before = before % 12;
  after = after % 12;
  for(int i = 0; i < 6; i++) {
    int brightness = transitionBrightnesses[i][before] * (1 - transition) + transitionBrightnesses[i][after] * transition;
    brightness = brightness * brightnessMultiplier;
    analogWrite(statePins[i], brightness);
  }
}

/*
 * Run MODE state
 * Every button press, change choice and display
 * If it's been 3 seconds since a press, go to that state
 */
void runModeState() {
  if(millis() - stateTimer > 3000) {
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    switch(stateIndicator) {
      case 0:
        currentState = NORMAL;
        return;
      case 1:
        currentState = BRIGHTNESS;
        stateTimer = millis();
        return;
      case 2:
        currentState = SPEED;
        stateTimer = millis();
        return;
      case 3:
        currentState = OFFSET;
        stateTimer = millis();
        return;
    }
  }
  
  if(digitalRead(buttonPin) == HIGH) {
    stateTimer = millis();
    stateIndicator = (stateIndicator + 1) % 4;

    if(stateIndicator == 0) {
      for(int i = 0; i < 6; i++) {
        analogWrite(statePins[i], 255);
      }
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

    delay(250);
  }
}

/*
 * Run BRIGHTNESS state
 * Every button press, change choice and display
 * If it's been 3 seconds since a press, go to back to normal state
 */
void runBrightnessState() {
  if(millis() - stateTimer > 3000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      if(brightnessMultiplier < .1) {
        analogWrite(statePins[i], 255);
      }
      else {
        analogWrite(statePins[i], 0);
      }
    }
    delay(200);
    return;
  }

  bool pressed = false;
  if(digitalRead(buttonPin) == HIGH) {
    stateTimer = millis();
    brightnessMultiplier += .2;
    if(brightnessMultiplier > 1) {
      brightnessMultiplier = 0;
    }
    pressed = true;
  }
  
  for(int i = 0; i < 6; i++) {
    analogWrite(statePins[i], 255 * brightnessMultiplier);
  }

  if(pressed) {
    delay(250);
  }
}

/*
 * Run PERIOD state
 * Every button press, change choice and display
 * If it's been 3 seconds since a press, go to back to normal state
 */
void runPeriodState() {
  if(millis() - stateTimer > 3000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    return;
  }

  bool pressed = false;
  if(digitalRead(buttonPin) == HIGH) {
    stateTimer = millis();
    currentPeriod = (currentPeriod + 1) % 6;
    pressed = true;
  }
  
  for(int i = 0; i < 6; i++) {
    if(i == currentPeriod) {
      analogWrite(statePins[i], 255);
    }
    else {
      analogWrite(statePins[i], 0);
    }
  }

  if(pressed) {
    delay(250);
  }
}

/*
 * Run OFFSET state
 * Every button press, add to offset and flash
 * If it's been 3 seconds since a press, go to back to normal state
 */
void runOffsetState() {
  if(millis() - stateTimer > 3000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    return;
  }
  
  for(int i = 0; i < 6; i++) {
    analogWrite(statePins[i], 255);
  }

  if(digitalRead(buttonPin) == HIGH) {
    stateTimer = millis();
    if(currentPeriod == 5) {
      offsetFull = (offsetFull + 1) % 30;
    }
    else {
      offset = (offset + 1) % 12;
    }
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 255);
    }
    delay(800);
  }
  
}

/*
 * Determine which state it is in and run corresponding function
 */
void loop() {
  switch(currentState) {
    case NORMAL:
      runNormalState();
      break;
    case MODE:
      runModeState();
      break;
    case BRIGHTNESS:
      runBrightnessState();
      break;
    case PERIOD:
      runPeriodState();
      break;
    case OFFSET:
      runOffsetState();
      break;
  }
}
