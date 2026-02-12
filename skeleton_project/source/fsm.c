/**
 * @file fsm.c
 * @brief Generic finite state machine implementation.
 *
 * Implements event dispatching and state transitions for the FSM framework.
 */

#include "fsm.h"

/** @brief Global elevator FSM instance. */
fsm_t elevator_fsm;

void fsm_dispatch(fsm_events_t event) {
    if (elevator_fsm.state != NULL) {
        elevator_fsm.state(event);
    }
}

void fsm_transition(p_state_f new_state) {
    if (elevator_fsm.state != NULL) {
        fsm_dispatch(EVENT_EXIT);
    }
    elevator_fsm.state = new_state;
    fsm_dispatch(EVENT_ENTRY);
}
