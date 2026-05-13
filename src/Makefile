CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror

all: cat grep

cat:
	$(CC) $(CFLAGS) apps/cat/cat.c -o apps/cat/cat

grep:
	$(CC) $(CFLAGS) apps/grep/grep.c -o apps/grep/grep

test_cat:
	bash tests/test_cat.sh

test_grep:
	bash tests/test_grep.sh

test: test_cat test_grep

clean:
	rm -rf *.o apps/cat/cat apps/grep/grep

rebuild: clean all