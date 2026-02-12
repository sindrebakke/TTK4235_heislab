#ifndef ELEVATOR_TYPES_H
#define ELEVATOR_TYPES_H

#include <stdbool.h>

#define N_FLOORS 4

typedef enum {
    DIR_DOWN = -1,
    DIR_STOP = 0,
    DIR_UP = 1
} Direction;

typedef enum {
    ORDER_TYPE_HALL_UP,
    ORDER_TYPE_HALL_DOWN,
    ORDER_TYPE_CAB
} OrderType;

typedef enum {
    DOOR_CLOSED,
    DOOR_OPEN,
    DOOR_OBSTRUCTED
} DoorState;

/**
 * @brief Converts OrderType to string for debugging
 */
static inline const char* order_type_to_string(OrderType type) {
    switch (type) {
        case ORDER_TYPE_HALL_UP: return "HALL_UP";
        case ORDER_TYPE_HALL_DOWN: return "HALL_DOWN";
        case ORDER_TYPE_CAB: return "CAB";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Converts DoorState to string for debugging
 */
static inline const char* door_state_to_string(DoorState state) {
    switch (state) {
        case DOOR_CLOSED: return "CLOSED";
        case DOOR_OPEN: return "OPEN";
        case DOOR_OBSTRUCTED: return "OBSTRUCTED";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Checks if a floor is valid
 */
static inline bool is_valid_floor(int floor) {
    return floor >= 0 && floor < N_FLOORS;
}

/**
 * @brief Converts Direction to string for debugging
 */
static inline const char* direction_to_string(Direction dir) {
    switch (dir) {
        case DIR_UP: return "UP";
        case DIR_DOWN: return "DOWN";
        case DIR_STOP: return "STOP";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Inverts direction
 */
static inline Direction direction_opposite(Direction dir) {
    if (dir == DIR_UP) return DIR_DOWN;
    if (dir == DIR_DOWN) return DIR_UP;
    return DIR_STOP;
}

#endif