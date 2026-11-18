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

extern pros::Motor intakeBottom; //add cascade variable maybe piston or motor

extern pros::MotorGroup leftMotors;
extern pros::MotorGroup rightMotors;

extern pros::Imu imu;
extern pros::Distance distBack;//used with sensors maybe set up left sensor
extern pros::Distance distRight;

extern pros::Optical optical; //used to detect things ie match lines or score color

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

extern bool in_game;
extern bool in_skills;

#endif