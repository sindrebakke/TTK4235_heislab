#!/usr/bin/env python3
"""
UML Diagram Generator for Heislab (TTK4235 Elevator Lab)

This script generates various UML diagrams to visualize the architecture
and functionality of the elevator control system.

Requirements:
    pip install graphviz

Usage:
    python generate_uml.py

Output:
    - uml_diagrams/state_diagram.png
    - uml_diagrams/module_diagram.png
    - uml_diagrams/sequence_diagram.png
    - uml_diagrams/component_diagram.png
"""

import os
from graphviz import Digraph, Graph

# Create output directory
OUTPUT_DIR = "uml_diagrams"
os.makedirs(OUTPUT_DIR, exist_ok=True)


def create_state_diagram():
    """
    Creates a state diagram showing the elevator FSM states and transitions.
    """
    dot = Digraph(comment='Elevator FSM State Diagram')
    dot.attr(rankdir='TB', size='12,12')
    dot.attr('node', shape='ellipse', style='filled', fillcolor='lightblue')

    # Define states
    states = {
        'init': ('STATE_INIT', 'Initialization\n(Move down until floor found)'),
        'idle': ('STATE_IDLE', 'Idle\n(Wait for orders)'),
        'moving_up': ('STATE_MOVING_UP', 'Moving Up\n(Check floors for orders)'),
        'moving_down': ('STATE_MOVING_DOWN', 'Moving Down\n(Check floors for orders)'),
        'door_open': ('STATE_DOOR_OPEN', 'Door Open\n(3 sec timer)'),
        'emergency': ('STATE_EMERGENCY_STOP', 'Emergency Stop\n(Clear all orders)')
    }

    # Start node
    dot.node('start', '', shape='point', width='0.2', fillcolor='black')

    # Add state nodes
    for state_id, (name, description) in states.items():
        label = f'{name}\n\n{description}'
        if state_id == 'emergency':
            dot.node(state_id, label, fillcolor='#ff9999')
        elif state_id == 'init':
            dot.node(state_id, label, fillcolor='#99ff99')
        else:
            dot.node(state_id, label)

    # Transitions
    dot.edge('start', 'init', label='System Start')
    dot.edge('init', 'idle', label='EVENT_FLOOR_ARRIVED')

    dot.edge('idle', 'moving_up', label='Orders above\nEVENT_TICK')
    dot.edge('idle', 'moving_down', label='Orders below\nEVENT_TICK')
    dot.edge('idle', 'door_open', label='Order at floor\nEVENT_ORDER_RECEIVED')

    dot.edge('moving_up', 'door_open', label='Should stop\nEVENT_FLOOR_ARRIVED')
    dot.edge('moving_down', 'door_open', label='Should stop\nEVENT_FLOOR_ARRIVED')

    dot.edge('door_open', 'idle', label='EVENT_DOOR_TIMEOUT')
    dot.edge('door_open', 'door_open', label='EVENT_OBSTRUCTION\n(reset timer)')

    # Emergency transitions
    dot.edge('idle', 'emergency', label='EVENT_STOP_PRESSED', style='dashed', color='red')
    dot.edge('moving_up', 'emergency', label='EVENT_STOP_PRESSED', style='dashed', color='red')
    dot.edge('moving_down', 'emergency', label='EVENT_STOP_PRESSED', style='dashed', color='red')
    dot.edge('door_open', 'emergency', label='EVENT_STOP_PRESSED', style='dashed', color='red')
    dot.edge('emergency', 'idle', label='EVENT_STOP_RELEASED\n(at floor)', style='dashed', color='green')
    dot.edge('emergency', 'init', label='EVENT_STOP_RELEASED\n(between floors)', style='dashed', color='green')

    # Legend
    with dot.subgraph(name='cluster_legend') as legend:
        legend.attr(label='Legend', style='rounded', color='gray')
        legend.node('leg1', 'Normal State', shape='ellipse', fillcolor='lightblue')
        legend.node('leg2', 'Initial State', shape='ellipse', fillcolor='#99ff99')
        legend.node('leg3', 'Emergency State', shape='ellipse', fillcolor='#ff9999')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'state_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"State diagram saved to {output_path}.png")
    return dot


