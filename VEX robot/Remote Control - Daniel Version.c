#pragma config(Sensor, in2,    channel2,       sensorNone)
#pragma config(Sensor, in3,    channel3,       sensorNone)
#pragma config(Sensor, in4,    leftTracker,    sensorReflection)
#pragma config(Sensor, in5,    centerTracker,  sensorLineFollower)
#pragma config(Sensor, in6,    rightTracker,   sensorReflection)
#pragma config(Sensor, dgtl1,  leftSonar,               sensorSONAR_cm)
#pragma config(Sensor, dgtl6,  frontSonar,               sensorSONAR_cm)
#pragma config(Sensor, dgtl11, rightSonar,               sensorSONAR_cm)
#pragma config(Motor,  port5,           leftMotor,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           clawMotor,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           armMotor,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           rightMotor,    tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void circle();
void forward(int motorSpeed, float segments);
void backward(int segments);
void right(float times);
void left(float times);
void intersection();
void lineTracker();
void autoPark();
void parallelPark();
void goAround();

bool parked = false;

task main(){
	int drive, turn, leftSpeed, rightSpeed;
	float slowModeMultiplier = .25;

	bool slowMode = false;
	bool slowModeButtonPressed = false;

	// Main loop which repeats
	while(true){
		if (vexRT[Btn8D] == 1)  // If the circle button was pressed
				circle();
		else if (vexRT[Btn8L] == 1)  // Does the intersection track
				intersection();
		else if (vexRT[Btn8U] == 1)
				lineTracker();
		else if (vexRT[Btn7D] == 1 || true){
			//left(1);
			//wait1Msec(1000);
			//right(1);
			//wait1Msec(1000);
			autoPark();
		}
		else{ // Else, uses the remote control
			// Sets the slow mode boolean the first time the button is pressed
			if (vexRT[Btn8R] == 1 && !slowModeButtonPressed) {
				slowMode = !slowMode;
				slowModeButtonPressed = true;
			}
			else if (vexRT[Btn8R] == 0) { //Once the button is let go, the mode is allowed to be changed again
				slowModeButtonPressed = false;
			}
			// Gets the base values for the speed before modifiers
			drive = vexRT[Ch3];
			turn = vexRT[Ch4];

			// Applies the modifier if it is in slowMode
			if (slowMode) {
				drive *= slowModeMultiplier;
				turn *= slowModeMultiplier;
			}

			// Calculates what speed each motor must be to turn the requested amount
			rightSpeed = drive - turn;
			leftSpeed = drive + turn;

			// Sends the values to the motors
			motor[rightMotor] = rightSpeed;
			motor[leftMotor] = leftSpeed * .43;
			motor[clawMotor] = vexRT[Ch2];
			motor[armMotor] = vexRT(Btn5U)*127 - vexRT(Btn6U)*127;
		}
	}
}

void circle() {
	motor[rightMotor] = -31;
	motor[leftMotor] = 127;
	wait1Msec(100);
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
}

void intersection() {
	forward(127, 2);
	wait1Msec(300);
	right(2);
	wait1Msec(300);
	right(2);
	wait1Msec(300);
	right(2);
	wait1Msec(300);
	forward(127, 1.2);
	wait1Msec(100);
	right(1);
	wait1Msec(100);
	forward(127, 1);
	wait1Msec(100);
	right(2);
	wait1Msec(100);
	forward(127, 2);
	wait1Msec(100);
	right(2);
	wait1Msec(100);
	forward(127, 1);
	wait1Msec(100);
}

