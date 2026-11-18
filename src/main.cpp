#include "main.h"
#include "autons.hpp"
#include "global.hpp"
#include "config.hpp"
using namespace lemlib;
#include <memory>

bool in_selector = false;
POSITION_STATE selected_position = POSITION_STATE::NONE;
MODE_STATE selected_mode = MODE_STATE::NONE;
bool in_pid = false;
PID_STATE selected_pid = PID_STATE::NONE;

std::unique_ptr<pros::Task> comp_init_led_task;
std::unique_ptr<pros::Task> auto_led_task;
std::unique_ptr<pros::Task> driver_led_task;
std::unique_ptr<pros::Task> disabled_led_task;
pros::Task *auto_in_driver_task = nullptr;

lv_obj_t *red_btn;
lv_obj_t *blue_btn;
lv_obj_t *left_btn;
lv_obj_t *right_btn;
lv_obj_t *confirm_btn;
lv_obj_t *a_btn;
lv_obj_t *b_btn;
lv_obj_t *x_btn;
lv_obj_t *skills_btn;


lv_obj_t *temp_var_label;
lv_obj_t *connection_var_label;
lv_obj_t *pose_var_label;
lv_obj_t *battery_var_label;
lv_obj_t *distance_var_label;
lv_timer_t *info_timer = nullptr;