def create_state_entry_exit_diagram():
    """
    Creates a detailed state diagram showing entry and exit actions for all states.
    Based on actual implementation in elevator_fsm.c
    """
    dot = Digraph(comment='State Entry/Exit Actions')
    dot.attr(rankdir='TB', size='16,20')
    dot.attr('node', shape='box', style='filled,rounded')

    # STATE_INIT
    dot.node('init',
             'STATE_INIT\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_INIT\n'
             '  current_floor = read_floor_sensor()\n'
             '  if (current_floor != -1):\n'
             '      fsm_transition(state_idle)\n'
             '  else:\n'
             '      set_motor_direction(DIR_DOWN)\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  (no action)',
             fillcolor='#99ff99')

    # STATE_IDLE
    dot.node('idle',
             'STATE_IDLE\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_IDLE\n'
             '  set_motor_direction(DIR_STOP)\n'
             '  current_direction = DIR_STOP\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  (no action)',
             fillcolor='#cce6ff')

    # STATE_MOVING_UP
    dot.node('moving_up',
             'STATE_MOVING_UP\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_MOVING_UP\n'
             '  current_direction = DIR_UP\n'
             '  set_motor_direction(DIR_UP)\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  set_motor_direction(DIR_STOP)',
             fillcolor='#cce6ff')

    # STATE_MOVING_DOWN
    dot.node('moving_down',
             'STATE_MOVING_DOWN\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_MOVING_DOWN\n'
             '  current_direction = DIR_DOWN\n'
             '  set_motor_direction(DIR_DOWN)\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  set_motor_direction(DIR_STOP)',
             fillcolor='#cce6ff')

    # STATE_DOOR_OPEN
    dot.node('door_open',
             'STATE_DOOR_OPEN\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_DOOR_OPEN\n'
             '  set_motor_direction(DIR_STOP)\n'
             '  clear_orders_at_floor(floor, dir)\n'
             '  door_control_open_door()\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  door_control_close_door()',
             fillcolor='#ffffcc')

    # STATE_EMERGENCY_STOP
    dot.node('emergency',
             'STATE_EMERGENCY_STOP\n'
             '═══════════════════════════════════\n'
             'ENTRY:\n'
             '  current_state_id = STATE_EMERGENCY_STOP\n'
             '  set_motor_direction(DIR_STOP)\n'
             '  if (current_floor != -1):\n'
             '      door_control_open_door()\n'
             '  order_manager_clear_all_orders()\n'
             '───────────────────────────────────\n'
             'EXIT:\n'
             '  if (current_floor != -1):\n'
             '      door_control_close_door()',
             fillcolor='#ffcccc')

    # Start node
    dot.node('start', '', shape='point', width='0.3', fillcolor='black')

    # Transitions with events
    dot.edge('start', 'init', label='System Start', penwidth='2')

    dot.edge('init', 'idle', label='floor != -1', penwidth='2')

    dot.edge('idle', 'moving_up', label='has_orders() &&\nnext_dir == DIR_UP', penwidth='2')
    dot.edge('idle', 'moving_down', label='has_orders() &&\nnext_dir == DIR_DOWN', penwidth='2')
    dot.edge('idle', 'door_open', label='should_stop(floor, DIR_STOP)', penwidth='2')

    dot.edge('moving_up', 'door_open', label='should_stop(floor, DIR_UP)', penwidth='2')
    dot.edge('moving_up', 'idle', label='floor >= N_FLOORS-1\n(top floor)', penwidth='2', style='dashed')

    dot.edge('moving_down', 'door_open', label='should_stop(floor, DIR_DOWN)', penwidth='2')
    dot.edge('moving_down', 'idle', label='floor <= 0\n(bottom floor)', penwidth='2', style='dashed')

    dot.edge('door_open', 'idle', label='door_state == DOOR_CLOSED\n(timer expired)', penwidth='2')

    # Emergency transitions
    dot.edge('idle', 'emergency', label='EVENT_STOP_PRESSED', color='red', penwidth='2')
    dot.edge('moving_up', 'emergency', label='EVENT_STOP_PRESSED', color='red', penwidth='2')
    dot.edge('moving_down', 'emergency', label='EVENT_STOP_PRESSED', color='red', penwidth='2')
    dot.edge('door_open', 'emergency', label='EVENT_STOP_PRESSED', color='red', penwidth='2')
    dot.edge('emergency', 'idle', label='EVENT_STOP_RELEASED', color='green', penwidth='2')

    # Legend
    with dot.subgraph(name='cluster_legend') as legend:
        legend.attr(label='Legend', style='rounded', color='gray', fontsize='12')
        legend.node('leg1', 'Initial State', shape='box', fillcolor='#99ff99')
        legend.node('leg2', 'Normal State', shape='box', fillcolor='#cce6ff')
        legend.node('leg3', 'Door State', shape='box', fillcolor='#ffffcc')
        legend.node('leg4', 'Emergency State', shape='box', fillcolor='#ffcccc')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'state_entry_exit_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"State entry/exit diagram saved to {output_path}.png")

    # Also create SVG for better quality
    dot.render(output_path, format='svg', cleanup=True)
    print(f"State entry/exit diagram saved to {output_path}.svg")

    return dot


