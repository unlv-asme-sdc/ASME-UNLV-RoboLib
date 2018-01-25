#ifndef LSMHEADLESS.H
#define LSMHEADLESS.H
#include <Wire.h>
#include <LSM6.h>
#include <Arduino.h>
class LSMHeadless{
	public:
		void init();
		void zero();
		void iterate();
		void calibrate();
		float getAbsYaw();
		float getRelativeYaw();
	protected:
	private:
		unsigned long calibration_time;
		unsigned long last_calibration;
		float abs_yaw;
		float relative_yaw;
		unsigned long prev_time;
		int16_t bias;
		LSM6 lsm6;
};

#endif
