//setup stuff
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "main.h"
#include <string>

extern pros::Controller master;
extern pros::Motor lT;
extern pros::Motor lM;
extern pros::Motor lF;

extern pros::Motor rT;
extern pros::Motor rM;
extern pros::Motor rF;

extern pros::Motor intakeTop;
extern pros::Motor intakeBottom;

extern pros::MotorGroup leftMotors;
extern pros::MotorGroup rightMotors;

extern pros::Imu imu;
extern pros::Distance distBack;
extern pros::Distance distRight;

extern pros::adi::DigitalOut loader;
extern pros::adi::DigitalOut mid;
extern pros::adi::DigitalOut wing;
extern pros::adi::DigitalOut low;

extern pros::adi::Led ledsLeft;
extern pros::adi::Led ledsRight;

extern pros::Optical optical;

extern float lateral_kp;
extern float lateral_ki;
extern float lateral_kd;
extern float angular_kp;
extern float angular_ki;
extern float angular_kd;

extern lemlib::Chassis chassis;

enum class COLOR_STATE
{
    NONE,
    BLUE,
    RED
};

enum class POSITION_STATE
{
    NONE,
    LEFT,
    RIGHT
};

enum class MODE_STATE
{
    NONE,
    SKILLS,
    GAME_A,
    GAME_B,
    GAME_X
};

extern COLOR_STATE selected_color;

enum class PID_STATE
{
    NONE,
    LATERAL,
    ANGULAR
};

extern bool loader_state;
extern bool mid_state;
extern bool wing_state;
extern bool low_state;

extern bool led_enabled;

extern bool in_game;
extern bool in_skills;

#endif