def create_module_diagram():
    """
    Creates a module/class diagram showing the system architecture.
    """
    dot = Digraph(comment='Module Diagram')
    dot.attr(rankdir='TB', size='14,14')
    dot.attr('node', shape='box', style='filled,rounded', fillcolor='lightyellow')

    # Main module
    dot.node('main',
             'main.c\n'
             '─────────────────\n'
             'Entry Point\n'
             '+ main()\n'
             '+ 100ms event loop\n'
             '+ Initialize modules\n'
             '+ Poll hardware\n'
             '+ Dispatch FSM events')

    # FSM Framework
    dot.node('fsm',
             'fsm.c/h\n'
             '─────────────────\n'
             'Generic FSM Framework\n'
             '+ fsm_t (struct)\n'
             '+ fsm_events_t (enum)\n'
             '+ fsm_dispatch(event)\n'
             '+ fsm_transition(state)')

    # Elevator FSM
    dot.node('elevator_fsm',
             'elevator_fsm.c/h\n'
             '─────────────────\n'
             'Elevator FSM\n'
             '+ state_init()\n'
             '+ state_idle()\n'
             '+ state_moving_up()\n'
             '+ state_moving_down()\n'
             '+ state_door_open()\n'
             '+ state_emergency_stop()')

    # Order Manager
    dot.node('order_manager',
             'order_manager.c\n'
             '─────────────────\n'
             'Order Management\n'
             '+ cab_orders[4]\n'
             '+ hall_up_orders[3]\n'
             '+ hall_down_orders[3]\n'
             '+ add_order()\n'
             '+ clear_orders()\n'
             '+ should_stop()\n'
             '+ get_next_direction()')

    # Door Control
    dot.node('door_control',
             'door_control.c\n'
             '─────────────────\n'
             'Door Control\n'
             '+ door_state\n'
             '+ door_open_time\n'
             '+ DURATION=3s\n'
             '+ open_door()\n'
             '+ close_door()\n'
             '+ update()\n'
             '+ reset_timer()')

    # Hardware Interface
    dot.node('hardware_interface',
             'hardware_interface.c\n'
             '─────────────────\n'
             'Hardware Abstraction\n'
             '+ init()\n'
             '+ poll_buttons()\n'
             '+ set_motor_direction()\n'
             '+ read_floor_sensor()\n'
             '+ update_lights()')

    # Types
    dot.node('elevator_types',
             'elevator_types.h\n'
             '─────────────────\n'
             'Shared Types\n'
             '+ N_FLOORS = 4\n'
             '+ Direction\n'
             '+ OrderType\n'
             '+ DoorState',
             fillcolor='#e6e6fa')

    # Driver
    dot.node('elevio',
             'driver/elevio.c/h\n'
             '─────────────────\n'
             'Low-Level Driver\n'
             'TCP Socket Comm\n'
             '+ elevio_init()\n'
             '+ motorDirection()\n'
             '+ buttonLamp()\n'
             '+ floorIndicator()\n'
             '+ callButton()\n'
             '+ floorSensor()',
             fillcolor='#ffd699')

    # Hardware simulator
    dot.node('simulator',
             'Hardware Simulator\n'
             '─────────────────\n'
             'External Process\n'
             'TCP Server',
             shape='box3d', fillcolor='#d9d9d9')

    # Dependencies
    dot.edge('main', 'fsm', label='uses')
    dot.edge('main', 'elevator_fsm', label='initializes')
    dot.edge('main', 'hardware_interface', label='polls')
    dot.edge('main', 'order_manager', label='initializes')
    dot.edge('main', 'door_control', label='initializes')

    dot.edge('elevator_fsm', 'fsm', label='extends')
    dot.edge('elevator_fsm', 'order_manager', label='queries')
    dot.edge('elevator_fsm', 'door_control', label='controls')
    dot.edge('elevator_fsm', 'hardware_interface', label='uses')
    dot.edge('elevator_fsm', 'elevator_types', label='includes', style='dashed')

    dot.edge('hardware_interface', 'elevio', label='wraps')
    dot.edge('hardware_interface', 'order_manager', label='adds orders')
    dot.edge('hardware_interface', 'elevator_types', label='includes', style='dashed')

    dot.edge('order_manager', 'elevator_types', label='includes', style='dashed')
    dot.edge('door_control', 'elevator_types', label='includes', style='dashed')

    dot.edge('elevio', 'simulator', label='TCP socket', style='bold', color='blue')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'module_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Module diagram saved to {output_path}.png")
    return dot


def create_sequence_diagram():
    """
    Creates a sequence diagram showing a typical order handling flow.
    """
    dot = Digraph(comment='Sequence Diagram - Order Handling')
    dot.attr(rankdir='TB', size='16,20')

    # Use subgraphs for alignment
    # Lifelines (participants)
    participants = ['User', 'Hardware', 'HW_Interface', 'Order_Manager', 'FSM', 'Door_Control']

    # Create participant boxes
    for i, p in enumerate(participants):
        dot.node(f'{p}_head', p, shape='box', style='filled', fillcolor='lightgray')

    # Create the sequence
    dot.attr('node', shape='point', width='0.1')

    # Create vertical lines (simplified - Graphviz isn't ideal for sequence diagrams)
    # We'll create a flow diagram instead that shows the sequence

    dot = Digraph(comment='Sequence Diagram - Order Handling')
    dot.attr(rankdir='LR', size='18,10')
    dot.attr('node', shape='box', style='rounded,filled', fillcolor='white')

    # Sequence steps
    steps = [
        ('1', 'User presses\nhall button', '#e6ffe6'),
        ('2', 'Hardware sends\nbutton signal', '#fff2e6'),
        ('3', 'hardware_interface\npoll_buttons()', '#e6f2ff'),
        ('4', 'order_manager\nadd_order(floor, type)', '#ffe6e6'),
        ('5', 'Set button\nlamp ON', '#fff2e6'),
        ('6', 'fsm_dispatch\n(EVENT_ORDER_RECEIVED)', '#f2e6ff'),
        ('7', 'Transition to\nSTATE_MOVING_UP/DOWN', '#f2e6ff'),
        ('8', 'set_motor_direction\n(DIR_UP/DOWN)', '#e6f2ff'),
        ('9', 'Floor sensor\ntriggered', '#fff2e6'),
        ('10', 'fsm_dispatch\n(EVENT_FLOOR_ARRIVED)', '#f2e6ff'),
        ('11', 'order_manager\nshould_stop()?', '#ffe6e6'),
        ('12', 'Transition to\nSTATE_DOOR_OPEN', '#f2e6ff'),
        ('13', 'door_control\nopen_door()', '#ffffe6'),
        ('14', 'Clear orders\nat floor', '#ffe6e6'),
        ('15', 'Set button\nlamp OFF', '#fff2e6'),
        ('16', '3 second\ntimer', '#ffffe6'),
        ('17', 'fsm_dispatch\n(EVENT_DOOR_TIMEOUT)', '#f2e6ff'),
        ('18', 'door_control\nclose_door()', '#ffffe6'),
        ('19', 'Transition to\nSTATE_IDLE', '#f2e6ff'),
    ]

    # Create nodes and edges
    for i, (num, label, color) in enumerate(steps):
        dot.node(f'step{num}', f'{num}. {label}', fillcolor=color)
        if i > 0:
            prev_num = steps[i-1][0]
            dot.edge(f'step{prev_num}', f'step{num}')

    # Legend
    with dot.subgraph(name='cluster_legend') as legend:
        legend.attr(label='Color Legend', style='rounded')
        legend.node('leg_user', 'User Action', fillcolor='#e6ffe6')
        legend.node('leg_hw', 'Hardware', fillcolor='#fff2e6')
        legend.node('leg_hwi', 'HW Interface', fillcolor='#e6f2ff')
        legend.node('leg_order', 'Order Manager', fillcolor='#ffe6e6')
        legend.node('leg_fsm', 'FSM', fillcolor='#f2e6ff')
        legend.node('leg_door', 'Door Control', fillcolor='#ffffe6')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'sequence_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Sequence diagram saved to {output_path}.png")
    return dot


