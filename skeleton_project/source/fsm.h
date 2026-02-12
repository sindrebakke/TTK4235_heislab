/**
 * @file fsm.h
 * @brief Generic finite state machine framework.
 *
 * Provides a simple state machine infrastructure with support for
 * state transitions and event dispatching.
 */

#ifndef FSM_H
#define FSM_H

#include <stdlib.h>

/**
 * @brief FSM events that can be dispatched to states.
 */
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

/**
 * @brief State function pointer type.
 *
 * Each state is implemented as a function that handles events.
 */
typedef void (*p_state_f)(fsm_events_t event);

/**
 * @brief FSM structure containing current state.
 */
typedef struct {
    p_state_f state;  
} fsm_t;

/** @brief Global elevator FSM instance. */
extern fsm_t elevator_fsm;

/**
 * @brief Dispatches an event to the current state.
 *
 * @param event The event to dispatch.
 */
void fsm_dispatch(fsm_events_t event);

/**
 * @brief Transitions to a new state.
 *
 * Sends EXIT event to current state, changes state, then sends ENTRY event.
 *
 * @param new_state Pointer to the new state function.
 */
void fsm_transition(p_state_f new_state);

#endif
