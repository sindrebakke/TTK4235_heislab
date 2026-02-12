/**
 * @file door_control.c
 * @brief Door control module for the elevator.
 *
 * Manages door state, timing, and the door open indicator light.
 * The door stays open for a configurable duration before closing.
 */

#include "elevator_types.h"
#include <stdbool.h>
#include <time.h>

// Forward declaration
void hardware_interface_set_door_light(bool on);

/** @brief Current door state. */
static DoorState door_state = DOOR_CLOSED;

/** @brief Timestamp when door was opened. */
static time_t door_open_time = 0;

/** @brief Flag to keep door open indefinitely (emergency stop). */
static bool keep_open = false;

/** @brief Duration in seconds the door stays open. */
#define DOOR_OPEN_DURATION 3

/**
 * @brief Initializes the door control module.
 *
 * Sets door to closed state and turns off the door light.
 */
void door_control_init(void) {
    door_state = DOOR_CLOSED;
    door_open_time = 0;
    keep_open = false;
    hardware_interface_set_door_light(false);
}

/**
 * @brief Opens the door.
 *
 * Sets state to open, starts the timer, and turns on the door light.
 */
void door_control_open_door(void) {
    door_state = DOOR_OPEN;
    door_open_time = time(NULL);
    keep_open = false;
    hardware_interface_set_door_light(true);
}

/**
 * @brief Closes the door.
 *
 * Sets state to closed and turns off the door light.
 */
void door_control_close_door(void) {
    door_state = DOOR_CLOSED;
    keep_open = false;
    hardware_interface_set_door_light(false);
}

/**
 * @brief Resets the door open timer.
 *
 * Called when obstruction is detected to extend door open time.
 */
void door_control_reset_timer(void) {
    door_open_time = time(NULL);
}

/**
 * @brief Keeps the door open indefinitely.
 *
 * Used during emergency stop to prevent door from closing.
 */
void door_control_keep_open(void) {
    keep_open = true;
}

/**
 * @brief Updates door state based on timer.
 *
 * Checks if the door open duration has elapsed.
 *
 * @return DOOR_CLOSED if timer expired, otherwise current door state.
 */
DoorState door_control_update(void) {
    if (door_state == DOOR_OPEN && !keep_open) {
        time_t now = time(NULL);
        if (difftime(now, door_open_time) >= DOOR_OPEN_DURATION) {
            return DOOR_CLOSED;
        }
    }
    return door_state;
}
