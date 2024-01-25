#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  rightMotorEncoder, sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  leftMotorEncoder, sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           rightMotor,    tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port3,           leftMotor,     tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, I2C_2)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Initialize constants
const float wheelDiameter = 4; // Wheel diameter in inches
const float wheelSeperation = 12; // Distance in CM between left and right wheels
const float cmToInch = 2.54; // Conversion factor from cm to inches
const float cmToEncoderTicks = (627.2 / (wheelDiameter * cmToInch * 3.14159265)); // Calculate encoder ticks per cm
const float EncoderTicksToCm = 1 / cmToEncoderTicks;
const float turnDistance = (3.141592 * wheelSeperation / 4) * cmToEncoderTicks; // Number of ticks required to turn left or right. Multiply with number of degrees


// Iniitalize Functions
void forward(float distance);
void turnRight(int turns);
void turnLeft(int turns);
int calculateTurnEncoderValue(int degrees);
void left (int turns);
void right (int turns);
int calculateDecelerationTime();

task main()
{
	writeDebugStreamLine("Program Started");
  // Reset the encoders
  nMotorEncoder[rightMotor] = 0;
  nMotorEncoder[leftMotor] = 0;

  // Place instructions left
  forward(10);
  left(1);
  forward(50);
  right(1);
  forward(50);
  right(1);
  forward(50);
  left(1);
  forward(50);
  left(1);
  forward(50);
  right(1);
  forward(50);
  right(1);
  forward(50)


	int decelerationTime = calculateDecelerationTime();

	writeDebugStreamLine("Final right: %d. Final left: %d. Deceleration Time: %d",
											nMotorEncoder[rightMotor], nMotorEncoder[leftMotor], decelerationTime);
	writeDebugStreamLine("Final right: %d. Final left: %d. Deceleration Time: %d",
											nMotorEncoder[rightMotor] * EncoderTicksToCm, nMotorEncoder[leftMotor] * EncoderTicksToCm, decelerationTime);
}

// Go forward function
void forward(float distance) {
			writeDebugStreamLine("Moving Forward");

			nMotorEncoder[rightMotor] = 0;
	    nMotorEncoder[leftMotor] = 0;
	    int targetEncoderValue = distance * cmToEncoderTicks - 250; // May want to make a target left / right encoder
			if (distance == 10) {
				targetEncoderValue = targetEncoderValue + 100;
			}
	    //writeDebugStreamLine("Target encoder value: %d", targetEncoderValue);
	    int basePower = 100; // Base power for the motors (original was 63)

	    // PID constants
	    const float Kp = 10; // Proportional gain, adjust as necessary

	    // Correct for drift until it reaches target distance on each wheel
	    clearTimer(T1);
	    while (true) {
	        int leftEncoderValue = getMotorEncoder(leftMotor);
	        int rightEncoderValue = getMotorEncoder(rightMotor);

	        // Calculate the drift error (difference in encoder values)
	        int driftError = rightEncoderValue - leftEncoderValue;

	        // Proportional control for drift correction
	        int correction = Kp * driftError;

	        // Apply correction asymmetrically
	        motor[leftMotor] = basePower - correction; // Decrease speed if left motor is ahead
	        motor[rightMotor] = basePower + correction; // Increase speed if left motor is ahead
	        //writeDebugStreamLine("Right: %d. Left: %d. Difference: %d", rightEncoderValue, leftEncoderValue, driftError);
					writeDebugStreamLine("Right: %d. Left: %d. Difference: %d Correction: %d", rightEncoderValue, leftEncoderValue, driftError, correction);
	        // Exit condition: when the target distance is reached
	        if (abs(leftEncoderValue) >= targetEncoderValue && abs(rightEncoderValue) >= targetEncoderValue) {
	            motor[rightMotor] = 0;
	            motor[leftMotor] = 0;
	            wait1Msec(1000);
	            break;
	        }
	        wait1Msec(10);
	    }
}

// Main right turn function
void turnRight(int turns) {
		writeDebugStreamLine("Turning Right");

    int targetEncoderValue = turnDistance * turns;
    nMotorEncoder[rightMotor] = 0;
    nMotorEncoder[leftMotor] = 0;

    // PID constants
    const float Kp = .5; // Proportional gain, adjust as necessary

    // Correct for drift until it reaches target distance on each wheel
    while (true) {
        int leftEncoderValue = getMotorEncoder(leftMotor);
        int rightEncoderValue = getMotorEncoder(rightMotor);

        // Calculate the drift error (difference in encoder values)
        int driftError = leftEncoderValue + rightEncoderValue; // Adjusted for turning right

        // Proportional control for drift correction
        int correction = Kp * driftError;

        // Apply correction asymmetrically
        motor[leftMotor] = 30 - correction;  // Adjust speed of left motor
        motor[rightMotor] = -30 + correction; // Adjust speed of right motor (note the negative sign)

        // Exit condition: when the target turn is reached
        if (abs(leftEncoderValue) >= targetEncoderValue && abs(rightEncoderValue) >= targetEncoderValue) {
            motor[rightMotor] = 0;
            motor[leftMotor] = 0;
            writeDebugStreamLine("Right: %d. Left: %d. Difference: %d Correction: %d", rightEncoderValue, leftEncoderValue, driftError, correction);
            wait1Msec(1000);
            break;
        }

        wait1Msec(10);
    }
}


