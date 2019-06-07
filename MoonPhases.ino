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

void setup() {
  for(int i = 0; i < 6; i++) {
    pinMode(statePins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);
}

void applyState(double percentThrough) {
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

void loop() {
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

  applyState(current);

  //Button is configured as a pull down
  if(digitalRead(buttonPin) == HIGH) {
    //Increase by 1, but loop back down to 0 if more than a full period
    offset = (offset + 1) % 30;
    //Do the delay because otherwise it will cycle the offset really quickly
    delay(500);
  }
}
