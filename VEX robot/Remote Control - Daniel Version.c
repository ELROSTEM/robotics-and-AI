#pragma config(Sensor, in3, channel3, sensorNone)
#pragma config(Sensor, in2, channel2, sensorNone)

void circle();
void turn(bool turnLeft, int times);
void forward(int segments);

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
		else if (vexRT[Btn8R] == 1) { // Does the intersection track
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
			motor[port5] = leftSpeed * .95;
			motor[port6] = vexRT[Ch2];
			motor[port7] = vexRT(Btn5U)*127 - vexRT(Btn6U)*127;
		}
	}
}

void circle() {
	motor[port8] = -20;
	motor[port5] = 100;
	wait1Msec(100);
}

void turn(bool turnLeft, int times){
	if (turnLeft) {
		motor[port8] = -127;
		motor[port5] = 0;
		wait1Msec(250 * times);
	} else {
		motor[port8] = 0;
		motor[port5] = 127;
		wait1Msec(250 * times);
	}
}

void forward(int segments) {
	motor[port8] = -127;
	motor[port5] = 127 * .95;
	wait1Msec(500 * segments);
}

void intersection() {
	forward(2);
	turn(false, 2);
	forward(2);
	turn(false, 2);
	forward(1);
	turn(true, 1);
	forward(1);
	turn(true, 2);
	forward(2);
	turn(true, 2);
	forward(1);
}