void turnLeft(int turns) {
    int targetEncoderValue = turnDistance * turns -10;
    nMotorEncoder[rightMotor] = 0;
    nMotorEncoder[leftMotor] = 0;

    // PID constants
    const float Kp = 0.5; // Proportional gain, adjust as necessary

    // Correct for drift until it reaches target distance on each wheel
    while (true) {
        int leftEncoderValue = getMotorEncoder(leftMotor);
        int rightEncoderValue = getMotorEncoder(rightMotor);

        // Calculate the drift error (difference in encoder values)
        int driftError = leftEncoderValue + rightEncoderValue; // Adjusted for turning right

        // Proportional control for drift correction
        int correction = Kp * driftError;

        // Apply correction asymmetrically
        motor[rightMotor] = 30 - correction;  // Adjust speed of left motor
        motor[leftMotor] = -30 + correction; // Adjust speed of right motor (note the negative sign)

        // Exit condition: when the target turn is reached
        if (abs(rightEncoderValue) >= targetEncoderValue && abs(leftEncoderValue) >= targetEncoderValue) {
            motor[rightMotor] = 0;
            motor[leftMotor] = 0;
            writeDebugStreamLine("Right: %d. Left: %d. Difference: %d Correction: %d", rightEncoderValue, leftEncoderValue, driftError, correction);
            wait1Msec(1000);
            break;
        }

        wait1Msec(10);
    }
}

// Calculates the number of ticks each motor must turn in order to turn in a circle
int calculateTurnEncoderValue(int degrees) {
	const float wheelSeperation = 15.5;
	const float turnDistance = 3.141592 * wheelSeperation / 4;
	return turnDistance * cmToEncoderTicks;
}

// Times how long it takes the robot to decelerate after running a function
int calculateDecelerationTime() {
		clearTimer(T2);
		int initialLeftCount = nMotorEncoder[leftMotor];
		int initialRightCount = nMotorEncoder[rightMotor];
		int threshold = 5;
		bool rightStopped = false;
		bool leftStopped = false;

		while (!leftStopped || !rightStopped) { // While either motor is moving
			wait1Msec(10);

			if (!leftStopped) { // Checks every loop if the left motor stopped
				int currentLeftCount = nMotorEncoder[leftMotor];
				if (abs(currentLeftCount - initialLeftCount) <= threshold) {
					leftStopped = true;
				}
				initialLeftCount = currentLeftCount;
			}
			if (!rightStopped) { // Checks every loop if the right motor stopped
				int currentRightCount = nMotorEncoder[rightMotor];
				if (abs(currentRightCount - initialRightCount) <= threshold) {
					rightStopped = true;
				}
				initialRightCount = currentRightCount;
			}
		}

		int decelerationTime = time1[T2];
		return decelerationTime;
}

// Backup turn left function
void right(int turns) {
		writeDebugStreamLine("Turning Right");

    int targetEncoderValue = turnDistance * turns-30;
    nMotorEncoder[rightMotor] = 0;
    nMotorEncoder[leftMotor] = 0;

    // PID constants

    // Correct for drift until it reaches target distance on each wheel
    while (true) {
        int leftEncoderValue = getMotorEncoder(leftMotor);
        int rightEncoderValue = getMotorEncoder(rightMotor);

        // Calculate the drift error (difference in encoder values)
        int driftError = leftEncoderValue + rightEncoderValue; // Adjusted for turning right

        // Proportional control for drift correction
        int correction = driftError;

        // Apply correction asymmetrically
        motor[leftMotor] = 30 - correction;  // Adjust speed of left motor
        motor[rightMotor] = -30 + correction; // Adjust speed of right motor (note the negative sign)

        // Exit condition: when the target turn is reached
        if (abs(leftEncoderValue) >= targetEncoderValue && abs(rightEncoderValue) >= targetEncoderValue) {
            motor[rightMotor] = 0;
            motor[leftMotor] = 0;
            writeDebugStreamLine("Right: %d. Left: %d. Difference: %d Correction: %d", rightEncoderValue, leftEncoderValue, driftError, correction);
            wait1Msec(1000);
            break;
        }

        wait1Msec(10);
    }
}

void left(int turns) {
		writeDebugStreamLine("Turning Right");

    int targetEncoderValue = turnDistance * turns-5;
    nMotorEncoder[rightMotor] = 0;
    nMotorEncoder[leftMotor] = 0;

    // PID constants

    // Correct for drift until it reaches target distance on each wheel
    while (true) {
        int leftEncoderValue = getMotorEncoder(leftMotor);
        int rightEncoderValue = getMotorEncoder(rightMotor);

        // Calculate the drift error (difference in encoder values)
        int driftError = leftEncoderValue + rightEncoderValue; // Adjusted for turning right

        // Proportional control for drift correction
        int correction = driftError;

        // Apply correction asymmetrically
        motor[rightMotor] = 30 - correction;  // Adjust speed of left motor
        motor[leftMotor] = -30 + correction; // Adjust speed of right motor (note the negative sign)

        // it condition: when the target turn is reached
        if (abs(leftEncoderValue) >= targetEncoderValue && abs(rightEncoderValue) >= targetEncoderValue) {
            motor[rightMotor] = 0;
            motor[leftMotor] = 0;
            writeDebugStreamLine("Right: %d. Left: %d. Difference: %d Correction: %d", rightEncoderValue, leftEncoderValue, driftError, correction);
            wait1Msec(1000);
            break;
        }

        wait1Msec(10);
    }
}
