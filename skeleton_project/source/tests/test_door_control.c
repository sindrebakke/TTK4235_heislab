/**
 * @file test_door_control.c
 * @brief Unit tests for door_control module.
 */

#include <stdio.h>
#include <stdbool.h>
#include "../elevator_types.h"

// Stub for hardware interface (door_control calls this)
void hardware_interface_set_door_light(bool on) {
    (void)on;
}

// Door control functions
extern void door_control_init(void);
extern void door_control_open_door(void);
extern void door_control_close_door(void);
extern void door_control_reset_timer(void);
extern void door_control_keep_open(void);
extern DoorState door_control_update(void);

static int test_init(void) {
    door_control_init();
    return door_control_update() == DOOR_CLOSED;
}

static int test_open_door(void) {
    door_control_init();
    door_control_open_door();
    return door_control_update() == DOOR_OPEN;
}

static int test_close_door(void) {
    door_control_init();
    door_control_open_door();
    door_control_close_door();
    return door_control_update() == DOOR_CLOSED;
}

static int test_keep_open(void) {
    door_control_init();
    door_control_open_door();
    door_control_keep_open();
    // Even after timeout, door should stay open
    return door_control_update() == DOOR_OPEN;
}

int run_door_control_tests(void) {
    int failed = 0;

    printf("Door Control:\n");

    if (!test_init()) { printf("  FAIL: test_init\n"); failed++; }
    if (!test_open_door()) { printf("  FAIL: test_open_door\n"); failed++; }
    if (!test_close_door()) { printf("  FAIL: test_close_door\n"); failed++; }
    if (!test_keep_open()) { printf("  FAIL: test_keep_open\n"); failed++; }

    if (failed == 0) {
        printf("  All tests passed\n");
    }

    return failed;
}
