#include "autons.hpp"
#include "global.hpp"
#include "config.hpp"

static int x_m = 1; //used in autons depending on what color and side we're on
static int y_m = 1; 

static constexpr double MM2INCH = 0.0393701; //used in autoskills

//use in moveToPose target headings when declaring 360 degrees
float theta_m(float angle) 

{
    if (x_m == 1 && y_m == -1)
    {
        return 180 - angle;
    }
    else if (x_m == -1 && y_m == 1)
    {
        return -angle;
    }
    else if (x_m == -1 && y_m == -1)
    {
        return -(180 - angle);
    }
    return angle;
}


void run_auto(MODE_STATE mode, COLOR_STATE color, POSITION_STATE position)
{
    // reset mirror multipliers each run
    x_m = 1;
    y_m = 1;

    if (color == COLOR_STATE::BLUE)
        x_m = -1;

    if ((color == COLOR_STATE::BLUE && position == POSITION_STATE::LEFT) ||
        (color == COLOR_STATE::RED && position == POSITION_STATE::RIGHT))
        y_m = -1;

    if (mode == MODE_STATE::GAME_A)
    {
        in_game = true;
        //auto_4ball(); replace these with strats later on
    }
    else if (mode == MODE_STATE::GAME_B)
    {
        in_game = true;
        //auto_7ball();
    }
    else if (mode == MODE_STATE::GAME_X)
    {
        in_game = true;
        //auto_soloawp();
    }
    else if (mode == MODE_STATE::SKILLS)
    {
        auto_skills();
    }
    else
    {
        in_game = true;
        //auto_leavepark();
    }
}


