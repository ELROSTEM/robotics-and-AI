#pragma config(Sensor, in3, channel3, sensorNone)
#pragma config(Sensor, in2, channel2, sensorNone)
void circle(word *left, word *right);

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
			drive = vexRT[Ch4];
			turn = vexRT[Ch3];
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

			while (vexRT[Btn8D] == 1){
				circle(&motor[port8], &motor[port5]);
			}
			motor[port8] = leftSpeed;
			motor[port5] = rightSpeed;
			motor[port6] = vexRT(Btn5U)*127 - vexRT(Btn5D)*127;
			motor[port7] = vexRT(Btn6U)*127 - vexRT(Btn6D)*127;
	}
}

void circle(long *left, long *right){
	*left = 127;
	*right = 50;
}