def create_component_diagram():
    """
    Creates a component diagram showing the layered architecture.
    """
    dot = Digraph(comment='Component Diagram')
    dot.attr(rankdir='TB', size='12,14')

    # Application Layer
    with dot.subgraph(name='cluster_app') as app:
        app.attr(label='Application Layer', style='rounded', color='blue', bgcolor='#e6f3ff')
        app.node('main_app', 'main.c\nEvent Loop', shape='component', style='filled', fillcolor='white')

    # Control Layer
    with dot.subgraph(name='cluster_control') as ctrl:
        ctrl.attr(label='Control Layer', style='rounded', color='green', bgcolor='#e6ffe6')
        ctrl.node('fsm_ctrl', 'FSM Framework\n(fsm.c)', shape='component', style='filled', fillcolor='white')
        ctrl.node('elevator_fsm_ctrl', 'Elevator FSM\n(elevator_fsm.c)', shape='component', style='filled', fillcolor='white')
        ctrl.node('order_ctrl', 'Order Manager\n(order_manager.c)', shape='component', style='filled', fillcolor='white')
        ctrl.node('door_ctrl', 'Door Control\n(door_control.c)', shape='component', style='filled', fillcolor='white')

    # Hardware Abstraction Layer
    with dot.subgraph(name='cluster_hal') as hal:
        hal.attr(label='Hardware Abstraction Layer', style='rounded', color='orange', bgcolor='#fff2e6')
        hal.node('hw_interface', 'Hardware Interface\n(hardware_interface.c)', shape='component', style='filled', fillcolor='white')

    # Driver Layer
    with dot.subgraph(name='cluster_driver') as drv:
        drv.attr(label='Driver Layer', style='rounded', color='purple', bgcolor='#f2e6ff')
        drv.node('elevio_drv', 'Elevio Driver\n(driver/elevio.c)', shape='component', style='filled', fillcolor='white')

    # Hardware
    with dot.subgraph(name='cluster_hw') as hw:
        hw.attr(label='Hardware/Simulator', style='rounded', color='gray', bgcolor='#e6e6e6')
        hw.node('simulator', 'Elevator\nSimulator', shape='box3d', style='filled', fillcolor='white')

    # Shared
    with dot.subgraph(name='cluster_shared') as shared:
        shared.attr(label='Shared Types', style='rounded', color='red', bgcolor='#ffe6e6')
        shared.node('types', 'elevator_types.h', shape='note', style='filled', fillcolor='white')

    # Connections
    dot.edge('main_app', 'fsm_ctrl', label='dispatch events')
    dot.edge('main_app', 'elevator_fsm_ctrl', label='initialize')
    dot.edge('main_app', 'hw_interface', label='poll/control')

    dot.edge('elevator_fsm_ctrl', 'fsm_ctrl', label='uses')
    dot.edge('elevator_fsm_ctrl', 'order_ctrl', label='query orders')
    dot.edge('elevator_fsm_ctrl', 'door_ctrl', label='control door')
    dot.edge('elevator_fsm_ctrl', 'hw_interface', label='motor/sensors')

    dot.edge('hw_interface', 'order_ctrl', label='add orders')
    dot.edge('hw_interface', 'elevio_drv', label='wraps')

    dot.edge('elevio_drv', 'simulator', label='TCP socket', style='bold', color='blue')

    # Types dependencies
    dot.edge('elevator_fsm_ctrl', 'types', style='dashed')
    dot.edge('order_ctrl', 'types', style='dashed')
    dot.edge('door_ctrl', 'types', style='dashed')
    dot.edge('hw_interface', 'types', style='dashed')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'component_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Component diagram saved to {output_path}.png")
    return dot


def create_data_flow_diagram():
    """
    Creates a data flow diagram showing how data moves through the system.
    """
    dot = Digraph(comment='Data Flow Diagram')
    dot.attr(rankdir='LR', size='16,10')

    # External entities
    dot.attr('node', shape='box', style='filled', fillcolor='#d9d9d9')
    dot.node('user', 'User\n(Button Press)')
    dot.node('hw', 'Elevator\nHardware')

    # Processes
    dot.attr('node', shape='ellipse', style='filled', fillcolor='#99ccff')
    dot.node('p1', '1.0\nPoll Buttons')
    dot.node('p2', '2.0\nProcess Order')
    dot.node('p3', '3.0\nFSM Logic')
    dot.node('p4', '4.0\nControl Door')
    dot.node('p5', '5.0\nControl Motor')
    dot.node('p6', '6.0\nUpdate Lights')

    # Data stores
    dot.attr('node', shape='cylinder', style='filled', fillcolor='#ffcc99')
    dot.node('d1', 'Order\nArrays')
    dot.node('d2', 'FSM\nState')
    dot.node('d3', 'Door\nState')

    # Data flows
    dot.edge('user', 'p1', label='button signal')
    dot.edge('hw', 'p1', label='sensor data')

    dot.edge('p1', 'p2', label='button event')
    dot.edge('p2', 'd1', label='store order')
    dot.edge('p1', 'p3', label='floor/stop events')

    dot.edge('d1', 'p3', label='query orders')
    dot.edge('p3', 'd2', label='update state')
    dot.edge('d2', 'p3', label='current state')

    dot.edge('p3', 'p4', label='door commands')
    dot.edge('p4', 'd3', label='update door')
    dot.edge('d3', 'p4', label='door status')
    dot.edge('p4', 'hw', label='door lamp')

    dot.edge('p3', 'p5', label='direction')
    dot.edge('p5', 'hw', label='motor signal')

    dot.edge('p3', 'p2', label='clear orders')
    dot.edge('p2', 'p6', label='update lamps')
    dot.edge('p6', 'hw', label='lamp signals')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'data_flow_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Data flow diagram saved to {output_path}.png")
    return dot


