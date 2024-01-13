//Timing Initiation
int Millis = millis();
int prevMillis = 0;
int forwardTime = 2000;
int turnTime = 530;

//Motor Initiation
int motor1pin1 = 7;
int motor1pin2 = 6;
int motor2pin1 = 5;
int motor2pin2 = 4;
int speed1 = 9;
int speed2 = 10;

void setup() {
  //pins
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(speed1, OUTPUT);
  pinMode(speed2, OUTPUT);
  //Speed
  analogWrite(speed1, 255);
  analogWrite(speed2, 195);
}
void loop() {
  forward(1);
  left(1);
  forward(1);
  left(1);
  forward(1);
  left(1);
  forward(1);
  left(1);
  stop();
}

int forward(int loops){
  for (int i = 0; i < loops; i++){
    //set direction of both motors
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    delay(forwardTime); //How long it goes forward
  }
}

int right(int loops){
  for (int i = 0; i < loops; i++){
    //set direction of both motors
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
    delay(turnTime); //How long it turns
  }
}
int left(int loops){
  //How much it loops
  for (int i = 0; i < loops; i++){
    //set direction of both motors
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    delay(turnTime); //How long it turns
  }
}

void stop(){
  // Set speed of both motors to 0
  analogWrite(speed1, 0);
  analogWrite(speed2, 0);
}