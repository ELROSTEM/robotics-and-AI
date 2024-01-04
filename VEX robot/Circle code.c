#pragma config(Sensor, in3, channel3, sensorNone)
#pragma config(Sensor, in2, channel2, sensorNone)

task main(){
	int drive;
	int turn;
	int leftSpeed;
	int rightSpeed;
	float slowModeMultiplier = .25;
	float fastModeMultipler = 4.0;
	int change = 0;
	while(true){
			if (vexRT[Btn8R] == 1) {
				change = 1;
				drive *= slowModeMultiplier;
				turn *= slowModeMultiplier;
			}
			else if (vexRT[Btn8R] == 1 && change == 1) {
				change = 0;
				drive *= fastModeMultipler;
				turn *= fastModeMultipler;
			}

			leftSpeed = drive - turn;
			rightSpeed = drive + turn;

			motor[port8] = leftSpeed;
			motor[port5] = rightSpeed;
			motor[port6] = vexRT(btn5U)*127 - vexRT(btn5D)*127;
			motor[port7] = vexRT(btn6U)*127 - vexRT(btn6D)*127;
	}
}