def create_event_diagram():
    """
    Creates a diagram showing all FSM events and their sources.
    """
    dot = Digraph(comment='Event Diagram')
    dot.attr(rankdir='LR', size='14,10')

    # Event sources
    with dot.subgraph(name='cluster_sources') as src:
        src.attr(label='Event Sources', style='rounded', color='blue')
        src.node('timer', 'Timer\n(main loop)', shape='box', style='filled', fillcolor='#e6f2ff')
        src.node('floor_sensor', 'Floor\nSensor', shape='box', style='filled', fillcolor='#e6f2ff')
        src.node('buttons', 'Buttons\n(cab/hall)', shape='box', style='filled', fillcolor='#e6f2ff')
        src.node('stop_btn', 'Stop\nButton', shape='box', style='filled', fillcolor='#ffe6e6')
        src.node('obstruction', 'Obstruction\nSensor', shape='box', style='filled', fillcolor='#e6f2ff')
        src.node('door_timer', 'Door\nTimer', shape='box', style='filled', fillcolor='#e6f2ff')
        src.node('fsm_internal', 'FSM\nInternal', shape='box', style='filled', fillcolor='#f2e6ff')

    # Events
    with dot.subgraph(name='cluster_events') as evt:
        evt.attr(label='FSM Events', style='rounded', color='green')
        events = [
            ('EVENT_TICK', '#99ff99'),
            ('EVENT_ENTRY', '#ffff99'),
            ('EVENT_EXIT', '#ffff99'),
            ('EVENT_ORDER_RECEIVED', '#99ff99'),
            ('EVENT_FLOOR_ARRIVED', '#99ff99'),
            ('EVENT_DOOR_TIMEOUT', '#99ff99'),
            ('EVENT_STOP_PRESSED', '#ff9999'),
            ('EVENT_STOP_RELEASED', '#ff9999'),
            ('EVENT_OBSTRUCTION', '#99ff99'),
            ('EVENT_OBSTRUCTION_CLEAR', '#99ff99'),
        ]
        for event, color in events:
            evt.node(event, event, shape='oval', style='filled', fillcolor=color)

    # FSM
    dot.node('fsm', 'Finite State\nMachine', shape='box3d', style='filled', fillcolor='#d9d9d9')

    # Connections
    dot.edge('timer', 'EVENT_TICK', label='100ms')
    dot.edge('floor_sensor', 'EVENT_FLOOR_ARRIVED')
    dot.edge('buttons', 'EVENT_ORDER_RECEIVED')
    dot.edge('stop_btn', 'EVENT_STOP_PRESSED')
    dot.edge('stop_btn', 'EVENT_STOP_RELEASED')
    dot.edge('obstruction', 'EVENT_OBSTRUCTION')
    dot.edge('obstruction', 'EVENT_OBSTRUCTION_CLEAR')
    dot.edge('door_timer', 'EVENT_DOOR_TIMEOUT')
    dot.edge('fsm_internal', 'EVENT_ENTRY')
    dot.edge('fsm_internal', 'EVENT_EXIT')

    # All events to FSM
    for event, _ in events:
        dot.edge(event, 'fsm', style='dashed')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'event_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Event diagram saved to {output_path}.png")
    return dot


def create_order_handling_diagram():
    """
    Creates a diagram showing how orders are managed.
    """
    dot = Digraph(comment='Order Handling Diagram')
    dot.attr(rankdir='TB', size='12,14')

    # Buttons
    with dot.subgraph(name='cluster_buttons') as btns:
        btns.attr(label='Button Inputs', style='rounded', color='blue')
        for floor in range(4):
            btns.node(f'cab_{floor}', f'Cab {floor}', shape='box', style='filled', fillcolor='#99ccff')
        for floor in range(3):
            btns.node(f'hall_up_{floor}', f'Hall Up {floor}', shape='box', style='filled', fillcolor='#99ff99')
        for floor in range(1, 4):
            btns.node(f'hall_down_{floor}', f'Hall Down {floor}', shape='box', style='filled', fillcolor='#ff9999')

    # Order arrays
    with dot.subgraph(name='cluster_orders') as orders:
        orders.attr(label='Order Arrays', style='rounded', color='orange')
        orders.node('cab_orders', 'cab_orders[4]\n{F0, F1, F2, F3}', shape='record', style='filled', fillcolor='#ffcc99')
        orders.node('hall_up', 'hall_up_orders[3]\n{F0, F1, F2}', shape='record', style='filled', fillcolor='#99ff99')
        orders.node('hall_down', 'hall_down_orders[3]\n{F1, F2, F3}', shape='record', style='filled', fillcolor='#ff9999')

    # Decision logic
    dot.node('scheduler', 'Scheduler Logic\nget_next_direction()\nshould_stop()', shape='diamond', style='filled', fillcolor='#e6e6fa')

    # Outputs
    with dot.subgraph(name='cluster_outputs') as out:
        out.attr(label='Outputs', style='rounded', color='green')
        out.node('dir', 'Direction\n(UP/DOWN/STOP)', shape='box', style='filled', fillcolor='#d9d9d9')
        out.node('stop', 'Should Stop?\n(bool)', shape='box', style='filled', fillcolor='#d9d9d9')

    # Connections from buttons to arrays
    for floor in range(4):
        dot.edge(f'cab_{floor}', 'cab_orders')
    for floor in range(3):
        dot.edge(f'hall_up_{floor}', 'hall_up')
    for floor in range(1, 4):
        dot.edge(f'hall_down_{floor}', 'hall_down')

    # Arrays to scheduler
    dot.edge('cab_orders', 'scheduler')
    dot.edge('hall_up', 'scheduler')
    dot.edge('hall_down', 'scheduler')

    # Scheduler to outputs
    dot.edge('scheduler', 'dir')
    dot.edge('scheduler', 'stop')

    # Render
    output_path = os.path.join(OUTPUT_DIR, 'order_handling_diagram')
    dot.render(output_path, format='png', cleanup=True)
    print(f"Order handling diagram saved to {output_path}.png")
    return dot


