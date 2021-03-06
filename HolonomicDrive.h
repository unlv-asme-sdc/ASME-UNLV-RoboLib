#ifndef HOLONOMICDRIVE_H
#define HOLONOMICDRIVE_H
#include <Motor.h>
#include <PS2X_lib.h>
#include <Vector_temp.h>
#include <math.h>


/** \brief DriveBase for the Holonomic Drive Chassis.
 *
 * The Holonomic chassis is a chassis(obviously) with omniwheels for forward directions at 135 and 45 degrees.
 * These chassis can move in all directions while turning in place. When turning the drive chassis, the chassis
 * will sacrifice maximum forward force to create a moment about its center. Essentially, the faster you turn,
 * the slower you accelerate.
 */
class HolonomicDrive{
	public:
		HolonomicDrive(const Motor & motor1, const Motor & motor2, const Motor & motor3, const Motor & motor4);
		void drive(const float dir, const float thrust, const float turn);
		void drive(PS2X & ps2x);
		void setTrimMotor(const unsigned char motor, float value);
		void addTrimMotor(const unsigned char motor, float value);
		void reverseMotor(const unsigned char motor, const bool value);
		void reverseLeftMotors(const bool value);
		void reverseRightMotors(const bool value);
	protected:
		void smartConstrain(float &val1, float &val2);

	private:
		// Holders for motors
		Motor* motor1;
		Motor* motor2;
		Motor* motor3;
		Motor* motor4;
		bool reverse1;
		bool reverse2;
		bool reverse3;
		bool reverse4;
};
#endif