void lineTracker(){
	if (SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] > 2800) {
		clearTimer(T1);
		motor[rightMotor] = 80;
		motor[leftMotor] = 60;
		writeDebugStreamLine("\nMoving Forward");
		writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
		writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
		writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
	}
	if ((SensorValue[leftTracker] < 2800 && SensorValue[centerTracker] > 2800 && SensorValue[rightTracker] > 2800) || (SensorValue[leftTracker] < 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] > 2800)){
		clearTimer(T1);
		motor[rightMotor] = 127;
		motor[leftMotor] = -80;
		writeDebugStreamLine("\nTurning Left");
		writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
		writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
		writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
	}
	if ((SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] > 2800 && SensorValue[rightTracker] < 2800) || (SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] < 2800)){
		clearTimer(T1);
		motor[rightMotor] = -50;
		motor[leftMotor] = 90;
		writeDebugStreamLine("\nTurning Right");
		writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
		writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
		writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
	}
	if(SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] > 2800 && SensorValue[rightTracker] > 2800){
		if (time1(T1) >= 1000){
			motor[rightMotor] = 0;
			motor[leftMotor] = 0;
			writeDebugStreamLine("\nTurning Off");
			writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
			writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
			writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
		}
	}
}

void autoPark(){
	bool waitingForObstacle = false;
	bool gapTimerOn = false;
	int minGapTime = 1500;
	clearTimer(T2);
	while (!parked) {
		// If there is a block in front
		if (SensorValue(frontSonar) < 25) {
			writeDebugStream("Obstacle in front\n");
			motor[rightMotor] = 0;
			motor[leftMotor] = 0;
			// If this was the firt iteration that this block was in front, start timer
			if (waitingForObstacle == false) {
				writeDebugStream("Starting timer\n");
				clearTimer(T1);
				waitingForObstacle = true;
				gapTimerOn = false;
			}
			// If this was not the first iteraton, check if it has been 3 seconds since it started
			if (waitingForObstacle) {
				if (time1(T1) > 3000)	{
					writeDebugStream("Going Around\n");
					goAround();
					waitingForObstacle = false;
					clearTimer(T2);
				}
				writeDebugStream("Wait time elapsed: %d\n", time1(T1));
			}
		}
		// If thee is no car in front, check the right sensor
		else if (SensorValue(rightSonar) < 20 && SensorValue(rightSonar) != -1) {
			writeDebugStream("Car to right\n");
			clearTimer(T2);
			forward(60, 0);
		}
		// This else activates if there is no block in front or on the side (free space)
		else {
			writeDebugStream("Gap to right. Current gap time: %d \n", time1(T2));
			if (time1(T2) > minGapTime && parked == false) {
				writeDebugStream("Parking\n");
				parallelPark();
				parked = true;
			}
			else {
				writeDebugStream("Gap too small. Continuing\n");
				forward(60, 0);
			}
		}
	} //writeDebugStream("%d\n",SensorValue(rightTracker);
}

void parallelPark() {
	backward(1);
	right(1.4); // 1
	forward(60, 2.5); // 1.25
	left(1);
}

void goAround() {
	writeDebugStream("Going around\n");
	left(1.1);
	forward(60, 1);
	right(1.1);
	forward(60, 3.5);
	right(1);
	forward(60, 1.4);
	left(1.1);
	writeDebugStream("Done going around\n");
}

void forward(int motorSpeed, float segments) {
	if (segments == 0){
		motor[rightMotor] = motorSpeed;
		motor[leftMotor] = motorSpeed * .7;
	} else {
		motor[rightMotor] = motorSpeed;
		motor[leftMotor] = motorSpeed *.7;
		wait1Msec(1000 * segments);
		motor[rightMotor] = 0;
		motor[leftMotor] = 0;
	}
	writeDebugStream("Moving Forward\n\n");
}

void backward(int segments) {
	motor[rightMotor] = -40;
	motor[leftMotor] = -40;
	wait1Msec(1000*segments);
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
}

void right(float times) {
	clearTimer(T3);
	while (time1(T3) < 1000*times){
		motor[rightMotor] = -55;
		motor[leftMotor] = 55;
	}
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
	writeDebugStream("Turned Right\n\n");
}

void left(float times) {
	clearTimer(T3);
	while (time1(T3) < 1000*times){
		motor[rightMotor] = 60;
		motor[leftMotor] = -60;
	}
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
	writeDebugStream("Turned Left\n\n");
}
