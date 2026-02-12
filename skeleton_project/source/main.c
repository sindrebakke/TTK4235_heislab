#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "fsm.h"
#include "elevator_fsm.h"

// Forward declarations of functions from .c-modules
bool hardware_interface_init(void);
void hardware_interface_poll_buttons(void);
void hardware_interface_update_lights(int current_floor);
bool hardware_interface_read_stop_button(void);
bool hardware_interface_read_obstruction(void);

void order_manager_init(void);
void door_control_init(void);

int main() {
    
    if (!hardware_interface_init()) {
        printf("ERROR: Failed to initialize hardware\n");
        return 1;
    }
    
    order_manager_init();
    door_control_init();
    elevator_fsm_init();
    
    bool prev_stop_state = false;
    
    while (1) {
        hardware_interface_poll_buttons();
        
        bool stop_pressed = hardware_interface_read_stop_button();
        if (stop_pressed && !prev_stop_state) {
            fsm_dispatch(EVENT_STOP_PRESSED);
        } else if (!stop_pressed && prev_stop_state) {
            fsm_dispatch(EVENT_STOP_RELEASED);
        }
        prev_stop_state = stop_pressed;
        
        if (hardware_interface_read_obstruction()) {
            fsm_dispatch(EVENT_OBSTRUCTION);
        }
        
        fsm_dispatch(EVENT_TICK);
        
        hardware_interface_update_lights(current_floor);
        
        usleep(100000); // 100ms delay
    }
    
    return 0;
}