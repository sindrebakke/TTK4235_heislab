/**
 * @file elevator_fsm.c
 * @brief Elevator finite state machine implementation.
 *
 * Implements the state machine logic for elevator control including
 * initialization, movement, door control, and emergency stop handling.
 */

#include "elevator_fsm.h"
#include "fsm.h"
#include <stdio.h>

// Order manager forward declarations
void order_manager_clear_orders_at_floor(int floor, Direction direction);
bool order_manager_has_orders(void);
bool order_manager_should_stop(int floor, Direction direction);
Direction order_manager_get_next_direction(int current_floor, Direction current_direction);
void order_manager_clear_all_orders(void);

// Hardware interface forward declarations
void hardware_interface_set_motor_direction(Direction direction);
int hardware_interface_read_floor_sensor(void);

// Door control forward declarations
void door_control_open_door(void);
void door_control_close_door(void);
DoorState door_control_update(void);
void door_control_reset_timer(void);
void door_control_keep_open(void);

/** @brief Current state identifier. */
state_id_t current_state_id = STATE_INIT;

/** @brief Current floor (-1 if between floors). */
int current_floor = -1;

/** @brief Current movement direction. */
Direction current_direction = DIR_STOP;

void elevator_fsm_init(void) {
    elevator_fsm.state = state_init;
    fsm_dispatch(EVENT_ENTRY);
}

void state_init(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_INIT;
            current_floor = hardware_interface_read_floor_sensor();

            if (current_floor != -1) {
                fsm_transition(state_idle);
            } else {
                hardware_interface_set_motor_direction(DIR_DOWN);
            }
            return;

        case EVENT_TICK:
            current_floor = hardware_interface_read_floor_sensor();
            if (current_floor != -1) {
                hardware_interface_set_motor_direction(DIR_STOP);
                fsm_transition(state_idle);
            }
            return;

        case EVENT_EXIT:
            return;

        default:
            return;
    }
}

void state_idle(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_IDLE;
            hardware_interface_set_motor_direction(DIR_STOP);
            current_direction = DIR_STOP;
            return;

        case EVENT_TICK:
            if (order_manager_has_orders()) {
                Direction next_dir = order_manager_get_next_direction(
                    current_floor,
                    current_direction
                );

                if (next_dir == DIR_UP) {
                    fsm_transition(state_moving_up);
                } else if (next_dir == DIR_DOWN) {
                    fsm_transition(state_moving_down);
                } else if (order_manager_should_stop(current_floor, DIR_STOP)) {
                    fsm_transition(state_door_open);
                }
            }
            return;

        case EVENT_STOP_PRESSED:
            fsm_transition(state_emergency_stop);
            return;

        case EVENT_EXIT:
            return;

        default:
            return;
    }
}

void state_moving_up(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_MOVING_UP;
            current_direction = DIR_UP;
            hardware_interface_set_motor_direction(DIR_UP);
            return;

        case EVENT_TICK: {
            int floor = hardware_interface_read_floor_sensor();
            if (floor != -1) {
                current_floor = floor;

                // Stop at top floor regardless of orders
                if (current_floor >= N_FLOORS - 1) {
                    printf("[FSM] Reached top floor %d, stopping\n", current_floor);
                    fsm_transition(state_idle);
                    return;
                }

                if (order_manager_should_stop(current_floor, DIR_UP)) {
                    fsm_transition(state_door_open);
                }
            }
            return;
        }

        case EVENT_STOP_PRESSED:
            fsm_transition(state_emergency_stop);
            return;

        case EVENT_EXIT:
            printf("[FSM] STATE: MOVING_UP -> Exiting\n");
            hardware_interface_set_motor_direction(DIR_STOP);
            return;

        default:
            return;
    }
}

void state_moving_down(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_MOVING_DOWN;
            current_direction = DIR_DOWN;
            hardware_interface_set_motor_direction(DIR_DOWN);
            return;

        case EVENT_TICK: {
            int floor = hardware_interface_read_floor_sensor();
            if (floor != -1) {
                current_floor = floor;

                // Stop at bottom floor regardless of orders
                if (current_floor <= 0) {
                    printf("[FSM] Reached bottom floor %d, stopping\n", current_floor);
                    fsm_transition(state_idle);
                    return;
                }

                if (order_manager_should_stop(current_floor, DIR_DOWN)) {
                    fsm_transition(state_door_open);
                }
            }
            return;
        }

        case EVENT_STOP_PRESSED:
            fsm_transition(state_emergency_stop);
            return;

        case EVENT_EXIT:
            printf("[FSM] STATE: MOVING_DOWN -> Exiting\n");
            hardware_interface_set_motor_direction(DIR_STOP);
            return;

        default:
            return;
    }
}

void state_door_open(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_DOOR_OPEN;
            hardware_interface_set_motor_direction(DIR_STOP);
            order_manager_clear_orders_at_floor(current_floor, current_direction);
            door_control_open_door();
            return;

        case EVENT_TICK: {
            DoorState door_state = door_control_update();
            if (door_state == DOOR_CLOSED) {
                fsm_transition(state_idle);
            }
            return;
        }

        case EVENT_OBSTRUCTION:
            door_control_reset_timer();
            return;

        case EVENT_STOP_PRESSED:
            fsm_transition(state_emergency_stop);
            return;

        case EVENT_EXIT:
            door_control_close_door();
            return;

        default:
            return;
    }
}

void state_emergency_stop(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            current_state_id = STATE_EMERGENCY_STOP;
            hardware_interface_set_motor_direction(DIR_STOP);

            if (current_floor != -1) {
                door_control_open_door();
            }

            order_manager_clear_all_orders();
            return;

        case EVENT_TICK:
            if (current_floor != -1) {
                door_control_keep_open();
            }
            return;

        case EVENT_STOP_RELEASED:
            fsm_transition(state_idle);
            return;

        case EVENT_EXIT:
            if (current_floor != -1) {
                door_control_close_door();
            }
            return;

        default:
            return;
    }
}
