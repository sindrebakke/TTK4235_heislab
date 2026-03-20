CC = gcc
CFLAGS = -Wall -Wextra -g -Isource

SOURCES = source/main.c \
          source/fsm.c \
          source/elevator_fsm.c \
          source/order_manager.c \
          source/hardware_interface.c \
          source/door_control.c \
          source/driver/elevio.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = elevator

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

docs:
	doxygen Doxyfile

TEST_SOURCES = source/tests/test_runner.c \
               source/tests/test_order_manager.c \
               source/tests/test_door_control.c \
               source/order_manager.c \
               source/door_control.c

test: $(TEST_SOURCES)
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o test_elevator
	./test_elevator

.PHONY: all clean docs test