CC = gcc
CFLAGS = -Wall -Wextra -g -I.

SOURCES = source/main.c \
          source/fsm/fsm.c \
          source/fsm/elevator_fsm.c \
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

.PHONY: all clean