/*
* line_follow_pid.c
*
* Created: 11/29/2015 2:05:12 PM
*  Author: nss
*/
#include <pololu/3pi.h>

int line_follow_pid(int* sensors, unsigned int position, unsigned int last_proportional, _Bool ignore_outer_sensor) {
	long integral=0;

	// The "proportional" term should be 0 when we are on the line.
	int proportional = ((int)position) - 2000;

	// Compute the derivative (change) and integral (sum) of the
	// position.
	int derivative = proportional - last_proportional;
	integral += proportional;

	// Remember the last position.
	last_proportional = proportional;

	// Compute the difference between the two motor power settings,
	// m1 - m2.  If this is a positive number the robot will turn
	// to the right.  If it is a negative number, the robot will
	// turn to the left, and the magnitude of the number determines
	// the sharpness of the turn.
	int power_difference = proportional/20 + integral/10000 + derivative*3/2;


	const double speed_mult = 2.5;
	// Compute the actual motor settings.  We never set either motor
	// to a negative value.
	int max = 50;
	
	//
	if(sensors[1] > 900 && sensors[2] > 900 && sensors[3] > 900) {
		max = max*speed_mult;
	}

	if(power_difference > max)
	power_difference = max;
	if(power_difference < -max)
	power_difference = -max;

	if(power_difference < 0)
	set_motors(max+power_difference, max);
	else
	set_motors(max, max-power_difference);

	return proportional;
}