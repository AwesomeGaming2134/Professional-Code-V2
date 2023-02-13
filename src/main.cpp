#include "main.h"
#include <fstream>
using namespace std;
using namespace pros;

Controller controls(E_CONTROLLER_MASTER);

Motor indexer(17, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor intake_roller(9);

Motor flywheel1(19, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor flywheel2(20, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);

Motor FrontLeft(1, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor BackLeft(2, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);

Motor FrontRight(3, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor BackRight(4, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);

Motor_Group LeftMotors({1, 2});
Motor_Group RightMotors({3, 4});

void initialize()
{
	lcd::initialize();
	indexer.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	lcd::set_text(1, "Screen working");
	controls.set_text(0, 0, "Controller Working");
	indexer.tare_position();
	indexer.move_absolute(160, 127);
	indexer.tare_position();
}
void disabled()
{
}
void competition_initialize()
{ // there is a hostage
}
void autonomous()
{ // I exist help!
  
}
void opcontrol()
{
	// next time comment out hold, and check if it goes back to 0 when it goes backwards.
	indexer.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake_roller.set_brake_mode(E_MOTOR_BRAKE_HOLD);

	int x1, y1, x2, y2;
	int start_time = int(millis());
	int time_passed = int(millis()) - start_time;
	int intake_timer = int(millis()), indexer_timer = int(millis());
	int left_speeds = 0, right_speeds = 0;
	bool intake_on = false;

	ofstream movement_file;
	movement_file.open("/usd/movements.txt", ios::out);
	lcd::set_text(2, "Hello world");
	delay(400);

	if (!movement_file.is_open())
	{
		lcd::set_text(3, "File didnt open");
	}
	else
	{
		lcd::set_text(3, "EPIC is open!!");
	}

	/*
	FILE* usd_file_write = fopen("/usd/movement.txt", "w");
	fputs("Example text", usd_file_write);
	fclose(usd_file_write);
	*/

	while (true)
	{

		// x2 = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		// y2 = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

		// x1 = controls.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		// y1 = controls.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);
		// left_speeds = y2 - y1 - x1 - x2;
		// right_speeds = y2 - y1 + x1 + x2;
    	// LeftMotors.move(left_speeds);
    	// RightMotors.move(right_speeds);

		/*
		if y2 positive all negtative
		if y1 positive all positive
		if x1 negtative, left negative, right positive
		if x1 positive, left positive, right negative
		if x2 negtative, left negative, right positive
		if x2 positive, left positive, right negative
		*/

		int x = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		int y = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

		if (y == 0 && x == 0){
			LeftMotors.brake();
			RightMotors.brake();
		}
		else if (y != 0 && x == 0) {
			LeftMotors.move(y);
			RightMotors.move(y);
		}
		else if (y == 0 && x != 0) {
			LeftMotors.move(x);
			RightMotors.move(-x);
		}
		else if (y > 0 && x != 0) {
			LeftMotors.move(y + x);
			RightMotors.move(y - x);
		}
		else if (y < 0 && x != 0) {
			LeftMotors.move(y - x);
			RightMotors.move(y + x);
		}

		// movement_file << "L" << left_speeds << "R" << right_speeds;

		if (controls.get_digital(E_CONTROLLER_DIGITAL_R2) && indexer_timer <= int(millis()))
		{
			indexer.move_relative(360, 127);
			indexer_timer = int(millis()) + 690;
			movement_file << "PP";
		}
		else if (int(pros::c::motor_get_position(17)) % 360 != 0 && indexer_timer <= int(millis()))
		{
      int indexer_off = int(pros::c::motor_get_position(17)) % 360;
      if(indexer_off >= 180) {
        indexer.move_relative(360 - indexer_off, 127);
      }
      else
      {
        indexer.move_relative(-indexer_off, 127);
      }
			movement_file << "PN";
		}
		lcd::set_text(2, to_string(int(indexer.get_position()) % 360));

		if (controls.get_digital(E_CONTROLLER_DIGITAL_R1) && intake_timer <= int(millis()) && !intake_on)
		{
			intake_roller = -127;
			intake_on = !intake_on;
			intake_timer = int(millis()) + 500;
			movement_file << "IP";
		}
		else if (controls.get_digital(E_CONTROLLER_DIGITAL_R1) && intake_timer <= int(millis()) && intake_on)
		{
			intake_roller = 0;
			intake_on = !intake_on;
			intake_timer = int(millis()) + 500;
			movement_file << "IN";
		}

		if (controls.get_digital(E_CONTROLLER_DIGITAL_UP))
		{
			intake_roller = 69;
			movement_file << "RP";
		}
		else if (controls.get_digital(E_CONTROLLER_DIGITAL_UP) && !intake_on)
		{
			intake_roller = 0;
			movement_file << "RN";
		}

		if (controls.get_digital(E_CONTROLLER_DIGITAL_Y))
		{
			movement_file.close();
			while (true)
			{
				lcd::set_text(3, "hamburger");
			}
			break;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_L2))
		{
			flywheel1 = 127;
			flywheel2 = 127;
			movement_file << "FP";
		}
		else if (controls.get_digital(E_CONTROLLER_DIGITAL_L1))
		{
			flywheel1 = 0;
			flywheel2 = 0;
			movement_file << "FS";
		}
		movement_file << endl;

		time_passed = int(millis()) - start_time;
		delay(50);
	}
}

/*
Left joystick = flywheel front
Right joystick = intake front
Indexer = Right bumper 2
Intake = Right bumper 1
Flywheel start = Left bumper 1
Flywheel stop = Right bumper 2
*/

// basic idea
//  every loop or 60 milliseconds the program records the controllers button and joystick
//  it can record button a pressed, unpressed, unpressed, pressed
//  then we make a new program which takes the recorded inputs and replays them
//  thus we can get vince to shot and get roller, then replay it for auton
// bonjour