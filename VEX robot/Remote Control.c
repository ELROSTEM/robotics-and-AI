#pragma config(Sensor, in3, channel3, sensorNone)
#pragma config(Sensor, in2, channel2, sensorNone)

task main(){
	int drive, turn, leftSpeed, rightSpeed;
	float slowModeMultiplier = .25;
	float fastModeMultipler = 4.0;
	bool slowMode = false;
	bool slowModeButtonPressed = false;

	// Main loop which repeats
	while(true){
		if (vexRT[Btn8D] == 1) { // If the circle button was pressed
			leftSpeed = 127
			rightSpeed = -50
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
			leftSpeed = drive - turn;
			rightSpeed = drive + turn;
		}
		// Sends the values to the motors
		motor[port8] = leftSpeed;
		motor[port5] = rightSpeed;
		motor[port6] = vexRT(Btn5U)*127 - vexRT(Btn6U)*127;
		motor[port7] = vexRT[Ch1];
	}
}
