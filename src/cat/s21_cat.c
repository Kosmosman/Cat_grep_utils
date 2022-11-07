#include "s21_cat.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int check;
  char ch;
  FILE *stream;
  struct cat_flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  check = sort_flags(&flags, argc, argv);
  if (check) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        if ((stream = fopen(argv[i], "r"))) {
          FILE *input = fopen("input.txt", "w+");
          FILE *output = fopen("output.txt", "w+");
          if (input != NULL && output != NULL) {
            copy_file(stream, input);
            fseek(input, 0, SEEK_SET);
            check_flags(flags, input, output);
            while ((ch = fgetc(input)) != EOF) {
              putchar(ch);
            }
            fclose(input);
            fclose(output);
            remove("input.txt");
            remove("output.txt");
          }
          fclose(stream);
        }
      }
    }
  }
  return 0;
}

void copy_file(FILE *filename, FILE *copy) {
  while (!(feof(filename))) {
    fputc(fgetc(filename), copy);
  }
}

int sort_flags(struct cat_flags *flags, int argc, char *argv[]) {
  int result = 1;
  for (int i = 1; (i < argc) && result; i++) {
    if (argv[i][0] == '-' && !(flags->count_files)) {
      if (!strcmp(argv[i], NUMBER_NONBLANK_2)) {
        flags->flag_b += 1;
      } else if (!strcmp(argv[i], NUMBER_ALL_STRINGS_2)) {
        flags->flag_n += 1;
      } else if (!strcmp(argv[i], SQUEEZE_BLANK_2)) {
        flags->flag_s += 1;
      } else {
        for (long unsigned int j = 1; j < strlen(argv[i]); j++) {
          if (argv[i][j] == NUMBER_NONBLANK_1) {
            flags->flag_b += 1;
          } else if (argv[i][j] == END_OF_STRING_1) {
            flags->flag_e += 1;
          } else if (argv[i][j] == END_OF_STRING_2) {
            flags->flag_E += 1;
          } else if (argv[i][j] == NUMBER_ALL_STRINGS_1) {
            flags->flag_n += 1;
          } else if (argv[i][j] == SQUEEZE_BLANK_1) {
            flags->flag_s += 1;
          } else if (argv[i][j] == DISPLAY_TABS_1) {
            flags->flag_t += 1;
          } else if (argv[i][j] == DISPLAY_TABS_2) {
            flags->flag_T += 1;
          } else if (argv[i][j] == DISPLAY_SPECSIMBOL) {
            flags->flag_v += 1;
          } else {
            result = 0;
          }
        }
      }
    } else {
      flags->count_files += 1;
    }
  }
  return result;
}

void check_flags(struct cat_flags flags, FILE *input, FILE *output) {
  if (flags.flag_s) {
    squeeze_blank(input, output);
    rewrite(input, output);
  }
  if (flags.flag_t || flags.flag_T) {
    display_tabs(input, output);
    rewrite(input, output);
  }
  if (flags.flag_b) {
    numerate_nonempty_strings(input, output);
    rewrite(input, output);
  } else if (flags.flag_n) {
    numerate_all_strings(input, output);
    rewrite(input, output);
  }
  if (flags.flag_e || flags.flag_E) {
    end_of_strings(input, output);
    rewrite(input, output);
  }
  if (flags.flag_v && !flags.flag_e && !flags.flag_t) {
    cat_flag_v(input, output);
    rewrite(input, output);
  }
}

void rewrite(FILE *input, FILE *output) {
  fseek(input, 0, SEEK_SET);
  fputc('\0', input);
  fseek(input, 0, SEEK_SET);
  fseek(output, 0, SEEK_SET);
  copy_file(output, input);
  fseek(output, 0, SEEK_SET);
  fputc('\0', output);
  fseek(input, 0, SEEK_SET);
  fseek(output, 0, SEEK_SET);
}

void numerate_nonempty_strings(FILE *input, FILE *output) {
  long int end_pos = end_of_file(input);
  char ch;
  int end = check_position(end_pos, input);
  for (int i = 1; end; i++) {
    ch = fgetc(input);
    end = check_position(end_pos, input);
    if (ch == '\n') {
      i--;
      fputc(ch, output);
    } else {
      if (end) {
        fprintf(output, "%6d\t", i);
      }
      fputc(ch, output);
      while ((ch = getc(input)) != '\n' && end) {
        fputc(ch, output);
        end = check_position(end_pos, input);
      }
      fputc(ch, output);
    }
  }
}

void end_of_strings(FILE *input, FILE *output) {
  char ch;
  long int end_pos = end_of_file(input);
  int end = check_position(end_pos, input);
  while (end) {
    ch = fgetc(input);
    end = check_position(end_pos, input);
    if (ch != '\n') {
      display_invisible_symbols(ch, output);
    } else {
      fputc('$', output);
      fputc('\n', output);
    }
  }
}

void numerate_all_strings(FILE *input, FILE *output) {
  char str[1000];
  for (int i = 1; fgets(str, sizeof(str), input); i++) {
    if (str[0] != EOF) {
      fprintf(output, "%6d\t", i);
    }
    fputs(str, output);
    str[0] = '\0';
  }
}

void squeeze_blank(FILE *input, FILE *output) {
  char ch;
  long int end_pos = end_of_file(input);
  int end = check_position(end_pos, input);
  while (end) {
    ch = fgetc(input);
    end = check_position(end_pos, input);
    if (ch == '\n') {
      if (fgetc(input) != '\n') {
        fputc(ch, output);
      }
      fseek(input, -1, SEEK_CUR);
    } else {
      fputc(ch, output);
      while ((ch = getc(input)) != '\n' && end) {
        fputc(ch, output);
        end = check_position(end_pos, input);
      }
      fputc(ch, output);
    }
  }
}

void display_tabs(FILE *input, FILE *output) {
  char ch;
  long int end_pos = end_of_file(input);
  int end = check_position(end_pos, input);
  while (end) {
    ch = fgetc(input);
    end = check_position(end_pos, input);
    if (ch == '\t') {
      fprintf(output, "^I");
    } else {
      display_invisible_symbols(ch, output);
    }
  }
}

void display_invisible_symbols(int ch, FILE *output) {
  if (ch >= 0 && ch < 32 && ch != 9 && ch != 10) {
    fprintf(output, "^%c", ch + 64);
  } else if (ch > 127 && ch < 160) {
    fprintf(output, "M-^%c", ch - 64);
  } else if (ch == 127) {
    fprintf(output, "^%c", ch - 64);
  } else {
    fputc(ch, output);
  }
}

void cat_flag_v(FILE *input, FILE *output) {
  char ch;
  long int end_pos = end_of_file(input);
  int end = check_position(end_pos, input);
  while (end) {
    ch = fgetc(input);
    end = check_position(end_pos, input);
    display_invisible_symbols(ch, output);
  }
}

long int end_of_file(FILE *input) {
  long int end_pos;
  fseek(input, 0, SEEK_END);
  end_pos = ftell(input);
  fseek(input, 0, SEEK_SET);
  return end_pos;
}

int check_position(long int position, FILE *input) {
  int end = 1;
  fseek(input, 1, SEEK_CUR);
  if (ftell(input) >= position) {
    end = 0;
  }
  fseek(input, -1, SEEK_CUR);
  return end;
}