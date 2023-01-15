// Copyright joaquind 2022
#ifndef SRC_CAT_FUNCTIONS_H_
#define SRC_CAT_FUNCTIONS_H_
#include <stdio.h>

#define NUMBER_NONBLANK_1 'b'
#define NUMBER_NONBLANK_2 "--number-nonblank"
#define END_OF_STRING_1 'e'
#define END_OF_STRING_2 'E'
#define NUMBER_ALL_STRINGS_1 'n'
#define NUMBER_ALL_STRINGS_2 "--number"
#define SQUEEZE_BLANK_1 's'
#define SQUEEZE_BLANK_2 "--squeeze-blank"
#define DISPLAY_TABS_1 't'
#define DISPLAY_TABS_2 'T'
#define DISPLAY_SPECSIMBOL 'v'

struct cat_flags {
  int flag_b;
  int flag_e;
  int flag_n;
  int flag_s;
  int flag_t;
  int flag_v;
  int flag_T;
  int flag_E;
  int count_files;
};

void numerate_nonempty_strings(FILE *filename, FILE *output);
void end_of_strings(FILE *input, FILE *output);
void numerate_all_strings(FILE *input, FILE *output);
void squeeze_blank(FILE *filename, FILE *output);
void display_tabs(FILE *input, FILE *output);
void display_invisible_symbols(int ch, FILE *output);
void copy_file(FILE *filename, FILE *copy);
void rewrite(FILE *input, FILE *output);
void check_flags(struct cat_flags flags, FILE *input, FILE *output);
void cat_flag_v(FILE *input, FILE *output);
long int end_of_file(FILE *input);
int check_position(long int position, FILE *input);
int check_last_string(FILE *input, FILE *output, long int end_pos, int string);
int sort_flags(struct cat_flags *flags, int argc, char *argv[]);
void print_result(FILE *filename);

#endif  // SRC_CAT_FUNCTIONS_H_
