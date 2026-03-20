/**
 * @file test_order_manager.c
 * @brief Unit tests for order_manager module.
 */

#include <stdio.h>
#include <stdbool.h>
#include "../elevator_types.h"

// Order manager functions
extern void order_manager_init(void);
extern void order_manager_add_order(int floor, OrderType type);
extern void order_manager_clear_orders_at_floor(int floor, Direction direction);
extern void order_manager_clear_all_orders(void);
extern bool order_manager_has_orders(void);
extern bool order_manager_has_orders_above(int floor);
extern bool order_manager_has_orders_below(int floor);
extern bool order_manager_should_stop(int floor, Direction direction);
extern Direction order_manager_get_next_direction(int current_floor, Direction current_direction);

static int test_init(void) {
    order_manager_init();
    return !order_manager_has_orders();
}

static int test_add_cab_order(void) {
    order_manager_init();
    order_manager_add_order(2, ORDER_TYPE_CAB);
    return order_manager_has_orders() && order_manager_should_stop(2, DIR_UP);
}

static int test_add_hall_up_order(void) {
    order_manager_init();
    order_manager_add_order(1, ORDER_TYPE_HALL_UP);
    return order_manager_has_orders() && order_manager_should_stop(1, DIR_UP);
}

static int test_add_hall_down_order(void) {
    order_manager_init();
    order_manager_add_order(2, ORDER_TYPE_HALL_DOWN);
    return order_manager_has_orders() && order_manager_should_stop(2, DIR_DOWN);
}

static int test_clear_orders_at_floor(void) {
    order_manager_init();
    order_manager_add_order(2, ORDER_TYPE_CAB);
    order_manager_add_order(2, ORDER_TYPE_HALL_UP);
    order_manager_clear_orders_at_floor(2, DIR_UP);
    return !order_manager_should_stop(2, DIR_UP);
}

static int test_clear_all_orders(void) {
    order_manager_init();
    order_manager_add_order(0, ORDER_TYPE_CAB);
    order_manager_add_order(1, ORDER_TYPE_HALL_UP);
    order_manager_add_order(3, ORDER_TYPE_HALL_DOWN);
    order_manager_clear_all_orders();
    return !order_manager_has_orders();
}

static int test_has_orders_above(void) {
    order_manager_init();
    order_manager_add_order(3, ORDER_TYPE_CAB);
    return order_manager_has_orders_above(1) && !order_manager_has_orders_above(3);
}

static int test_has_orders_below(void) {
    order_manager_init();
    order_manager_add_order(0, ORDER_TYPE_CAB);
    return order_manager_has_orders_below(2) && !order_manager_has_orders_below(0);
}

static int test_get_next_direction_up(void) {
    order_manager_init();
    order_manager_add_order(3, ORDER_TYPE_CAB);
    return order_manager_get_next_direction(0, DIR_UP) == DIR_UP;
}

static int test_get_next_direction_down(void) {
    order_manager_init();
    order_manager_add_order(0, ORDER_TYPE_CAB);
    return order_manager_get_next_direction(3, DIR_DOWN) == DIR_DOWN;
}

static int test_get_next_direction_stop(void) {
    order_manager_init();
    return order_manager_get_next_direction(1, DIR_STOP) == DIR_STOP;
}

int run_order_manager_tests(void) {
    int failed = 0;

    printf("Order Manager:\n");

    if (!test_init()) { printf("  FAIL: test_init\n"); failed++; }
    if (!test_add_cab_order()) { printf("  FAIL: test_add_cab_order\n"); failed++; }
    if (!test_add_hall_up_order()) { printf("  FAIL: test_add_hall_up_order\n"); failed++; }
    if (!test_add_hall_down_order()) { printf("  FAIL: test_add_hall_down_order\n"); failed++; }
    if (!test_clear_orders_at_floor()) { printf("  FAIL: test_clear_orders_at_floor\n"); failed++; }
    if (!test_clear_all_orders()) { printf("  FAIL: test_clear_all_orders\n"); failed++; }
    if (!test_has_orders_above()) { printf("  FAIL: test_has_orders_above\n"); failed++; }
    if (!test_has_orders_below()) { printf("  FAIL: test_has_orders_below\n"); failed++; }
    if (!test_get_next_direction_up()) { printf("  FAIL: test_get_next_direction_up\n"); failed++; }
    if (!test_get_next_direction_down()) { printf("  FAIL: test_get_next_direction_down\n"); failed++; }
    if (!test_get_next_direction_stop()) { printf("  FAIL: test_get_next_direction_stop\n"); failed++; }

    if (failed == 0) {
        printf("  All tests passed\n");
    }

    return failed;
}
