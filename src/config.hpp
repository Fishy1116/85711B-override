//add in constants for motors, pistons etc

#ifndef CONFIG_HPP
#define CONFIG_HPP

const int LEFT_TOP_PORT = 11;
const int LEFT_MID_PORT = -13;
const int LEFT_FRONT_PORT = -14; 

const int RIGHT_TOP_PORT = -20;
const int RIGHT_MID_PORT = 18;
const int RIGHT_FRONT_PORT = 19;

const int INTAKE_BOTTOM_PORT = 1;
const int INTAKE_TOP_PORT = 21;

const int IMU_PORT = 2;

const int DIST_BACK_PORT = 15;
const int DIST_RIGHT_PORT = 17;
const float DIST_BACK_OFFSET = 4; // inches to bot center crossbrace
const float DIST_RIGHT_OFFSET = 4.5; // inches to bot center

const int OPTICAL_PORT = 5;

const char LOADER_PORT = 'c';
const char MID_MECH_PORT = 'f';
const char WING_PORT = 'g';
const char LOW_MECH_PORT = 'h';

#endif