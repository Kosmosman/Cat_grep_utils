CC = gcc
CFLAGS = -std=c11 -lm -Wall -Wextra -Werror

all: clean s21_grep check

s21_grep: s21_grep.c s21_grep.h
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep -lpcre

clean:
	rm -f s21_grep

rebuild: clean s21_grep

check:
	cp ../../materials/linters/clang-format ../../src/grep/clang-format
	clang-format -style=Google -n s21_grep.c s21_grep.h
	rm -f clang-format

test:
	sh test_func_grep.sh