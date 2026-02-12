#include "elevator_types.h"
#include "driver/elevio.h"
#include <stdbool.h>
#include <stdio.h>

// Forward declarations av order_manager funksjoner
void order_manager_add_order(int floor, OrderType type);

bool hardware_interface_init(void) {
    elevio_init();
    return true;
}

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

void hardware_interface_update_lights(int current_floor) {
    // Update floor indicator
    if (current_floor >= 0 && current_floor < N_FLOORS) {
        elevio_floorIndicator(current_floor);
    }
    
    // Update button lights (simplified - skulle ideelt sett spørre order_manager)
    // Dette må du implementere basert på dine behov
}

void hardware_interface_set_motor_direction(Direction direction) {
    elevio_motorDirection((MotorDirection)direction);
}

int hardware_interface_read_floor_sensor(void) {
    return elevio_floorSensor();
}

bool hardware_interface_read_stop_button(void) {
    return elevio_stopButton();
}

bool hardware_interface_read_obstruction(void) {
    return elevio_obstruction();
}

void hardware_interface_set_door_light(bool on) {
    elevio_doorOpenLamp(on);
}

void hardware_interface_set_stop_light(bool on) {
    elevio_stopLamp(on);
}