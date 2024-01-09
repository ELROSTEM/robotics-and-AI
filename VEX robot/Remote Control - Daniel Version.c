#pragma config(Sensor, in3, channel3, sensorNone)
#pragma config(Sensor, in2, channel2, sensorNone)

void circle();
void turn(bool turnLeft, int times);
void forward(int segments);
void intersection();

task main(){
	int drive, turn, leftSpeed, rightSpeed;
	float slowModeMultiplier = .25;

	bool slowMode = false;
	bool slowModeButtonPressed = false;

	// Main loop which repeats
	while(true){
		if (vexRT[Btn8D] == 1) { // If the circle button was pressed
			circle();
		}
		else if (vexRT[Btn8L] == 1) { // Does the intersection track
			intersection();
		}
		else { // Else, uses the remote control
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
			motor[port8] = rightSpeed;
			motor[port5] = leftSpeed * .43;
			motor[port6] = vexRT[Ch2];
			motor[port7] = vexRT(Btn5U)*127 - vexRT(Btn6U)*127;
		}
	}
}

void circle() {
	motor[port8] = -31;
	motor[port5] = 127;
	wait1Msec(100);
	motor[port8] = 0;
	motor[port5] = 0;
}
void turn(bool turnLeft, int times){
	if (turnLeft) {
		motor[port8] = -127;
		motor[port5] = 0;
	} else {
		motor[port8] = 0;
		motor[port5] = 127;
	}
	if (times == 1) {
		wait1Msec(750);
	}
	wait1Msec(530 * times);
	motor[port8] = 0;
	motor[port5] = 0;
}

void forward(int segments) {
	motor[port8] = -127;
	motor[port5] = 127 * .43;
	wait1Msec(700 * segments);
	motor[port8] = 0;
	motor[port5] = 0;
}

void intersection() {
	forward(2);
	wait1Msec(300);
	turn(false, 2);
	wait1Msec(300);
	forward(2);
	wait1Msec(300);
	turn(false, 2);
	wait1Msec(300);
	forward(1);
	wait1Msec(100);
	turn(true, 1);
	wait1Msec(100);
	forward(1);
	wait1Msec(100);
	turn(true, 2);
	wait1Msec(100);
	forward(2);
	wait1Msec(100);
	turn(true, 2);
	wait1Msec(100);
	forward(1);
	wait1Msec(100);
}
