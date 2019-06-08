//Time for the full moon period: 29 days, 12 hours, 44 minutes, 3 seconds
//Lengths of a period
unsigned long periods[6] = {12000, 720000, 3600000, 86400000, 604800000, 2551443000};
//Length of an offset
unsigned long offsets[6] = {1000, 60000, 300000, 7200000, 50400000, 86400000};
//Number of offsets
int offset = 0;
//Which speed to do
int currentPeriod = 5;

unsigned long currentTime = 0;

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

enum ButtonState {NORMAL, MODE, BRIGHTNESS, PERIOD, OFFSET};
ButtonState currentState = NORMAL;
int stateIndicator = 0;
unsigned long stateTimer = 0;

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

  //(Time elapsed in period + adding halfway through the state + the offset based on button input) / the period of the state
  //The halfway through state is so the starting point is right at the full moon to make a good reference point
  double current = ((millis() - currentTime) + (periods[currentPeriod] / 24.) + (offsets[currentPeriod] * offset)) / periods[currentPeriod];
  while(current > 1) {
    current--;
  }

  applyMoonState(current);
}

void runModeState() {
  if(millis() - stateTimer > 2000) {
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

void runBrightnessState() {
  if(millis() - stateTimer > 3000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 255);
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

void runPeriodState() {
  if(millis() - stateTimer > 2000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 255);
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

void runOffsetState() {
  if(millis() - stateTimer > 2000) {
    currentState = NORMAL;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 255);
    }
    delay(200);
    return;
  }
  
  for(int i = 0; i < 6; i++) {
    analogWrite(statePins[i], 255);
  }

  if(digitalRead(buttonPin) == HIGH) {
    stateTimer = millis();
    offset = (offset + 1) % 30;
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(200);
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 255);
    }
    delay(300);
  }
  
}

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