void confirm_screen()
{
    // large auto confirmation screen
    if (info_timer)
    {
        lv_timer_delete(info_timer);
        info_timer = nullptr;
    }
    lv_obj_clean(lv_screen_active());
    temp_var_label = nullptr;
    connection_var_label = nullptr;
    pose_var_label = nullptr;
    battery_var_label = nullptr;
    distance_var_label = nullptr;

    lv_obj_t *auto_label = lv_label_create(lv_screen_active());

    if (selected_color == COLOR_STATE::RED)
    {
        lv_obj_set_style_bg_color(lv_screen_active(), lv_palette_main(LV_PALETTE_RED), 0);
    }
    else if (selected_color == COLOR_STATE::BLUE)
    {
        lv_obj_set_style_bg_color(lv_screen_active(), lv_palette_main(LV_PALETTE_BLUE), 0);
    }
    else // forgot to select color
    {
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    }

    std::string auto_text = "Auto ";

    switch (selected_mode)
    {
    case (MODE_STATE::GAME_A):
        auto_text += "A ";
        break;
    case (MODE_STATE::GAME_B):
        auto_text += "B ";
        break;
    case (MODE_STATE::GAME_X):
        auto_text += "X ";
        break;
    }

    switch (selected_position)
    {
    case (POSITION_STATE::LEFT):
        auto_text += "Left";
        break;
    case (POSITION_STATE::RIGHT):
        auto_text += "Right";
        break;
    }

    if (selected_mode == MODE_STATE::SKILLS)
    {
        auto_text = "Auto Skills";
    }
    else if ((selected_color == COLOR_STATE::NONE) || (selected_mode == MODE_STATE::NONE) || (selected_position == POSITION_STATE::NONE))
    {
        auto_text = "NO AUTO";
    }

    lv_label_set_text(auto_label, auto_text.c_str());
    lv_obj_align(auto_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(auto_label, &lv_font_montserrat_48, 0);

    // blink the color's LED slowly
    int blink_color;
    if (selected_color == COLOR_STATE::RED)
    {
        blink_color = 0xbb0000;
    }
    else if (selected_color == COLOR_STATE::BLUE)
    {
        blink_color = 0x0000bb;
    }
    else
    {
        blink_color = 0x990099; // purple
    }

    if (driver_led_task)
        driver_led_task->notify();
    if (disabled_led_task)
        disabled_led_task->notify();
    if (auto_led_task)
        auto_led_task->notify();
}

static void btn_handler(lv_event_t *e)
{
    lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);

    if (target == red_btn)
    {
        lv_obj_remove_local_style_prop(red_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(blue_btn, LV_OPA_TRANSP, 0);
        selected_color = COLOR_STATE::RED;
    }
    else if (target == blue_btn)
    {
        lv_obj_remove_local_style_prop(blue_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(red_btn, LV_OPA_TRANSP, 0);
        selected_color = COLOR_STATE::BLUE;
    }
    else if (target == left_btn)
    {
        lv_obj_remove_local_style_prop(left_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(right_btn, LV_OPA_TRANSP, 0);
        selected_position = POSITION_STATE::LEFT;
    }
    else if (target == right_btn)
    {
        lv_obj_remove_local_style_prop(right_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(left_btn, LV_OPA_TRANSP, 0);
        selected_position = POSITION_STATE::RIGHT;
    }
    else if (target == a_btn)
    {
        lv_obj_remove_local_style_prop(a_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(b_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(x_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(skills_btn, LV_OPA_TRANSP, 0);
        selected_mode = MODE_STATE::GAME_A;
    }
    else if (target == b_btn)
    {
        lv_obj_remove_local_style_prop(b_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(a_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(x_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(skills_btn, LV_OPA_TRANSP, 0);
        selected_mode = MODE_STATE::GAME_B;
    }
    else if (target == x_btn)
    {
        lv_obj_remove_local_style_prop(x_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(b_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(a_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(skills_btn, LV_OPA_TRANSP, 0);
        selected_mode = MODE_STATE::GAME_X;
    }
    else if (target == skills_btn)
    {
        lv_obj_remove_local_style_prop(skills_btn, LV_STYLE_BG_OPA, 0);
        lv_obj_set_style_bg_opa(a_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(b_btn, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(x_btn, LV_OPA_TRANSP, 0);
        selected_mode = MODE_STATE::SKILLS;
    }
    else if (target == confirm_btn)
    {
        confirm_screen();
    }
}

void screen_update_task(lv_timer_t *timer)
{
    // --- TEMPS ---
    double temps[8] = {lT.get_temperature(), lM.get_temperature(), lF.get_temperature(), rT.get_temperature(), rM.get_temperature(), rF.get_temperature(), intakeBottom.get_temperature()};
    double maxTemp = *std::max_element(temps, temps + 8);

    if (temp_var_label != NULL)
    {
        lv_label_set_text(temp_var_label, std::format("{:.2f}", maxTemp).c_str());
        if (maxTemp > 65)
        {
            lv_obj_set_style_text_color(temp_var_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
        }
        else if (maxTemp > 55)
        {
            lv_obj_set_style_text_color(temp_var_label, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);
        }
        else
        {
            lv_obj_set_style_text_color(temp_var_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
        }
    }

    // --- MOTORS ---
    bool motorsInstalled = lT.is_installed() && lM.is_installed() && lF.is_installed() && rT.is_installed() && rM.is_installed() && rF.is_installed() && intakeBottom.is_installed();
    if (connection_var_label != NULL)
    {
        if (motorsInstalled)
        {
            lv_label_set_text(connection_var_label, "ok");
            lv_obj_set_style_text_color(connection_var_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
        }
        else
        {
            lv_label_set_text(connection_var_label, "FAILURE");
            lv_obj_set_style_text_color(connection_var_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
        }
    }

    // --- POSE ---
    lemlib::Pose pose = chassis.getPose();
    if (pose_var_label != NULL)
    {
        lv_label_set_text(pose_var_label, std::format("({:.1f}, {:.1f}, {:.1f})", pose.x, pose.y, pose.theta).c_str());
    }

    // --- BATTERY ---
    double battery_capacity = pros::battery::get_capacity();
    if (battery_var_label != NULL)
    {
        lv_label_set_text(battery_var_label, std::format("{}%", battery_capacity).c_str());
        if (battery_capacity <= 20)
        {
            lv_obj_set_style_text_color(battery_var_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
        }
        else if (battery_capacity <= 50)
        {
            lv_obj_set_style_text_color(battery_var_label, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);
        }
        else
        {
            lv_obj_set_style_text_color(battery_var_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
        }
    }

    // --- DISTANCE SENSORS ---
    float distX = distRight.get_distance() / 10.0; // cm
    float distY = distBack.get_distance() / 10.0;  // cm
    if (distance_var_label != NULL)
    {
        lv_label_set_text(distance_var_label, std::format("({:.1f}, {:.1f})", distX, distY).c_str());
    }
}

void info_and_selector_screen(std::string mode)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_remove_local_style_prop(lv_screen_active(), LV_STYLE_BG_COLOR, 0);

    info_timer = lv_timer_create(screen_update_task, 100, NULL); // timer for brain info updating

    // info
    lv_obj_t *mode_label = lv_label_create(lv_screen_active());
    lv_label_set_text(mode_label, mode.c_str());
    lv_obj_align(mode_label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *temp_label = lv_label_create(lv_screen_active());
    lv_label_set_text(temp_label, "Temps: ");
    lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 20, 30);

    temp_var_label = lv_label_create(lv_screen_active());
    lv_obj_align(temp_var_label, LV_ALIGN_TOP_LEFT, 90, 30);

    lv_obj_t *connection_label = lv_label_create(lv_screen_active());
    lv_label_set_text(connection_label, "Motors: ");
    lv_obj_align(connection_label, LV_ALIGN_TOP_LEFT, 20, 60);

    connection_var_label = lv_label_create(lv_screen_active());
    lv_obj_align(connection_var_label, LV_ALIGN_TOP_LEFT, 90, 60);

    lv_obj_t *pose_label = lv_label_create(lv_screen_active());
    lv_label_set_text(pose_label, "Pose: ");
    lv_obj_align(pose_label, LV_ALIGN_TOP_LEFT, 20, 90);

    pose_var_label = lv_label_create(lv_screen_active());
    lv_obj_align(pose_var_label, LV_ALIGN_TOP_LEFT, 90, 90);

    lv_obj_t *battery_label = lv_label_create(lv_screen_active());
    lv_label_set_text(battery_label, "Battery: ");
    lv_obj_align(battery_label, LV_ALIGN_TOP_LEFT, 20, 120);

    battery_var_label = lv_label_create(lv_screen_active());
    lv_obj_align(battery_var_label, LV_ALIGN_TOP_LEFT, 90, 120);

    lv_obj_t *distance_label = lv_label_create(lv_screen_active());
    lv_label_set_text(distance_label, "Distance: ");
    lv_obj_align(distance_label, LV_ALIGN_TOP_LEFT, 20, 150);

    distance_var_label = lv_label_create(lv_screen_active());
    lv_obj_align(distance_var_label, LV_ALIGN_TOP_LEFT, 90, 150);

    // auto buttons
    red_btn = lv_button_create(lv_screen_active());
    lv_obj_align(red_btn, LV_ALIGN_TOP_MID, 110, 10);
    lv_obj_set_style_bg_color(red_btn, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(red_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(red_btn, 2, 0);
    lv_obj_set_style_border_color(red_btn, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_add_event_cb(red_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *red_btn_label = lv_label_create(red_btn);
    lv_label_set_text(red_btn_label, "Red");

    blue_btn = lv_button_create(lv_screen_active());
    lv_obj_align(blue_btn, LV_ALIGN_TOP_MID, 190, 10);
    lv_obj_set_style_bg_color(blue_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(blue_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(blue_btn, 2, 0);
    lv_obj_set_style_border_color(blue_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_add_event_cb(blue_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *blue_btn_label = lv_label_create(blue_btn);
    lv_label_set_text(blue_btn_label, "Blue");

    left_btn = lv_button_create(lv_screen_active());
    lv_obj_align(left_btn, LV_ALIGN_TOP_MID, 110, 55);
    lv_obj_set_style_bg_color(left_btn, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(left_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_btn, 2, 0);
    lv_obj_set_style_border_color(left_btn, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_add_event_cb(left_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *left_btn_label = lv_label_create(left_btn);
    lv_label_set_text(left_btn_label, "Left");

    right_btn = lv_button_create(lv_screen_active());
    lv_obj_align(right_btn, LV_ALIGN_TOP_MID, 190, 55);
    lv_obj_set_style_bg_color(right_btn, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(right_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_btn, 2, 0);
    lv_obj_set_style_border_color(right_btn, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_add_event_cb(right_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *right_btn_label = lv_label_create(right_btn);
    lv_label_set_text(right_btn_label, "Right");

    a_btn = lv_button_create(lv_screen_active());
    lv_obj_align(a_btn, LV_ALIGN_TOP_MID, 100, 100);
    lv_obj_set_style_bg_color(a_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_bg_opa(a_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(a_btn, 2, 0);
    lv_obj_set_style_border_color(a_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_add_event_cb(a_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *a_btn_label = lv_label_create(a_btn);
    lv_label_set_text(a_btn_label, "A");

    b_btn = lv_button_create(lv_screen_active());
    lv_obj_align(b_btn, LV_ALIGN_TOP_MID, 150, 100);
    lv_obj_set_style_bg_color(b_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_bg_opa(b_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(b_btn, 2, 0);
    lv_obj_set_style_border_color(b_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_add_event_cb(b_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *b_btn_label = lv_label_create(b_btn);
    lv_label_set_text(b_btn_label, "B");

    x_btn = lv_button_create(lv_screen_active());
    lv_obj_align(x_btn, LV_ALIGN_TOP_MID, 200, 100);
    lv_obj_set_style_bg_color(x_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_bg_opa(x_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(x_btn, 2, 0);
    lv_obj_set_style_border_color(x_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_add_event_cb(x_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *x_btn_label = lv_label_create(x_btn);
    lv_label_set_text(x_btn_label, "X");

    skills_btn = lv_button_create(lv_screen_active());
    lv_obj_align(skills_btn, LV_ALIGN_TOP_MID, 150, 145);
    lv_obj_set_style_bg_color(skills_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_bg_opa(skills_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(skills_btn, 2, 0);
    lv_obj_set_style_border_color(skills_btn, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_add_event_cb(skills_btn, btn_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *skills_btn_label = lv_label_create(skills_btn);
    lv_label_set_text(skills_btn_label, "Skills");

    confirm_btn = lv_button_create(lv_screen_active());
    lv_obj_align(confirm_btn, LV_ALIGN_TOP_MID, 150, 195);
    lv_obj_set_style_bg_color(confirm_btn, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_add_event_cb(confirm_btn, btn_handler, LV_EVENT_LONG_PRESSED, NULL);

    lv_obj_t *confirm_btn_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_btn_label, "Confirm");
}

// ################# COMPETITION FUNCTIONS #################

void initialize()
{
    pros::delay(500); // all 3 wire ports to configure

    chassis.calibrate();
    pros::delay(2000); // allow IMU to calibrate
    chassis.setPose(0, 0, 0);

}

// runs after initialize and before comp switch / field control, e.g. auto selector on brain
// controller does not work
// with comp switch this will run immediately comp switch is in disabled when prog runs
// can use this to detect if we are in game
void competition_initialize()
{
    info_and_selector_screen("Comp Init");
}

void autonomous()
{
    if (comp_init_led_task)
        comp_init_led_task->notify();
    if (driver_led_task)
        driver_led_task->notify();
    if (disabled_led_task)
        disabled_led_task->notify();

    run_auto(selected_mode, selected_color, selected_position);
}

void opcontrol()
{
    info_and_selector_screen("Driver");

    // end auto led task and start driver task
    if (auto_led_task)
        auto_led_task->notify();
    if (comp_init_led_task)
        comp_init_led_task->notify();
    if (disabled_led_task)
        disabled_led_task->notify();

    while (true)
    {
        // auto selector: press up and left simultaneously
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) && master.get_digital(pros::E_CONTROLLER_DIGITAL_UP))
        {
            in_selector = true;

            // reset selections
            selected_mode = MODE_STATE::NONE;
            selected_color = COLOR_STATE::NONE;
            selected_position = POSITION_STATE::NONE;
            in_game = false;
            in_skills = false;
        }

        while (in_selector)
        {
            // select mode
            master.clear_line(1);
            pros::delay(100);
            master.set_text(1, 1, "Mode (A/B/X/Y=Sk)");

            while (selected_mode == MODE_STATE::NONE)
            {
                if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A))
                {
                    selected_mode = MODE_STATE::GAME_A;
                    lv_obj_send_event(a_btn, LV_EVENT_CLICKED, NULL);
                }
                else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B))
                {
                    selected_mode = MODE_STATE::GAME_B;
                    lv_obj_send_event(b_btn, LV_EVENT_CLICKED, NULL);
                }
                else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X))
                {
                    selected_mode = MODE_STATE::GAME_X;
                    lv_obj_send_event(x_btn, LV_EVENT_CLICKED, NULL);
                }
                else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y))
                {
                    selected_mode = MODE_STATE::SKILLS;
                    lv_obj_send_event(skills_btn, LV_EVENT_CLICKED, NULL);
                }

                pros::delay(20);
            }

            if (selected_mode != MODE_STATE::SKILLS)
            {
                // select color
                master.clear_line(1);
                pros::delay(100);
                master.set_text(1, 3, "Red <- | -> Blue");

                while (selected_color == COLOR_STATE::NONE)
                {
                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT))
                    {
                        selected_color = COLOR_STATE::RED;
                        lv_obj_send_event(red_btn, LV_EVENT_CLICKED, NULL);
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT))
                    {
                        selected_color = COLOR_STATE::BLUE;
                        lv_obj_send_event(blue_btn, LV_EVENT_CLICKED, NULL);
                    }

                    pros::delay(20);
                }

                // select starting position
                master.clear_line(1);
                pros::delay(100);
                master.set_text(1, 3, "Left <- | -> Right");

                while (selected_position == POSITION_STATE::NONE)
                {
                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT))
                    {
                        selected_position = POSITION_STATE::LEFT;
                        lv_obj_send_event(left_btn, LV_EVENT_CLICKED, NULL);
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT))
                    {
                        selected_position = POSITION_STATE::RIGHT;
                        lv_obj_send_event(right_btn, LV_EVENT_CLICKED, NULL);
                    }

                    pros::delay(20);
                }
            }

            // print selected auto
            std::string mode_label = "";
            std::string color_label = "";
            std::string position_label = "";

            switch (selected_mode)
            {
            case MODE_STATE::GAME_A:
                mode_label = " A";
                break;
            case MODE_STATE::GAME_B:
                mode_label = " B";
                break;
            case MODE_STATE::GAME_X:
                mode_label = " X";
                break;
            case MODE_STATE::SKILLS:
                mode_label = "Skills";
                break;
            }

            switch (selected_color)
            {
            case COLOR_STATE::RED:
                color_label = "Red";
                break;
            case COLOR_STATE::BLUE:
                color_label = "Blue";
                break;
            }

            switch (selected_position)
            {
            case POSITION_STATE::LEFT:
                position_label = "Left";
                break;
            case POSITION_STATE::RIGHT:
                position_label = "Right";
                break;
            }

            std::string auto_label = "Auto: " + color_label + " " + position_label + mode_label;

            master.clear_line(1);
            pros::delay(100);
            master.set_text(1, 2, auto_label);

            confirm_screen();

            // reset everything
            in_selector = false;
        }

        // Run auto in driver, press left and A to start, move stick to override
        if (selected_mode == MODE_STATE::SKILLS && master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) && master.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        {
            if (auto_in_driver_task == nullptr)
            {
                auto_in_driver_task = new pros::Task(auto_skills);
            }
        }

        // end task if its done
        if (auto_in_driver_task != nullptr && auto_in_driver_task->get_state() == pros::E_TASK_STATE_DELETED)
        {
            delete auto_in_driver_task;
            auto_in_driver_task = nullptr;
        }

        bool stick_moved = (abs(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) > 40 ||
                            abs(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 40 ||
                            abs(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)) > 40 ||
                            abs(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)) > 40);

        // Cancel skills if stick moved
        if (auto_in_driver_task != nullptr && stick_moved)
        {
            auto_in_driver_task->remove();
            chassis.cancelAllMotions();
            delete auto_in_driver_task;
            auto_in_driver_task = nullptr;
        }

        // PID Tuning utility - press left and right simultaneously
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X) && master.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        {
            in_pid = true;
        }

        while (in_pid)
        {
            pros::screen::erase();
            pros::screen::print(TEXT_LARGE_CENTER, 1, "Tuning PID");

            // select pid mode
            master.clear_line(1);
            pros::delay(100);
            master.set_text(1, 3, "Lat (Y)  |  Ang (A)");

            while (selected_pid == PID_STATE::NONE)
            {
                if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y))
                {
                    selected_pid = PID_STATE::LATERAL;
                }
                else if (master.get_digital_new_release(pros::E_CONTROLLER_DIGITAL_A))
                {
                    selected_pid = PID_STATE::ANGULAR;
                }

                pros::delay(20);
            }

            // tune lateral pid
            if (selected_pid == PID_STATE::LATERAL)
            {
                // press left and right again to leave
                while (!master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) || !master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT))
                {
                    pros::screen::erase();
                    std::string p_label = "P:" + std::to_string(lateral_kp);
                    std::string d_label = "D:" + std::to_string(lateral_kd);
                    pros::screen::print(TEXT_LARGE_CENTER, 1, "Lateral PID");
                    pros::screen::print(TEXT_LARGE_CENTER, 3, p_label.c_str());
                    pros::screen::print(TEXT_LARGE_CENTER, 5, d_label.c_str());

                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
                    {
                        lateral_kp += 0.25;
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1))
                    {
                        lateral_kp -= 0.25;
                    }

                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2))
                    {
                        lateral_kd += 0.25;
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1))
                    {
                        lateral_kd -= 0.25;
                    }

                    if (lateral_kp < 0)
                    {
                        lateral_kp = 0;
                    }

                    if (lateral_kd < 0)
                    {
                        lateral_kd = 0;
                    }

                    // drive forward and back a tile
                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X))
                    {
                        chassis.lateralPID.kP = lateral_kp;
                        chassis.lateralPID.kD = lateral_kd;
                        chassis.setPose(0, 0, 0);
                        chassis.moveToPose(0, 24, 0, 8000, {.lead = 0}); // lead to make straight, movetopoint doesn't keep straight
                        chassis.moveToPose(0, 0, 0, 8000, {.forwards = false, .lead = 0});
                    }

                    pros::delay(20);
                }
            }

            // tune angular pid
            else if (selected_pid == PID_STATE::ANGULAR)
            {
                // press left and right again to leave
                while (!master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) || !master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT))
                {
                    pros::screen::erase();
                    std::string p_label = "P:" + std::to_string(angular_kp);
                    std::string d_label = "D:" + std::to_string(angular_kd);
                    pros::screen::print(TEXT_LARGE_CENTER, 1, "Angular PID");
                    pros::screen::print(TEXT_LARGE_CENTER, 3, p_label.c_str());
                    pros::screen::print(TEXT_LARGE_CENTER, 5, d_label.c_str());

                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
                    {
                        angular_kp += 0.25;
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1))
                    {
                        angular_kp -= 0.25;
                    }

                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2))
                    {
                        angular_kd += 0.25;
                    }
                    else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1))
                    {
                        angular_kd -= 0.25;
                    }

                    if (angular_kp < 0)
                    {
                        angular_kp = 0;
                    }

                    if (angular_kd < 0)
                    {
                        angular_kd = 0;
                    }

                    // turn in a circle
                    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X))
                    {
                        chassis.angularPID.kP = angular_kp;
                        chassis.angularPID.kD = angular_kd;
                        chassis.setPose(0, 0, 0);
                        chassis.turnToHeading(90, 4000);
                        chassis.turnToHeading(180, 4000);
                        chassis.turnToHeading(270, 4000);
                        chassis.turnToHeading(0, 4000);
                    }

                    pros::delay(20);
                }
            }

            in_pid = false;
            selected_pid = PID_STATE::NONE;
        }

        // drive
        if (auto_in_driver_task == nullptr)
        {
            int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
            int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

            chassis.arcade(leftY, rightX, false);
        }

        // intake
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
        {
            intakeBottom.move(127);
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
        {
            intakeBottom.move(-127);
        }
        else
        {
            intakeBottom.move(0);
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
        {
            //intakeTop.move(127);
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
        {
            //intakeTop.move(-127);
        }
        else
        {
            //intakeTop.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            //intakeTop.move_velocity(0); // brake to prevent balls from coming out
        }

        pros::delay(20); // don't break task scheduler
    }
}

// after game ends
void disabled()
{
    info_and_selector_screen("Disabled");

    // led
    if (comp_init_led_task)
        comp_init_led_task->notify();
    if (auto_led_task)
        auto_led_task->notify();
    if (driver_led_task)
        driver_led_task->notify();

    // driver skills (disabled after selecting skills in driver)
    if (selected_mode == MODE_STATE::SKILLS)
        in_skills = true;
}
