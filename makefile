CC = gcc
CFLAGS = -Wall -Wextra -std=c23 -g
SRC_DIR = ./src
TARGET = dsp_simulator

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/dsp_arithmetic.c $(SRC_DIR)/register_module.c $(SRC_DIR)/dsp_io.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

# Проверка утечек памяти с помощью Valgrind
check: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

.PHONY: all clean check