def generate_plantuml_entry_exit():
    """
    Generates PlantUML code for a detailed state diagram with entry/exit actions.
    """
    plantuml = '''@startuml State Entry Exit Diagram

skinparam state {
    BackgroundColor<<init>> #99ff99
    BackgroundColor<<normal>> #cce6ff
    BackgroundColor<<door>> #ffffcc
    BackgroundColor<<emergency>> #ffcccc
    BorderColor Black
}

title Elevator FSM - Entry/Exit Actions

[*] --> STATE_INIT

state STATE_INIT <<init>> {
    STATE_INIT : **ENTRY:**
    STATE_INIT : - current_state_id = STATE_INIT
    STATE_INIT : - current_floor = read_floor_sensor()
    STATE_INIT : - if (current_floor != -1):
    STATE_INIT : -     fsm_transition(state_idle)
    STATE_INIT : - else:
    STATE_INIT : -     set_motor_direction(DIR_DOWN)
    STATE_INIT : ---
    STATE_INIT : **EXIT:**
    STATE_INIT : - (no action)
}

state STATE_IDLE <<normal>> {
    STATE_IDLE : **ENTRY:**
    STATE_IDLE : - current_state_id = STATE_IDLE
    STATE_IDLE : - set_motor_direction(DIR_STOP)
    STATE_IDLE : - current_direction = DIR_STOP
    STATE_IDLE : ---
    STATE_IDLE : **EXIT:**
    STATE_IDLE : - (no action)
}

state STATE_MOVING_UP <<normal>> {
    STATE_MOVING_UP : **ENTRY:**
    STATE_MOVING_UP : - current_state_id = STATE_MOVING_UP
    STATE_MOVING_UP : - current_direction = DIR_UP
    STATE_MOVING_UP : - set_motor_direction(DIR_UP)
    STATE_MOVING_UP : ---
    STATE_MOVING_UP : **EXIT:**
    STATE_MOVING_UP : - set_motor_direction(DIR_STOP)
}

state STATE_MOVING_DOWN <<normal>> {
    STATE_MOVING_DOWN : **ENTRY:**
    STATE_MOVING_DOWN : - current_state_id = STATE_MOVING_DOWN
    STATE_MOVING_DOWN : - current_direction = DIR_DOWN
    STATE_MOVING_DOWN : - set_motor_direction(DIR_DOWN)
    STATE_MOVING_DOWN : ---
    STATE_MOVING_DOWN : **EXIT:**
    STATE_MOVING_DOWN : - set_motor_direction(DIR_STOP)
}

state STATE_DOOR_OPEN <<door>> {
    STATE_DOOR_OPEN : **ENTRY:**
    STATE_DOOR_OPEN : - current_state_id = STATE_DOOR_OPEN
    STATE_DOOR_OPEN : - set_motor_direction(DIR_STOP)
    STATE_DOOR_OPEN : - clear_orders_at_floor(floor, dir)
    STATE_DOOR_OPEN : - door_control_open_door()
    STATE_DOOR_OPEN : ---
    STATE_DOOR_OPEN : **EXIT:**
    STATE_DOOR_OPEN : - door_control_close_door()
}

state STATE_EMERGENCY_STOP <<emergency>> {
    STATE_EMERGENCY_STOP : **ENTRY:**
    STATE_EMERGENCY_STOP : - current_state_id = STATE_EMERGENCY_STOP
    STATE_EMERGENCY_STOP : - set_motor_direction(DIR_STOP)
    STATE_EMERGENCY_STOP : - if (current_floor != -1):
    STATE_EMERGENCY_STOP : -     door_control_open_door()
    STATE_EMERGENCY_STOP : - order_manager_clear_all_orders()
    STATE_EMERGENCY_STOP : ---
    STATE_EMERGENCY_STOP : **EXIT:**
    STATE_EMERGENCY_STOP : - if (current_floor != -1):
    STATE_EMERGENCY_STOP : -     door_control_close_door()
}

STATE_INIT --> STATE_IDLE : floor != -1

STATE_IDLE --> STATE_MOVING_UP : has_orders() &&\\nnext_dir == DIR_UP
STATE_IDLE --> STATE_MOVING_DOWN : has_orders() &&\\nnext_dir == DIR_DOWN
STATE_IDLE --> STATE_DOOR_OPEN : should_stop(floor, DIR_STOP)

STATE_MOVING_UP --> STATE_DOOR_OPEN : should_stop(floor, DIR_UP)
STATE_MOVING_UP --> STATE_IDLE : floor >= N_FLOORS-1 (top)

STATE_MOVING_DOWN --> STATE_DOOR_OPEN : should_stop(floor, DIR_DOWN)
STATE_MOVING_DOWN --> STATE_IDLE : floor <= 0 (bottom)

STATE_DOOR_OPEN --> STATE_IDLE : door_state == DOOR_CLOSED

STATE_IDLE -[#red]-> STATE_EMERGENCY_STOP : EVENT_STOP_PRESSED
STATE_MOVING_UP -[#red]-> STATE_EMERGENCY_STOP : EVENT_STOP_PRESSED
STATE_MOVING_DOWN -[#red]-> STATE_EMERGENCY_STOP : EVENT_STOP_PRESSED
STATE_DOOR_OPEN -[#red]-> STATE_EMERGENCY_STOP : EVENT_STOP_PRESSED

STATE_EMERGENCY_STOP -[#green]-> STATE_IDLE : EVENT_STOP_RELEASED

legend right
    |= Color |= Meaning |
    | <#99ff99> | Initial State |
    | <#cce6ff> | Normal State |
    | <#ffffcc> | Door State |
    | <#ffcccc> | Emergency State |
endlegend

@enduml
'''
    output_path = os.path.join(OUTPUT_DIR, 'state_entry_exit.puml')
    with open(output_path, 'w') as f:
        f.write(plantuml)
    print(f"PlantUML entry/exit diagram saved to {output_path}")
    return plantuml


