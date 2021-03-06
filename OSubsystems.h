// TODO 2-15
// CHECK PROG USAGE
// Currently Working on PUSHING 3vairables onto stacks

#ifndef OSUBSYSTEMS_H
#define OSUBSYSTEMS_H
#include <TalonSR.h>
#include <PololuG2.h>
#include <HolonomicDrive.h>
#include <HS485.h>
#include <Motor.h>
#define MAX_SEQUENCE 20
#define INTAKE_MOTOR_POWER 1
#define INTAKE_SERVO_ANGLE 2
#define INTAKE_INTAKE 3
#define INTAKE_OUTTAKE 4
#define INTAKE_IDLE 5 
#define SHOOTER_ANGLE 6
#define SHOOTER_POWER 7 
#define CHAMBER_INTAKE 8 
#define CHAMBER_SHOOT 9 
#define CHAMBER_IDLE 10
#define RESET_SUBSYSTEMS 11


/** \brief  2018 SDC Offensive Robot Object.
 *
 * An object to controll Desperado/Renegade subsystems in an a orderly manner. It provides a single sequence stack for multitasking.
 *
 * A sequence stack in this context is a sequence of commands. The commands are called in chronological order with specified delays. Other robot operations and subsystems can act independently while the sequence stack is executing commands. Note: The sequence stack pops commands from the sequence stack when executing them. It does not execute commands by unshifting the stack.
 *
 * For example:
	subsystems.pushSequence(CHAMBER_IDLE,0);
	subsystems.pushSequence(SHOOTER_POWER, 2000, (float)0.0);
	subsystems.pushSequence(CHAMBER_SHOOT, 1500);
	subsystems.pushSequence(SHOOTER_POWER, 0, (float)1.0);
	subsystems.pushSequence(SHOOTER_ANGLE, 0, (float)4645/52);
	subsystems.pushSequence(CHAMBER_IDLE, 0, true);
 * This code will reset shooting subsystems and set shooting angle. After 1.5 seconds, it will shoot the ball. 2 seconds later, it will shut off the shooter motor and reset the chamber.
 * All other subsystems (drive chassis and intake) can still act independtly during this 4 seconds.
 *
 * No additional commands can be added to the sequence stack while the sequence stack is executing commands.
*/
class OSubsystems{
	public:
		OSubsystems(const HolonomicDrive & drive, const Motor & shooter, const HS485 & shooter_servo, const HS485 & chamber, const Motor & intake_motor, const HS485 & intake_servo);
		
		// Sequence Commands
		void pushSequence(byte type, unsigned long delay, bool executeNow = false); 
		void pushSequence(byte type, unsigned long delay, float value, bool executeNow = false);
		void popSequence();
		void executeSequence();
		void resetSubsystems();
		void iterate();
		bool isLocked();
		void resetLocks();

		// Subsystems Command
		void setShooter(float power, bool bypass = false);
		void setShooterAngle(const float angle, bool bypass = false);
		void setChamber(float angle, bool bypass = false);
		void setIntakeAngle(float angle, bool bypass = false);
		void setIntakeRoller(float power, bool bypass = false);
		void setSystemsIntake(bool bypass = false);
		void setSystemsOuttake(bool bypass = false);
		void setSystemsIdle(bool bypass = false);

		// Default Values
		float chamber_intake_pos;
		float chamber_shoot_pos;
		float chamber_idle_pos;
		float intake_idle_pos;
		float intake_intake_pos;
		float intake_roller_in;
		float intake_roller_out;
	protected:
	private:
		// Object Pointers
		HolonomicDrive* chassis;
		Motor* shooter;
		Motor* intake_motor;
		HS485* intake_servo;
		HS485* chamber;
		HS485* shooter_servo;

		// Sequence Locks
		bool sequenceLock;
		bool autolock = true;
		bool chamberLock;
		bool intakeMotorLock;
		bool intakeServoLock;
		bool shooterLock;

		// Sequence Data
		byte sequence_types[MAX_SEQUENCE];
		unsigned long sequence_delays[MAX_SEQUENCE];
		float sequence_values[MAX_SEQUENCE];
		char sequence_index;
		unsigned long last_time;
};
#endif
