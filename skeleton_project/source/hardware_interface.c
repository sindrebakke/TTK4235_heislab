/**
 * @file hardware_interface.c
 * @brief Hardware abstraction layer for elevator control.
 *
 * This module provides an interface between the elevator control logic
 * and the low-level hardware driver (elevio). It handles button polling,
 * motor control, sensors, and indicator lights.
 */

#include "elevator_types.h"
#include "driver/elevio.h"
#include <stdbool.h>
#include <stdio.h>

// Forward declarations
void order_manager_add_order(int floor, OrderType type);

/**
 * @brief Initializes the hardware interface.
 *
 * Establishes connection to the elevator hardware/simulator.
 *
 * @return true if initialization succeeded, false otherwise.
 */
bool hardware_interface_init(void) {
    elevio_init();
    return true;
}

/**
 * @brief Polls all elevator buttons and registers orders.
 *
 * Checks cab buttons, hall up buttons, and hall down buttons.
 * When a button press is detected, an order is added via order_manager.
 */
void hardware_interface_poll_buttons(void) {
    // Poll cab buttons
    for (int floor = 0; floor < N_FLOORS; floor++) {
        if (elevio_callButton(floor, BUTTON_CAB)) {
            order_manager_add_order(floor, ORDER_TYPE_CAB);
        }
    }

    // Poll hall up buttons 
    for (int floor = 0; floor < N_FLOORS - 1; floor++) {
        if (elevio_callButton(floor, BUTTON_HALL_UP)) {
            order_manager_add_order(floor, ORDER_TYPE_HALL_UP);
        }
    }

    // Poll hall down buttons
    for (int floor = 1; floor < N_FLOORS; floor++) {
        if (elevio_callButton(floor, BUTTON_HALL_DOWN)) {
            order_manager_add_order(floor, ORDER_TYPE_HALL_DOWN);
        }
    }
}

/**
 * @brief Updates the floor indicator light.
 *
 * @param current_floor The floor to display on the indicator
 */
void hardware_interface_update_lights(int current_floor) {
    if (current_floor >= 0 && current_floor < N_FLOORS) {
        elevio_floorIndicator(current_floor);
    }
}

/**
 * @brief Sets the motor direction.
 *
 * @param direction The desired direction (DIR_UP, DIR_DOWN, or DIR_STOP).
 */
void hardware_interface_set_motor_direction(Direction direction) {
    elevio_motorDirection((MotorDirection)direction);
}

/**
 * @brief Reads the floor sensor.
 *
 * @return The current floor (0 to N_FLOORS-1) if at a floor, -1 if between floors.
 */
int hardware_interface_read_floor_sensor(void) {
    return elevio_floorSensor();
}

/**
 * @brief Reads the stop button state.
 *
 * @return true if the stop button is pressed, false otherwise.
 */
bool hardware_interface_read_stop_button(void) {
    return elevio_stopButton();
}

/**
 * @brief Reads the obstruction sensor.
 *
 * @return true if an obstruction is detected, false otherwise.
 */
bool hardware_interface_read_obstruction(void) {
    return elevio_obstruction();
}

/**
 * @brief Sets the door open indicator light.
 *
 * @param on true to turn the light on, false to turn it off.
 */
void hardware_interface_set_door_light(bool on) {
    elevio_doorOpenLamp(on);
}

/**
 * @brief Sets the stop button indicator light.
 *
 * @param on true to turn the light on, false to turn it off.
 */
void hardware_interface_set_stop_light(bool on) {
    elevio_stopLamp(on);
}
