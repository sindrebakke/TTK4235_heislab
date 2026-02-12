/**
 * @file elevator_fsm.h
 * @brief Elevator finite state machine definitions.
 *
 * Defines the states and state functions for the elevator control system.
 * The FSM handles initialization, idle, movement, door control, and emergency stop.
 */

#ifndef ELEVATOR_FSM_H
#define ELEVATOR_FSM_H

#include "fsm.h"
#include "elevator_types.h"

/**
 * @brief Enumeration of elevator states.
 */
typedef enum {
    STATE_INIT,           
    STATE_IDLE,           
    STATE_MOVING_UP,     
    STATE_MOVING_DOWN,    
    STATE_DOOR_OPEN,     
    STATE_EMERGENCY_STOP 
} state_id_t;

/** @brief Current state identifier for external access. */
extern state_id_t current_state_id;

/** @brief Current floor position (-1 if between floors). */
extern int current_floor;

/** @brief Current movement direction. */
extern Direction current_direction;

/**
 * @brief Initializes the elevator FSM.
 *
 * Sets the initial state and dispatches the entry event.
 */
void elevator_fsm_init(void);

/**
 * @brief Initial state handler.
 *
 * Moves elevator down until a valid floor is reached.
 *
 * @param event The event to process.
 */
void state_init(fsm_events_t event);

/**
 * @brief Idle state handler.
 *
 * Waits for orders and transitions to movement or door open states.
 *
 * @param event The event to process.
 */
void state_idle(fsm_events_t event);

/**
 * @brief Moving up state handler.
 *
 * Controls upward movement and stops at floors with orders.
 *
 * @param event The event to process.
 */
void state_moving_up(fsm_events_t event);

/**
 * @brief Moving down state handler.
 *
 * Controls downward movement and stops at floors with orders.
 *
 * @param event The event to process.
 */
void state_moving_down(fsm_events_t event);

/**
 * @brief Door open state handler.
 *
 * Manages door timing and transitions back to idle when done.
 *
 * @param event The event to process.
 */
void state_door_open(fsm_events_t event);

/**
 * @brief Emergency stop state handler.
 *
 * Stops the elevator, clears orders, and opens door if at a floor.
 *
 * @param event The event to process.
 */
void state_emergency_stop(fsm_events_t event);

#endif
