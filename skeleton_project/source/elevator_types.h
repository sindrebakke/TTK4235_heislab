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
    DOOR_OPENN,
    DOOR_OBSTRUCTED
} DoorState;

