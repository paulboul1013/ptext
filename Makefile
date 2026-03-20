
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = prototype_text

all: $(TARGET)

$(TARGET): main.c
	$(CC) main.c -o $(TARGET) $(CFLAGS)

main: $(TARGET)
