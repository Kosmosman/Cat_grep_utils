CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: clean s21_cat check

s21_cat: s21_cat.c s21_cat.h
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat

clean: 
	rm -f s21_cat

rebuild: clean s21_cat

check:
	cp ../../materials/linters/clang-format ../../src/cat/clang-format
	clang-format -style=Google -n s21_cat.c s21_cat.h
	rm -f clang-format

test:
	sh test_func_cat.sh