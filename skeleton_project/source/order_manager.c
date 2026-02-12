/**
 * @file order_manager.c
 * @brief Order management system for the elevator.
 *
 * Manages cab orders and hall call orders. Provides functions for
 * adding, clearing, and querying orders to determine elevator behavior.
 */

#include "elevator_types.h"
#include <stdbool.h>
#include <stdio.h>

/** @brief Cab button orders for each floor. */
static bool cab_orders[N_FLOORS];

/** @brief Hall up button orders (floors 0 to N_FLOORS-2). */
static bool hall_up_orders[N_FLOORS - 1];

/** @brief Hall down button orders (floors 1 to N_FLOORS-1, indexed as 0 to N_FLOORS-2). */
static bool hall_down_orders[N_FLOORS - 1];

/**
 * @brief Prints current order status to console.
 *
 * Displays all active orders for debugging purposes.
 */
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

/**
 * @brief Initializes the order manager.
 *
 * Clears all orders from all floors.
 */
void order_manager_init(void) {
    for (int i = 0; i < N_FLOORS; i++) {
        cab_orders[i] = false;
    }
    for (int i = 0; i < N_FLOORS - 1; i++) {
        hall_up_orders[i] = false;
        hall_down_orders[i] = false;
    }
}

/**
 * @brief Adds a new order.
 *
 * @param floor The floor number (0 to N_FLOORS-1).
 * @param type The order type (CAB, HALL_UP, or HALL_DOWN).
 */
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

/**
 * @brief Clears orders at a specific floor.
 *
 * Clears the cab order and the hall order matching the current direction.
 *
 * @param floor The floor number.
 * @param direction The current elevator direction.
 */
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

/**
 * @brief Checks if there are any pending orders.
 *
 * @return true if there are orders, false otherwise.
 */
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

/**
 * @brief Determines if the elevator should stop at a floor.
 *
 * @param floor The floor to check.
 * @param direction The current movement direction.
 * @return true if the elevator should stop, false otherwise.
 */
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

/**
 * @brief Determines the next direction based on current position and orders.
 *
 * Implements a simple elevator algorithm: continue in current direction
 * if there are orders ahead, otherwise reverse or stop.
 *
 * @param current_floor The current floor position.
 * @param current_direction The current movement direction.
 * @return The next direction to move (DIR_UP, DIR_DOWN, or DIR_STOP).
 */
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

/**
 * @brief Clears all orders.
 *
 * Used during emergency stop to reset all pending orders.
 */
void order_manager_clear_all_orders(void) {
    order_manager_init();
}

/**
 * @brief Checks if there are orders above a given floor.
 *
 * @param floor The reference floor.
 * @return true if there are orders above, false otherwise.
 */
bool order_manager_has_orders_above(int floor) {
    for (int f = floor + 1; f < N_FLOORS; f++) {
        if (cab_orders[f]) return true;
        if (f < N_FLOORS - 1 && hall_up_orders[f]) return true;
        if (f > 0 && hall_down_orders[f - 1]) return true;
    }
    return false;
}

/**
 * @brief Checks if there are orders below a given floor.
 *
 * @param floor The reference floor.
 * @return true if there are orders below, false otherwise.
 */
bool order_manager_has_orders_below(int floor) {
    for (int f = 0; f < floor; f++) {
        if (cab_orders[f]) return true;
        if (f < N_FLOORS - 1 && hall_up_orders[f]) return true;
        if (f > 0 && hall_down_orders[f - 1]) return true;
    }
    return false;
}
