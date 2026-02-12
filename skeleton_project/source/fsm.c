#include "fsm.h"

fsm_t elevator_fsm;

void fsm_dispatch(fsm_events_t event) {
    if (elevator_fsm.state != NULL) {
        elevator_fsm.state(event);
    }
}

void fsm_transition(state_fp new_state) {
    if (elevator_fsm.state != NULL) {
        fsm_dispatch(EVENT_EXIT);
    }
    elevator_fsm.state = new_state;
    fsm_dispatch(EVENT_ENTRY);
}