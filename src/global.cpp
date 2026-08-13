#include "global.hpp"
#include "main.h"
#include "config.hpp"
#include <string>
#include <unordered_map>

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::Motor lT(LEFT_TOP_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);
pros::Motor lM(LEFT_MID_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);
pros::Motor lF(LEFT_FRONT_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);

pros::Motor rT(RIGHT_TOP_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);
pros::Motor rM(RIGHT_MID_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);
pros::Motor rF(RIGHT_FRONT_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);

pros::MotorGroup leftMotors({LEFT_TOP_PORT, LEFT_MID_PORT, LEFT_FRONT_PORT});
pros::MotorGroup rightMotors({RIGHT_TOP_PORT, RIGHT_MID_PORT, RIGHT_FRONT_PORT});

pros::Imu imu(IMU_PORT);
pros::Distance distBack(DIST_BACK_PORT);
pros::Distance distRight(DIST_RIGHT_PORT);

bool loader_state = false;
bool mid_state = false;
bool wing_state = false; 
bool low_state = false;

bool led_enabled = false;

COLOR_STATE selected_color = COLOR_STATE::NONE;

pros::adi::DigitalOut loader(LOADER_PORT, loader_state);
pros::adi::DigitalOut mid(MID_MECH_PORT, mid_state);
pros::adi::DigitalOut wing(WING_PORT, wing_state); // high when up
pros::adi::DigitalOut low(LOW_MECH_PORT, low_state);

pros::Motor intakeTop(INTAKE_TOP_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);
pros::Motor intakeBottom(INTAKE_BOTTOM_PORT, pros::MotorGears::blue, pros::MotorEncoderUnits::degrees);

pros::Optical optical(OPTICAL_PORT);

lemlib::Drivetrain drivetrain(
    &leftMotors,                // left motor group
    &rightMotors,               // right motor group
    11.3125,                    // track width (between wheels) TODO: measure this
    lemlib::Omniwheel::NEW_325, // wheel
    360,                        // drivetrain rpm
    8                           // 2 for no traction wheels, 8 for traction wheels
);

float lateral_kp = 8.25; // 7-12 works
float lateral_ki = 0;
float lateral_kd = 17.75; // high works to prevent tipping

lemlib::ControllerSettings lateralController(
    lateral_kp, // kP
    lateral_ki, // kI
    lateral_kd, // kD
    0,          // integral anti windup, 0 to disable
    1,          // 1 small error range, in inches (if within this range for timeout stop)
    100,          // 100 small error range timeout, in milliseconds
    3,          // 3 large error range, in inches (if within this range for timeout stop)
    500,          // 500 large error range timeout, in milliseconds
    7           // maximum acceleration (slew), 0 to disable, 5-15 to prevent tipping
);

float angular_kp = 2.3; // 2 - 2.75 works
float angular_ki = 0;
float angular_kd = 15; // 10-20 works

lemlib::ControllerSettings angularController(
    angular_kp, // kP
    angular_ki, // kI
    angular_kd, // kD
    0,          // integral anti windup, 0 to disable
    1,          // 1 small error range, in degrees
    100,        // 100 small error range timeout, in milliseconds
    3,          // 3 large error range, in degrees
    500,        // 500 large error range timeout, in milliseconds
    0           // maximum acceleration (slew), 0 to disable
);

lemlib::OdomSensors sensors(
    nullptr, // vertical tracking wheel 1
    nullptr, // vertical tracking wheel 2
    nullptr, // horizontal tracking wheel 1
    nullptr, // horizontal tracking wheel 2
    &imu  // imu
);

lemlib::ExpoDriveCurve throttle_curve(
    1,    // deadband
    10,   // min power, tuned 12/12
    1.019 // gain, https://www.desmos.com/calculator/umicbymbnl
);

lemlib::ExpoDriveCurve steer_curve(
    1,    // deadband
    10,   // min power, tuned 12/12
    1.019 // gain, https://www.desmos.com/calculator/umicbymbnl
);

lemlib::Chassis chassis(drivetrain, lateralController, angularController, sensors, &throttle_curve, &steer_curve);

bool in_game = false;
bool in_skills = false;