
#include "elevator_types.h"
#include <stdbool.h>
#include <stdio.h>

static bool cab_orders[N_FLOORS];
static bool hall_up_orders[N_FLOORS - 1];
static bool hall_down_orders[N_FLOORS - 1];


void order_manager_init(void) {
    for (int i = 0; i < N_FLOORS; i++) {
        cab_orders[i] = false;
    }
    for (int i = 0; i < N_FLOORS - 1; i++) {
        hall_up_orders[i] = false;
        hall_down_orders[i] = false;
    }
}

void order_manager_add_order(int floor, OrderType type) {
    if (!is_valid_floor(floor)) return;
    
    switch (type) {
        case ORDER_TYPE_CAB:
            cab_orders[floor] = true;
            break;

        case ORDER_TYPE_HALL_UP:
            if (floor < N_FLOORS - 1) {
                hall_up_orders[floor] = true;
            }
            break;

        case ORDER_TYPE_HALL_DOWN:
            if (floor > 0) {
                hall_down_orders[floor] = true;
            }
            break;
    }
}

void order_manager_clear_orders_at_floor(int floor, Direction direction) {
    if (!is_valid_floor(floor)) return;
    
    cab_orders[floor] = false;
    
    if (direction == DIR_UP && floor < N_FLOORS - 1) {
        hall_up_orders[floor] = false;
    }
    if (direction == DIR_DOWN && floor > 0) {
        hall_down_orders[floor] = false;
    }
    
    printf("Cleared orders at floor %d (direction: %s)\n", 
           floor, direction_to_string(direction));
}

bool order_manager_has_orders(void) {
    for (int i = 0; i < N_FLOORS; i++) {
        if (cab_orders[i]) return true;
    }
    for (int i = 0; i < N_FLOORS - 1; i++) {
        if (hall_up_orders[i]) return true;
        if (hall_down_orders[i]) return true;
    }
    return false;
}

bool order_manager_should_stop(int floor, Direction direction) {
    if (!is_valid_floor(floor)) return false;
    
    if (cab_orders[floor]) return true;
    
    if (direction == DIR_UP && floor < N_FLOORS - 1) {
        if (hall_up_orders[floor]) return true;
    }
    if (direction == DIR_DOWN && floor > 0) {
        if (hall_down_orders[floor]) return true;
    }
    
    return false;
}

Direction order_manager_get_next_direction(int current_floor, Direction current_direction) {
    if (current_direction == DIR_UP || current_direction == DIR_STOP) {
        for (int f = current_floor + 1; f < N_FLOORS; f++) {
            if (cab_orders[f] || hall_up_orders[f] || hall_down_orders[f]) {
                return DIR_UP;
            }
        }
    }
    
    if (current_direction == DIR_DOWN || current_direction == DIR_STOP) {
        for (int f = current_floor - 1; f >= 0; f--) {
            if (cab_orders[f] || hall_up_orders[f] || hall_down_orders[f]) {
                return DIR_DOWN;
            }
        }
    }
    
    return DIR_STOP;
}

void order_manager_clear_all_orders(void) {
    order_manager_init();
}

bool order_manager_has_orders_above(int floor) {
    for (int f = floor + 1; f < N_FLOORS; f++) {
        if (cab_orders[f]) return true;
        if (f < N_FLOORS - 1 && hall_up_orders[f]) return true;
        if (hall_down_orders[f]) return true;
    }
    return false;
}

bool order_manager_has_orders_below(int floor) {
    for (int f = 0; f < floor; f++) {
        if (cab_orders[f]) return true;
        if (hall_up_orders[f]) return true;
        if (f > 0 && hall_down_orders[f]) return true;
    }
    return false;
}