def generate_plantuml_state():
    """
    Generates PlantUML code for a state diagram (text-based, can be used with PlantUML).
    """
    plantuml = '''@startuml Elevator State Machine

skinparam state {
    BackgroundColor LightBlue
    BorderColor Black
    StartColor Green
}

[*] --> STATE_INIT : System Start

STATE_INIT : Move down until floor found
STATE_INIT --> STATE_IDLE : Floor sensor triggered

STATE_IDLE : Wait for orders
STATE_IDLE --> STATE_MOVING_UP : Orders above
STATE_IDLE --> STATE_MOVING_DOWN : Orders below
STATE_IDLE --> STATE_DOOR_OPEN : Order at current floor

STATE_MOVING_UP : Moving upward
STATE_MOVING_UP --> STATE_DOOR_OPEN : Should stop at floor
STATE_MOVING_UP : Check floor sensor

STATE_MOVING_DOWN : Moving downward
STATE_MOVING_DOWN --> STATE_DOOR_OPEN : Should stop at floor
STATE_MOVING_DOWN : Check floor sensor

STATE_DOOR_OPEN : Door open (3s timer)
STATE_DOOR_OPEN --> STATE_IDLE : Timer expired
STATE_DOOR_OPEN --> STATE_DOOR_OPEN : Obstruction (reset timer)
STATE_DOOR_OPEN : Clear orders at floor

STATE_EMERGENCY_STOP : Emergency stop pressed
STATE_EMERGENCY_STOP : Clear all orders
STATE_EMERGENCY_STOP : Stop motor
STATE_EMERGENCY_STOP --> STATE_IDLE : Stop released (at floor)
STATE_EMERGENCY_STOP --> STATE_INIT : Stop released (between floors)

STATE_IDLE --> STATE_EMERGENCY_STOP : Stop button
STATE_MOVING_UP --> STATE_EMERGENCY_STOP : Stop button
STATE_MOVING_DOWN --> STATE_EMERGENCY_STOP : Stop button
STATE_DOOR_OPEN --> STATE_EMERGENCY_STOP : Stop button

note right of STATE_EMERGENCY_STOP #pink
  Emergency state
  - Clears all orders
  - Stops motor immediately
  - Opens door if at floor
end note

@enduml
'''
    output_path = os.path.join(OUTPUT_DIR, 'state_diagram.puml')
    with open(output_path, 'w') as f:
        f.write(plantuml)
    print(f"PlantUML state diagram saved to {output_path}")
    return plantuml


def generate_plantuml_class():
    """
    Generates PlantUML code for a class/module diagram.
    """
    plantuml = '''@startuml Module Diagram

skinparam classAttributeIconSize 0
skinparam class {
    BackgroundColor LightYellow
    BorderColor Black
}

package "Application" {
    class main {
        + main()
        - poll_loop()
    }
}

package "Control Layer" {
    class fsm <<framework>> {
        + fsm_t state
        + fsm_dispatch(event)
        + fsm_transition(state)
    }

    class elevator_fsm {
        - current_state_id
        - current_floor
        - current_direction
        + elevator_fsm_init()
        + state_init()
        + state_idle()
        + state_moving_up()
        + state_moving_down()
        + state_door_open()
        + state_emergency_stop()
    }

    class order_manager {
        - cab_orders[4]
        - hall_up_orders[3]
        - hall_down_orders[3]
        + order_manager_add_order()
        + order_manager_clear_orders()
        + order_manager_should_stop()
        + order_manager_get_next_direction()
    }

    class door_control {
        - door_state
        - door_open_time
        - DOOR_OPEN_DURATION
        + door_control_open_door()
        + door_control_close_door()
        + door_control_update()
        + door_control_reset_timer()
    }
}

package "Hardware Abstraction" {
    class hardware_interface {
        + hardware_interface_init()
        + hardware_interface_poll_buttons()
        + hardware_interface_set_motor_direction()
        + hardware_interface_read_floor_sensor()
        + hardware_interface_update_lights()
    }
}

package "Driver" {
    class elevio {
        - sockfd
        + elevio_init()
        + elevio_motorDirection()
        + elevio_buttonLamp()
        + elevio_floorIndicator()
        + elevio_callButton()
    }
}

package "Types" #DDDDDD {
    enum Direction {
        DIR_DOWN
        DIR_STOP
        DIR_UP
    }

    enum OrderType {
        ORDER_TYPE_HALL_UP
        ORDER_TYPE_HALL_DOWN
        ORDER_TYPE_CAB
    }

    enum DoorState {
        DOOR_CLOSED
        DOOR_OPEN
        DOOR_OBSTRUCTED
    }
}

main --> fsm : uses
main --> elevator_fsm : initializes
main --> hardware_interface : polls

elevator_fsm --|> fsm : extends
elevator_fsm --> order_manager : queries
elevator_fsm --> door_control : controls
elevator_fsm --> hardware_interface : uses

hardware_interface --> elevio : wraps
hardware_interface --> order_manager : adds orders

elevator_fsm ..> Direction
elevator_fsm ..> OrderType
door_control ..> DoorState
order_manager ..> OrderType

@enduml
'''
    output_path = os.path.join(OUTPUT_DIR, 'module_diagram.puml')
    with open(output_path, 'w') as f:
        f.write(plantuml)
    print(f"PlantUML module diagram saved to {output_path}")
    return plantuml


