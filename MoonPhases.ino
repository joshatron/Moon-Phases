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

enum ButtonState {NORMAL, MODE, BRIGHTNESS, PERIOD, OFFSET, DIRECTION, SMOOTHNESS};
ButtonState currentState;
bool buttonPressed;
unsigned long buttonTimer;

int stateIndicator;
unsigned long lastChanged;

//The brightness is from 0 to 1
double brightnessMultiplier;

//Periods are 12 seconds, 12 minutes, 1 hour, 1 day, 12 days, and 1 full moon period
//Time for the full moon period: 29 days, 12 hours, 44 minutes, 3 seconds
unsigned long periods[6] = {12000, 720000, 3600000, 86400000, 1036800000, 2551443000};
int currentPeriod;

//Offsets are 1 second, 1 minute, 5 minutes, 2 hours, 1 day, and 1 day
unsigned long offsets[6] = {1000, 60000, 300000, 7200000, 86400000, 86400000};
int offset;
int offsetFull;

bool backward = false;

bool smooth = true;

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
  //Using millis() - currentTime makes it work even with the overflow that millis makes
  if(millis() - currentTime > periods[currentPeriod]) {
    currentTime += periods[currentPeriod];
  }
  //Keep track of the full moon cycle time separately
  if(millis() - currentTimeFull > periods[5]) {
    currentTimeFull += periods[5];
  }

  double current = 0;
  if(currentPeriod == 5) {
    current = ((millis() - currentTimeFull) + (offsets[5] * offsetFull)) / double(periods[5]);
  }
  else {
    //Time 0 is the full moon
    current = ((millis() - currentTime) + (offsets[currentPeriod] * offset)) / double(periods[currentPeriod]);
  }
  while(current > 1) {
    current--;
  }
  if(backward) {
    current = 1 - current;
  }

  int before = int(current * 12);
  int after = before + 1;
  double transition = current * 12 - before;
  before = before % 12;
  after = after % 12;
  for(int i = 0; i < 6; i++) {
    int brightness = 0;
    if(smooth) {
      brightness = transitionBrightnesses[i][before] * (1 - transition) + transitionBrightnesses[i][after] * transition;
    }
    else {
      if(transition < .5) {
        brightness = transitionBrightnesses[i][before];
      }
      else {
        brightness = transitionBrightnesses[i][after];
      }
    }
    brightness = brightness * brightnessMultiplier;
    analogWrite(statePins[i], brightness);
  }
}

/*
 * Run MODE state
 */
void runModeState(bool next) {
  if(next) {
    stateIndicator = (stateIndicator + 1) % 6;
    lastChanged = millis();
  }

  unsigned long animationTime = millis() - lastChanged;

  if(animationTime < 1000) {
    switch(stateIndicator) {
      //normal
      case 0:
        for(int i = 0; i < 6; i++) {
          analogWrite(statePins[i], 255);
        }
        break;
        
      //brightness
      case 1:
        double tempBrightness;
        if(animationTime < 500) {
          tempBrightness = animationTime / 500.;
        }
        else {
          tempBrightness = 1 - ((animationTime - 500) / 500.);
        }
        
        for(int i = 0; i < 6; i++) {
          analogWrite(statePins[i], 255 * tempBrightness);
        }
        break;
      
      //speed
      case 2:
        int spot;
        if(animationTime < 250) {
          spot = int((animationTime / 250.) * 6);
        }
        else {
          spot = int(((animationTime - 250) / 750.) * 6);
        }
        spot = spot % 6;
        
        for(int i = 0; i < 6; i++) {
          if(i == spot) {
            analogWrite(statePins[i], 255);
          }
          else {
            analogWrite(statePins[i], 0);
          }
        }
        break;
      
      //offset
      case 3:
        for(int i = 0; i < 6; i++) {
          if((animationTime > 500 && animationTime < 600)) {
            analogWrite(statePins[i], 0);
          }
          else {
            analogWrite(statePins[i], 255);
          }
        }
        break;
      
      //direction
      case 4:
        int loc;
        if(animationTime < 500) {
          loc = int((animationTime / 500.) * 3) + 3;
        }
        else {
          loc = 2 - int(((animationTime - 500) / 500.) * 3);
        }
        for(int i = 0; i < 6; i++) {
          if(i == loc) {
            analogWrite(statePins[i], 255);
          }
          else {
            analogWrite(statePins[i], 0);
          }
        }
        break;

      //smoothness
      case 5:
        double b;
        if(animationTime < 500) {
          b = animationTime / 500.;
        }
        else if(animationTime < 750){
          b = 0.;
        }
        else {
          b = 1.;
        }
        for(int i = 0; i < 6; i++) {
          analogWrite(statePins[i], 255 * b);
        }
        break;
    }
  }
  else {
    if(stateIndicator != 0) {
      for(int i = 0; i < 6; i++) {
        if(i == stateIndicator - 1) {
          analogWrite(statePins[i], 255);
        }
        else {
          analogWrite(statePins[i], 0);
        }
      }
    }
    else {
      for(int i = 0; i < 6; i++) {
        analogWrite(statePins[i], 255);
      }
    }
  }
}

