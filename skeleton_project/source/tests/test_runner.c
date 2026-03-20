/**
 * @file test_runner.c
 * @brief Main test runner for elevator unit tests.
 */

#include <stdio.h>

extern int run_order_manager_tests(void);
extern int run_door_control_tests(void);

int main(void) {
    int failed = 0;

    printf("Running unit tests...\n\n");

    failed += run_order_manager_tests();
    failed += run_door_control_tests();

    printf("\n");
    if (failed == 0) {
        printf("All tests PASSED\n");
    } else {
        printf("%d test(s) FAILED\n", failed);
    }

    return failed;
}
