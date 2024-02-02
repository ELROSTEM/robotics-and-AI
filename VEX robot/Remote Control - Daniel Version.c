#pragma config(Sensor, in2,    channel2,       sensorNone)
#pragma config(Sensor, in3,    channel3,       sensorNone)
#pragma config(Sensor, in4,    leftTracker,    sensorReflection)
#pragma config(Sensor, in5,    centerTracker,  sensorLineFollower)
#pragma config(Sensor, in6,    rightTracker,   sensorReflection)
#pragma config(Motor,  port5,  leftMotor,     tmotorVex269_MC29, openLoop)
#pragma config(Motor,  port6,  clawMotor,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,  armMotor,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,  rightMotor,    tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void circle();
void turn(int times);
void forward(int segments);
void intersection();
void lineTracker();

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
		else if (vexRT[Btn8U] == 1 || true)
				lineTracker();
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
			drive = vexRT[Ch4];
			turn = vexRT[Ch3];

			// Applies the modifier if it is in slowMode
			if (slowMode) {
				drive *= slowModeMultiplier;
				turn *= slowModeMultiplier;
			}

			// Calculates what speed each motor must be to turn the requested amount
			leftSpeed = drive + turn;
			rightSpeed = drive - turn;

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
/*void turn(bool turnLeft, int times){
	if (turnLeft) {
		motor[rightMotor] = -127;
		motor[leftMotor] = 0;
	} else {
		motor[rightMotor] = 0;
		motor[leftMotor] = 127;
	}
	if (times == 1) {
		wait1Msec(750);
	}
	wait1Msec(540 * times);
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
} */

void turn(int times) {
	motor[rightMotor] = -127;
	motor[leftMotor] = -127;
	if (times == 1) {
		wait1Msec(340);
	}
	else if (times == 2) {
		wait1Msec(352 * 2);
	}
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
}

void forward(int segments) {
	motor[rightMotor] = -127;
	motor[leftMotor] = 127 * .45;
	wait1Msec(1000 * segments);
	motor[rightMotor] = 0;
	motor[leftMotor] = 0;
}

void intersection() {
	forward(2);
	wait1Msec(300);
	turn(2);
	wait1Msec(300);
	forward(2);
	wait1Msec(300);
	turn(2);
	wait1Msec(300);
	forward(1.2);
	wait1Msec(100);
	turn(1);
	wait1Msec(100);
	forward(1);
	wait1Msec(100);
	turn(2);
	wait1Msec(100);
	forward(2);
	wait1Msec(100);
	turn(2);
	wait1Msec(100);
	forward(1);
	wait1Msec(100);
}

void lineTracker(){
	bool finished = false;
	while (true){
		if ((SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] > 2800) && finished == false) {
			clearTimer(T1);
			motor[rightMotor] = -60;
			motor[leftMotor] = 60;
			writeDebugStreamLine("\nMoving Forward");
			writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
			writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
			writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
		}
		if ((SensorValue[leftTracker] < 2800 && SensorValue[centerTracker] > 2800 && SensorValue[rightTracker] > 2800) || (SensorValue[leftTracker] < 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] > 2800) && finished == false){
			clearTimer(T1);
			motor[rightMotor] = -80;
			motor[leftMotor] = -80;
			writeDebugStreamLine("\nTurning Left");
			writeDebugStreamLine("leftTracker: %d", SensorValue[leftTracker]);
			writeDebugStreamLine("centerTracker: %d", SensorValue[centerTracker]);
			writeDebugStreamLine("rightTracker: %d", SensorValue[rightTracker]);
		}
		if ((SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] > 2800 && SensorValue[rightTracker] < 2800) || (SensorValue[leftTracker] > 2800 && SensorValue[centerTracker] < 2800 && SensorValue[rightTracker] < 2800) && finished == false){
			clearTimer(T1);
			motor[rightMotor] = 80;
			motor[leftMotor] = 80;
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
				finished = true;
			}
		}
	}
}
