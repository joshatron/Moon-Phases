//Time for the moon period: 29 days, 12 hours, 44 minutes, 3 seconds
unsigned long moonPeriod = 2551443000;
//Time in a day
unsigned long offsetAmount = 86400000;
//Number of days to offset, based on the button
int offset = 0;

unsigned long time = 0;

//Pins for lights from left to right
int statePins[6] = {2,3,4,5,6,7};
//Pin for button
int buttonPin = 8;

int currentState = -1;
bool states[12][6] = {
  {true,true,true,true,true,true},
  {true,true,true,true,true,false},
  {true,true,true,true,false,false},
  {true,true,true,false,false,false},
  {true,true,false,false,false,false},
  {true,false,false,false,false,false},
  {false,false,false,false,false,false},
  {false,false,false,false,false,true},
  {false,false,false,false,true,true},
  {false,false,false,true,true,true},
  {false,false,true,true,true,true},
  {false,true,true,true,true,true},
};

void setup() {
  for(int i = 0; i < 6; i++) {
    pinMode(statePins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);
}

void applyState(int state) {
  for(int i = 0; i < 6; i++) {
    if(states[state][i]) {
      digitalWrite(statePins[i], HIGH);
    }
    else {
      digitalWrite(statePins[i], LOW);
    }
  }
}

void loop() {
  //Using millis() - time makes it work even with the overflow that millis makes
  if(millis() - time > moonPeriod) {
    time = millis();
  }

  //(Time elapsed in period + adding halfway through the state + the offset based on button input) / the period of the state
  //The halfway through state is so the starting point is right at the full moon to make a good reference point
  int state = ((millis() - time) + (moonPeriod / 24) + (offsetAmount * offset)) / (moonPeriod / 12);
  state = state % 12;

  //Update the state if in a new one
  if(currentState != state) {
    currentState = state;
    applyState(state);
  }

  //Assumes the button is pull down
  if(digitalRead(buttonPin) == HIGH) {
    //Increase by 1, but loop back down to 0 if more than a full period
    offset = (offset + 1) % 30;
    //Do the delay because otherwise it will cycle the offset really quickly
    delay(500);
  }
}
