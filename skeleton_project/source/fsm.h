#ifndef FSM_H
#define FSM_H

#include <stdlib.h>

typedef enum {
    EVENT_TICK,              
    EVENT_ENTRY,             
    EVENT_EXIT,              
    EVENT_ORDER_RECEIVED,   
    EVENT_FLOOR_ARRIVED,     
    EVENT_DOOR_TIMEOUT,      
    EVENT_STOP_PRESSED,  
    EVENT_STOP_RELEASED,     
    EVENT_OBSTRUCTION,       
    EVENT_OBSTRUCTION_CLEAR  
} fsm_events_t;

typedef void (*state_fp)(fsm_events_t event);

typedef struct {
    state_fp state;
} fsm_t;

extern fsm_t elevator_fsm;

void fsm_dispatch(fsm_events_t event);
void fsm_transition(state_fp new_state);

#endif