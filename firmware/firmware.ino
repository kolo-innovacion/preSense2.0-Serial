//KOLO preSense 2.0
//FIRMWARE VERSION: 2.1.1
//Authors: Eduardo Gante + Alejandro Thacker
// Date: 01/02/2019
//Arduino Nano ATmega328P + MaxBotix MaxSonar EZ1 (MB1010)
// pin D11: PWM input from ultrasonic sensor PW(M) pin output
// pin D8: left LED outputh
// pin D10: right LED output
// pin A0: input from 50KOhm trimPot output
// pin D3: normally high output (to player)
// pin D4: normally low output (to player)

//constants
const int pwmPin = 11;//
const int leftLed = 8;//era 2
const int rightLed = 10;//era 4
const int builtinLed = 13;//
const int trimPin = 0;//era 7
const int normHi = 3;// era 10
const int normLo = 4;// era 8

const int leftLim = 10;
const int rightLim = 500;

//vars
long pulse, distance;
int triggerVal = 0;
int delTime = 33;//30 FPS (1000ms/33ms)
int rangeTime = 200;
bool leftOut = false;
bool rightOut = false;
bool inRange = true;

bool curr = false;
bool prev = false;

//reset after 12 hours (in millis)

int maxTime = 43200000;

//continuity
int index = 0;
int maxSamples = 6;
bool samples[6];//array size must equal maxSamples!
bool currAch = false;
bool prevAch = false;

//WARNING: enabling developer mode will enable serial communication, output values through the USB port and change the FPS to 3
bool developer = false;

void setup() {
  startSerial();
  setPinModes();
  showAlive();
}

void loop() {
  checkRange();
  rangeIndicator();
  getDistance();
  //printVals();
  setOutputs();
  //checkReset();
  delay(delTime);
}

void checkIndex() {
  if (index >= maxSamples) {
    checkSamples();
    index = 0;
  } else {
    index++;
  }
}
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void checkReset() {
  if (millis() > maxTime) {
    resetFunc();  //call reset
  } else {}
}

void setOutputs() {
  if ((distance < triggerVal) && (inRange)) {
    curr = true;
    setActive();

  }
  else
  {
    curr = false;
    setNormal();
  }
  checkStatus();
  checkIndex();
  statusUpdate();
}

void statusUpdate() {

  prev = curr;
}

void checkStatus() {
  if ((curr == true) && (prev == false)) {
    //when person is present and previous state was missing (person entry)

    samples[index] = true;
    //Serial.write("preSenseEntry\n");
  } else if ((curr == false) && (prev == true )) {
    //person exit

    samples[index] = false;
    //Serial.write("preSenseExit\n");
  } else if ((curr == true) && (prev == true)) {
    samples[index] = true;
    //Serial.write("Continuous");
  } else {}
}

void checkSamples() {
  bool compare = true;

  for (int i = 0; i < maxSamples; i++) {
    if (compare && samples[i]) {
      compare = true;
    } else {
      compare = false;
    }
  }//end for
  if (compare) {
    //Serial.println("ACHIEVED");
  } else {}
  currAch = compare;
  if ((currAch == true) && (prevAch == false)) {
    //Serial.println("---SPECIAL");
    Serial.write("preSenseEntry\n");
  } else {}
}

void getDistance() {
  //triggerVal = 1024 - analogRead(trimPin);
  triggerVal = analogRead(trimPin);
  pulse = pulseIn(pwmPin, HIGH);
  distance = (pulse * 2.54) / 147;
}

void printVals() {

  if (developer) {
    Serial.println("Trigger value: ");
    Serial.print(triggerVal);
    Serial.println(" ");
    Serial.println("Detected value: ");
    Serial.print(distance);
    Serial.println(" ");
  } else {}
}

void startSerial() {
  Serial.begin(9600);
  Serial.write("2.1.1\n");
}

void setPinModes() {
  pinMode(pwmPin, INPUT);
  pinMode(leftLed , OUTPUT);
  pinMode(rightLed , OUTPUT);
  pinMode(normHi , OUTPUT);
  pinMode(normLo , OUTPUT);
  pinMode(builtinLed , OUTPUT);
  setNormal();

}

void setNormal() {
  digitalWrite(normHi, HIGH);
  digitalWrite(normLo, LOW);

  digitalWrite(leftLed , LOW);
  digitalWrite(rightLed , LOW);
}

void setActive() {
  digitalWrite(normHi, LOW);
  digitalWrite(normLo, HIGH);

  digitalWrite(leftLed , HIGH);
  digitalWrite(rightLed , HIGH);
}

void checkRange() {
  if (triggerVal < leftLim) {
    leftOut = true;
    rightOut = false;
    inRange = false;
  } else if (triggerVal > rightLim) {
    leftOut = false;
    rightOut = true;
    inRange = false;
  } else {
    leftOut = false;
    rightOut = false;
    inRange = true;
  }
}

void rangeIndicator() {
  if (leftOut) {
    digitalWrite(leftLed , HIGH);
    digitalWrite(rightLed , LOW);
    delay(rangeTime);
    digitalWrite(leftLed , LOW);
    digitalWrite(rightLed , LOW);
    delay(rangeTime);
  } else if (rightOut) {
    digitalWrite(leftLed , LOW);
    digitalWrite(rightLed , HIGH);
    delay(rangeTime);
    digitalWrite(leftLed , LOW);
    digitalWrite(rightLed , LOW);
    delay(rangeTime);
  }
}

void showAlive() {
  digitalWrite(rightLed , HIGH);
  delay(rangeTime);
  digitalWrite(leftLed , HIGH);
  delay(rangeTime);
  digitalWrite(builtinLed , HIGH);
  delay(rangeTime);
  digitalWrite(leftLed , LOW);
  digitalWrite(rightLed , LOW);
  digitalWrite(builtinLed , LOW);
}