void auto_skills()
{
    int dist_average;
    int new_y;
    int new_x;

    chassis.setPose(-46.5, 15, 90);
    //intakeTop.move(-20); // prevent preload from jamming
    intakeBottom.move(127);

    // get 4 from center
    chassis.moveToPose(-24, 24, 45, 2000, {}, false);

    // score 5 in long goal
    chassis.moveToPose(-24, 24, -45, 600, {.lead = 0}, false);
    chassis.moveToPose(-52, 48, -45, 1000, {.lead = 0, .minSpeed = 40}, false);
    chassis.moveToPose(-52, 48, -90, 600, {.lead = 0, .minSpeed = 40}, false);
    //loader.set_value(HIGH);
    chassis.moveToPose(-20, 48, -90, 1000, {.forwards = false, .lead = 0, .minSpeed = 80}, false);
    chassis.tank(-127, -127, true);
    intakeBottom.move(-127); // get unstuck
    pros::delay(200);
    intakeBottom.move(127);
    //intakeTop.move(127);
    pros::delay(800);
    //intakeTop.move(0);
    chassis.tank(0, 0, true);
    pros::delay(300);
    chassis.setPose(-29, 48, chassis.getPose().theta); // pose reset

    // get top left loader
    chassis.moveToPose(-62, 48, -90, 1000, {.lead = 0, .maxSpeed = 60}, false);
    pros::delay(2000);


    // go to right side along inside
    chassis.moveToPose(-48, 48, -90, 1000, {.forwards = false, .lead = 0}, false);
    //loader.set_value(LOW);
    intakeBottom.move(0);
    chassis.moveToPose(-48, 48, -45, 600, {.lead = 0}, false);
    chassis.moveToPose(-30, 33, -45, 1000, {.forwards = false, .lead = 0}, false);
    chassis.moveToPose(-30, 33, -90, 600, {.lead = 0}, false);
    chassis.moveToPose(45, 33, -90, 1500, {.forwards = false, .lead = 0}, false);

    // score top right
    chassis.moveToPose(45, 33, 180, 600, {.lead = 0, .minSpeed = 40}, false);
    chassis.moveToPose(45, 48, 180, 1000, {.forwards = false, .lead = 0}, false);
    chassis.moveToPose(45, 48, 90, 600, {.lead = 0, .minSpeed = 40}, false);
    //loader.set_value(HIGH);
    chassis.moveToPose(20, 48, 90, 1000, {.forwards = false, .lead = 0}, false);
    chassis.tank(-127, -127, true);
    intakeBottom.move(-127); // get unstuck
    pros::delay(200);
    intakeBottom.move(127);
    //intakeTop.move(127);
    pros::delay(1500);
    //intakeTop.move(0);
    chassis.tank(0, 0, true);
    pros::delay(100);
    chassis.setPose(29, 48, chassis.getPose().theta); // pose reset

    // get top right loader
    //loader.set_value(HIGH);
    chassis.moveToPose(62, 48, 90, 1000, {.lead = 0, .maxSpeed = 60}, false);
    pros::delay(2000);

    // // distance sensor reset for y in case loader fails
    // dist_average = average_distance(distRight, 96 / MM2INCH, 144 / MM2INCH);
    // new_y = (dist_average > 0) ? -72 + dist_average * MM2INCH + DIST_RIGHT_OFFSET : 48; // hardcode fallback
    // chassis.setPose(chassis.getPose().x, new_y, chassis.getPose().theta);

    // score in top right again
    chassis.moveToPose(20, 48, 90, 1000, {.forwards = false, .lead = 0}, false);
    chassis.tank(-127, -127, true);
    intakeBottom.move(-127); // get unstuck
    pros::delay(200);
    intakeBottom.move(127);
    //intakeTop.move(127);
    pros::delay(2000);
    //intakeTop.move(0);
    chassis.tank(0, 0, true);
    pros::delay(100);
    chassis.setPose(29, 48, 90); // pose reset
    //loader.set_value(LOW);

    // go to bottom right
    chassis.moveToPose(40, 48, 90, 1000, {.lead = 0}, false);
    chassis.moveToPose(40, 48, 0, 600, {.lead = 0}, false);
    chassis.moveToPose(40, -48, 0, 3000, {.forwards = false, .lead = 0, .maxSpeed = 80}, false);
    chassis.moveToPose(40, -48, 90, 600, {.lead = 0}, false);

    // get bottom right loader
    //loader.set_value(HIGH);
    chassis.moveToPose(62, -48, 90, 2000, {.lead = 0, .maxSpeed = 60}, false);
    pros::delay(2000);

    // distance sensor reset for y in case loader fails
  
    // aligner reset
    chassis.moveToPose(20, -48, 90, 2000, {.forwards = false, .lead = 0}, false);
    chassis.tank(-127, -127, true);
    pros::delay(500);
    chassis.tank(0, 0, true);
    pros::delay(200);
    chassis.setPose(29, -48, 90); // pose reset
    //loader.set_value(LOW);

    // go to bottom left along inside
    chassis.moveToPose(48, -48, 90, 1500, {.lead = 0}, false);
    chassis.moveToPose(48, -48, 135, 1000, {.lead = 0}, false);
    intakeBottom.move(0);
    chassis.moveToPose(30, -33, 135, 2000, {.forwards = false, .lead = 0}, false);
    chassis.moveToPose(30, -33, 90, 1000, {.lead = 0}, false);
    chassis.moveToPose(-45, -33, 90, 3000, {.forwards = false, .lead = 0}, false);

    // score in bottom left
    chassis.moveToPose(-45, -33, 0, 2000, {.lead = 0, .maxSpeed = 80}, false);
    chassis.moveToPose(-45, -48, 0, 2000, {.forwards = false, .lead = 0, .maxSpeed = 80}, false);
    intakeBottom.move(127);
    chassis.moveToPose(-45, -48, -90, 1000, {.lead = 0, .maxSpeed = 80}, false);
    //loader.set_value(HIGH);
    chassis.moveToPose(-20, -48, -90, 2000, {.forwards = false, .lead = 0}, false);
    chassis.tank(-127, -127, true);
    intakeBottom.move(-127); // get unstuck
    pros::delay(200);
    intakeBottom.move(127);
    //intakeTop.move(127);
    pros::delay(1200);
    //intakeTop.move(0);
    chassis.tank(0, 0, true);
    pros::delay(100);
    chassis.setPose(-29, -48, chassis.getPose().theta); // pose reset

    // get bottom left loader
   // loader.set_value(HIGH);
    chassis.moveToPose(-62, -48, -90, 2000, {.lead = 0, .maxSpeed = 60}, false);
    pros::delay(2000);

    // distance sensor reset for y in case loader fails
    // score in bottom left again
    chassis.moveToPose(-20, -48, -90, 1000, {.forwards = false, .lead = 0}, false);
    chassis.tank(-127, -127, true);
    intakeBottom.move(-127); // get unstuck
    pros::delay(200);
    intakeBottom.move(127);
    //intakeTop.move(127);
    pros::delay(1500);
    //intakeTop.move(0);
    chassis.tank(0, 0, true);
    pros::delay(300);
    chassis.setPose(-29, -48, -90); // pose reset

    // park
    //loader.set_value(LOW);
    chassis.moveToPose(-45, -48, -90, 1000, {.lead = 0}, false);
    chassis.moveToPose(-45, -48, -45, 600, {.lead = 0}, false);
    chassis.moveToPose(-62, -19, -10, 1000, {}, false);
    
}

// Goes forward 1 tile, turns around 180, goes back 1 tile, turns around 180
void auto_test()
{
    chassis.setPose(0, 0, 0);
    chassis.moveToPose(0, 24, 0, 4000, {.lead = 0});
    chassis.moveToPose(0, 24, 180, 2000, {.lead = 0});
    chassis.moveToPose(0, 0, 180, 4000, {.lead = 0});
    chassis.moveToPose(0, 0, 0, 2000, {.lead = 0});
}




