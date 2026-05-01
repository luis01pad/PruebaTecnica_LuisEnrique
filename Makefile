CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -Isrc
LDFLAGS = -pthread
SRC = src/buffer.c src/config.c src/crc.c src/main.c
OBJ = $(SRC:.c=.o)
TARGET = bin/app
TEST_SRC = src/buffer.c src/config.c src/crc.c tests/test.c
TEST_TARGET = bin/test
all: $(TARGET)
$(TARGET): $(SRC)
	@if not exist bin mkdir bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
test: $(TEST_TARGET)
$(TEST_TARGET): $(TEST_SRC)
	@if not exist bin mkdir bin
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_TARGET) $(LDFLAGS)
clean:
	if exist bin rmdir /s /q bin