def generate_plantuml_sequence():
    """
    Generates PlantUML code for a sequence diagram.
    """
    plantuml = '''@startuml Order Handling Sequence

skinparam sequenceArrowThickness 2
skinparam sequenceParticipant underline

actor User
participant "Hardware" as HW
participant "hardware_interface" as HWI
participant "order_manager" as OM
participant "fsm" as FSM
participant "elevator_fsm" as EFSM
participant "door_control" as DC

== Order Received ==
User -> HW : Press hall button (floor 2, up)
HW -> HWI : Button signal
HWI -> OM : order_manager_add_order(2, HALL_UP)
OM --> HWI : Order stored
HWI -> HW : Set button lamp ON
HWI -> FSM : fsm_dispatch(EVENT_ORDER_RECEIVED)

== Movement ==
FSM -> EFSM : state_idle(EVENT_ORDER_RECEIVED)
EFSM -> OM : order_manager_get_next_direction()
OM --> EFSM : DIR_UP
EFSM -> FSM : fsm_transition(state_moving_up)
EFSM -> HWI : set_motor_direction(DIR_UP)
HWI -> HW : Motor UP

loop Every 100ms
    HWI -> HW : Read floor sensor
    HW --> HWI : Floor position
    alt At floor with order
        HWI -> FSM : fsm_dispatch(EVENT_FLOOR_ARRIVED)
    end
end

== Arrival at Floor ==
FSM -> EFSM : state_moving_up(EVENT_FLOOR_ARRIVED)
EFSM -> OM : order_manager_should_stop(2, DIR_UP)
OM --> EFSM : true
EFSM -> FSM : fsm_transition(state_door_open)
EFSM -> HWI : set_motor_direction(DIR_STOP)
EFSM -> DC : door_control_open_door()
DC -> HW : Door lamp ON

== Door Open ==
EFSM -> OM : order_manager_clear_orders_at_floor(2, DIR_UP)
OM --> HWI : Clear order
HWI -> HW : Button lamp OFF

note right of DC : 3 second timer

DC -> FSM : fsm_dispatch(EVENT_DOOR_TIMEOUT)
FSM -> EFSM : state_door_open(EVENT_DOOR_TIMEOUT)
EFSM -> DC : door_control_close_door()
DC -> HW : Door lamp OFF
EFSM -> FSM : fsm_transition(state_idle)

@enduml
'''
    output_path = os.path.join(OUTPUT_DIR, 'sequence_diagram.puml')
    with open(output_path, 'w') as f:
        f.write(plantuml)
    print(f"PlantUML sequence diagram saved to {output_path}")
    return plantuml


def main():
    """
    Main function to generate all UML diagrams.
    """
    print("=" * 60)
    print("UML Diagram Generator for Heislab (TTK4235)")
    print("=" * 60)
    print()

    try:
        # Generate Graphviz diagrams
        print("Generating Graphviz diagrams...")
        print("-" * 40)
        create_state_diagram()
        create_state_entry_exit_diagram()
        create_module_diagram()
        create_sequence_diagram()
        create_component_diagram()
        create_data_flow_diagram()
        create_event_diagram()
        create_order_handling_diagram()

        print()
        print("Generating PlantUML source files...")
        print("-" * 40)
        generate_plantuml_state()
        generate_plantuml_entry_exit()
        generate_plantuml_class()
        generate_plantuml_sequence()

        print()
        print("=" * 60)
        print(f"All diagrams saved to: {os.path.abspath(OUTPUT_DIR)}/")
        print("=" * 60)
        print()
        print("Generated files:")
        for f in sorted(os.listdir(OUTPUT_DIR)):
            print(f"  - {f}")
        print()
        print("Note: PlantUML files (.puml) can be rendered at:")
        print("  - https://www.plantuml.com/plantuml/uml")
        print("  - VS Code PlantUML extension")
        print("  - IntelliJ PlantUML plugin")

    except ImportError:
        print("ERROR: graphviz package not installed.")
        print("Install with: pip install graphviz")
        print()
        print("Also ensure Graphviz is installed on your system:")
        print("  macOS: brew install graphviz")
        print("  Ubuntu: sudo apt install graphviz")
        print("  Windows: https://graphviz.org/download/")
        return 1

    return 0


if __name__ == "__main__":
    exit(main())
