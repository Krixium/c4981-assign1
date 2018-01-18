CC = gcc
NAME = assign1
CFLAGS = -w -Wall -ansi -pedantic -o $(NAME) -Werror

all: *.c
	$(CC) $(CFLAGS) *.c

clean:
	rm $(NAME)