/*
 * Run BRIGHTNESS state
 */
void runBrightnessState(bool next) {
  if(next) {
    brightnessMultiplier += .2;
    if(brightnessMultiplier > 1) {
      brightnessMultiplier = 0;
    }
  }
  
  for(int i = 0; i < 6; i++) {
    analogWrite(statePins[i], 255 * brightnessMultiplier);
  }
}

/*
 * Run PERIOD state
 */
void runPeriodState(bool next) {
  if(next) {
    currentPeriod = (currentPeriod + 1) % 6;
  }
  
  for(int i = 0; i < 6; i++) {
    if(i == currentPeriod) {
      analogWrite(statePins[i], 255);
    }
    else {
      analogWrite(statePins[i], 0);
    }
  }
}

/*
 * Run OFFSET state
 */
void runOffsetState(bool next) {
  if(next) {
    if(currentPeriod == 5) {
      offsetFull = (offsetFull + 1) % 30;
    }
    else {
      offset = (offset + 1) % 12;
    }
    for(int i = 0; i < 6; i++) {
      analogWrite(statePins[i], 0);
    }
    delay(100);
  }
  
  for(int i = 0; i < 6; i++) {
    analogWrite(statePins[i], 255);
  }
}

/*
 * Run DIRECTION state
 */
void runDirectionState(bool next) {
  if(next) {
    backward = !backward;
  }

  for(int i = 0; i < 6; i++) {
    if((backward && i < 3) || (!backward && i >= 3)) {
      analogWrite(statePins[i], 255);
    }
    else {
      analogWrite(statePins[i], 0);
    }
  }
}

/*
 * Run SMOOTHNESS state
 */
void runSmoothnessState(bool next) {
  if(next) {
    smooth = !smooth;
  }

  for(int i = 0; i < 6; i++) {
    if(i == 0 || i == 2 || i == 4) {
      analogWrite(statePins[i], 255);
    }
    else {
      if(smooth) {
        analogWrite(statePins[i], 75);
      }
      else {
        analogWrite(statePins[i], 0);
      }
    }
  }
}

/*
 * Determine which state it is in and run corresponding function
 */
void loop() {
  bool pressed = false;
  bool switched = false;
  if(digitalRead(buttonPin) == HIGH) {
    pressed = true;
    //For debouncing. If button presses are getting triggered twice sometimes, increase this number
    delay(20);
  }
  if(pressed != buttonPressed) {
    buttonTimer = millis();
    buttonPressed = pressed;
    switched = true;
  }

  //Logic for switching modes
  if(currentState != NORMAL) {
    if(!buttonPressed && millis() - buttonTimer > 2000) {
      if(currentState == MODE) {
        buttonTimer = millis();
        switch(stateIndicator) {
          case 0:
            currentState = NORMAL;
            break;
          case 1:
            currentState = BRIGHTNESS;
            break;
          case 2:
            currentState = SPEED;
            break;
          case 3:
            currentState = OFFSET;
            break;
          case 4:
            currentState = DIRECTION;
            break;
          case 5:
            currentState = SMOOTHNESS;
            break;
        }
        stateIndicator = 0;
      }
      else {
        currentState = NORMAL;
      }
      
      for(int i = 0; i < 6; i++) {
        analogWrite(statePins[i], 0);
      }
      delay(100);
    }
  }
  else if(buttonPressed) {
    currentState = MODE;
  }

  //Logic for whether next should be true
  bool next = false;
  if(switched && buttonPressed) {
    next = true;
  }
  else if(buttonPressed && millis() - buttonTimer > 750) {
    next = true;
    buttonTimer = millis();
  }
  
  switch(currentState) {
    case NORMAL:
      runNormalState();
      break;
    case MODE:
      runModeState(next);
      break;
    case BRIGHTNESS:
      runBrightnessState(next);
      break;
    case PERIOD:
      runPeriodState(next);
      break;
    case OFFSET:
      runOffsetState(next);
      break;
    case DIRECTION:
      runDirectionState(next);
      break;
    case SMOOTHNESS:
      runSmoothnessState(next);
      break;
  }
}
