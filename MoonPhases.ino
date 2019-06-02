unsigned long moonPeriod = 2551443000;
unsigned long time = 0;

int statePins[6] = {2,3,4,5,6,7};

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
  if(millis() - time > moonPeriod) {
    time = millis();
  }

  int state = (millis() - time) / (moonPeriod / 12);
  if(state > 11) {
    state = 11;
  }

  applyState(state);
}
