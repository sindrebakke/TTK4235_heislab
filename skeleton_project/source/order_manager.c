
#include "elevator_types.h"
#include <stdbool.h>
#include <stdio.h>

static bool cab_orders[N_FLOORS];
static bool hall_up_orders[N_FLOORS - 1];
static bool hall_down_orders[N_FLOORS - 1];


static void order_manager_print_status(void) {
    printf("\n[ORDERS] --------- ORDER STATUS --------\n");
    printf("[ORDERS] CAB:       ");
    for (int i = 0; i < N_FLOORS; i++) {
        printf("%d:%s ", i, cab_orders[i] ? "X" : "-");
    }
    printf("\n[ORDERS] HALL_UP:   ");
    for (int i = 0; i < N_FLOORS - 1; i++) {
        printf("%d:%s ", i, hall_up_orders[i] ? "X" : "-");
    }
    printf("\n[ORDERS] HALL_DOWN: ");
    for (int i = 0; i < N_FLOORS - 1; i++) {
        printf("%d:%s ", i + 1, hall_down_orders[i] ? "X" : "-");
    }
    printf("\n");
}

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

    bool was_set = false;
    switch (type) {
        case ORDER_TYPE_CAB:
            if (!cab_orders[floor]) was_set = true;
            cab_orders[floor] = true;
            break;

        case ORDER_TYPE_HALL_UP:
            if (floor < N_FLOORS - 1) {
                if (!hall_up_orders[floor]) was_set = true;
                hall_up_orders[floor] = true;
            }
            break;

        case ORDER_TYPE_HALL_DOWN:
            if (floor > 0 && floor < N_FLOORS) {
                if (!hall_down_orders[floor - 1]) was_set = true;
                hall_down_orders[floor - 1] = true;
            }
            break;
    }

    if (was_set) {
        printf("[ORDERS] New order: floor %d, type %s\n", floor, order_type_to_string(type));
        order_manager_print_status();
    }
}

void order_manager_clear_orders_at_floor(int floor, Direction direction) {
    if (!is_valid_floor(floor)) return;
    
    cab_orders[floor] = false;
    
    if (direction == DIR_UP && floor < N_FLOORS - 1) {
        hall_up_orders[floor] = false;
    }
    if (direction == DIR_DOWN && floor > 0) {
        hall_down_orders[floor - 1] = false;
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
        if (hall_down_orders[floor - 1]) return true;
    }
    
    return false;
}

Direction order_manager_get_next_direction(int current_floor, Direction current_direction) {
    Direction result = DIR_STOP;

    if (current_direction == DIR_UP || current_direction == DIR_STOP) {
        for (int f = current_floor + 1; f < N_FLOORS; f++) {
            bool has_hall_up = (f < N_FLOORS - 1) && hall_up_orders[f];
            bool has_hall_down = (f > 0) && hall_down_orders[f - 1];
            if (cab_orders[f] || has_hall_up || has_hall_down) {
                result = DIR_UP;
                printf("[DECISION] Floor %d, direction %s -> choosing %s (order on floor %d)\n",
                       current_floor, direction_to_string(current_direction),
                       direction_to_string(result), f);
                return result;
            }
        }
    }

    if (current_direction == DIR_DOWN || current_direction == DIR_STOP) {
        for (int f = current_floor - 1; f >= 0; f--) {
            bool has_hall_up = (f < N_FLOORS - 1) && hall_up_orders[f];
            bool has_hall_down = (f > 0) && hall_down_orders[f - 1];
            if (cab_orders[f] || has_hall_up || has_hall_down) {
                result = DIR_DOWN;
                printf("[DECISION] Floor %d, direction %s -> choosing %s (order on floor %d)\n",
                       current_floor, direction_to_string(current_direction),
                       direction_to_string(result), f);
                return result;
            }
        }
    }

    return result;
}

void order_manager_clear_all_orders(void) {
    order_manager_init();
}

bool order_manager_has_orders_above(int floor) {
    for (int f = floor + 1; f < N_FLOORS; f++) {
        if (cab_orders[f]) return true;
        if (f < N_FLOORS - 1 && hall_up_orders[f]) return true;
        if (f > 0 && hall_down_orders[f - 1]) return true;
    }
    return false;
}

bool order_manager_has_orders_below(int floor) {
    for (int f = 0; f < floor; f++) {
        if (cab_orders[f]) return true;
        if (f < N_FLOORS - 1 && hall_up_orders[f]) return true;
        if (f > 0 && hall_down_orders[f - 1]) return true;
    }
    return false;
}