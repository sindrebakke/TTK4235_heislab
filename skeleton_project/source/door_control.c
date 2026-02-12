#include "elevator_types.h"
#include <stdbool.h>
#include <time.h>

// Forward declaration
void hardware_interface_set_door_light(bool on);

static DoorState door_state = DOOR_CLOSED;
static time_t door_open_time = 0;
static bool keep_open = false;

#define DOOR_OPEN_DURATION 3

void door_control_init(void) {
    door_state = DOOR_CLOSED;
    door_open_time = 0;
    keep_open = false;
    hardware_interface_set_door_light(false);
}

void door_control_open_door(void) {
    door_state = DOOR_OPEN;
    door_open_time = time(NULL);
    keep_open = false;
    hardware_interface_set_door_light(true);
}

void door_control_close_door(void) {
    door_state = DOOR_CLOSED;
    keep_open = false;
    hardware_interface_set_door_light(false);
}

void door_control_reset_timer(void) {
    door_open_time = time(NULL);
}

void door_control_keep_open(void) {
    keep_open = true;
}

DoorState door_control_update(void) {
    if (door_state == DOOR_OPEN && !keep_open) {
        time_t now = time(NULL);
        if (difftime(now, door_open_time) >= DOOR_OPEN_DURATION) {
            return DOOR_CLOSED;
        }
    }
    return door_state;
}
