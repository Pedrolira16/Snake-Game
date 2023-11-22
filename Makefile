CC = gcc
CFLAGS = -Wall -g -I./include
SRC = $(wildcard ./src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = snake_game

$(EXEC): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(EXEC) $(OBJ)