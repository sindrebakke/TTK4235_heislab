
#ifndef ELEVATOR_FSM_H
#define ELEVATOR_FSM_H

#include "fsm.h"
#include "elevator_types.h"

typedef enum {
    STATE_INIT,
    STATE_IDLE,
    STATE_MOVING_UP,
    STATE_MOVING_DOWN,
    STATE_DOOR_OPEN,
    STATE_EMERGENCY_STOP
} state_id_t;

// Global state tracking
extern state_id_t current_state_id;
extern int current_floor;
extern Direction current_direction;

// Initialization
void elevator_fsm_init(void);

// State functions
void state_init(fsm_events_t event);
void state_idle(fsm_events_t event);
void state_moving_up(fsm_events_t event);
void state_moving_down(fsm_events_t event);
void state_door_open(fsm_events_t event);
void state_emergency_stop(fsm_events_t event);

#endif