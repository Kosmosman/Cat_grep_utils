// Copyright joaquind 2022
#include "s21_grep.h"

#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int error = 0;
  grep_flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL};
  error = get_memory(&flags);
  if (!error) {
    sort_bash_first(argc, argv, &flags);
    sort_bash_second(argc, argv, &flags);
    make_pcre_patterns(&flags);
    free_memory(&flags);
  }
  return 0;
}

void sort_bash_first(int argc, char *argv[], grep_flags *flags) {
  int result = 1;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0' && result; j++) {
        result = find_flags(argv[i][j], flags);
      }
      result = 1;
    }
  }
}

void sort_bash_second(int argc, char *argv[], grep_flags *flags) {
  int i = 0, j = 0;
  int option = 0, out = 0;
  int point_e = 0, point_f = 0;
  int was_there_a_pattern = 0;
  option = (flags->flag_e || flags->flag_f) ? 1 : 0;
  if (option == 1) {
    for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        for (j = 1; argv[i][j] != '\0' && !out; j++) {
          if (argv[i][j] == 'e' && argv[i][j + 1] != '\0') {
            add_pattern(&argv[i][j + 1], flags);
            out = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] != '\0') {
            add_pattern_from_file(&argv[i][j + 1], flags);
          } else if (argv[i][j] == 'e' && argv[i][j + 1] == '\0') {
            point_e = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] == '\0') {
            point_f = 1;
          }
        }
      } else {
        if (point_f) {
          add_pattern_from_file(argv[i], flags);
          point_f = 0;
        } else if (point_e) {
          add_pattern(argv[i], flags);
          point_e = 0;
        } else {
          add_file(argv[i], flags);
        }
      }
      out = 0;
    }
  } else {
    for (i = 1; i < argc; i++) {
      if (argv[i][0] != '-' && !was_there_a_pattern) {
        add_pattern(argv[i], flags);
        was_there_a_pattern = 1;
      } else if (argv[i][0] != '-') {
        add_file(argv[i], flags);
      }
    }
  }
}

int find_flags(char ch, grep_flags *flags) {
  int res = 1;
  if (ch == 'e') {
    flags->flag_e += 1;
    res = 0;
  } else if (ch == 'i') {
    flags->flag_i += 1;
  } else if (ch == 'v') {
    flags->flag_v += 1;
  } else if (ch == 'c') {
    flags->flag_c += 1;
  } else if (ch == 'l') {
    flags->flag_l += 1;
  } else if (ch == 'n') {
    flags->flag_n += 1;
  } else if (ch == 'h') {
    flags->flag_h += 1;
  } else if (ch == 's') {
    flags->flag_s += 1;
  } else if (ch == 'f') {
    flags->flag_f += 1;
    res = 0;
  } else if (ch == 'o') {
    flags->flag_o += 1;
  }
  return res;
}

int get_memory(grep_flags *flags) {
  int error = 0;
  flags->files = (char **)calloc(2048, sizeof(char *));
  flags->patterns = (char *)calloc(2048, sizeof(char));
  if (flags->files == NULL) {
    error = 1;
    free(flags->files);
  }
  if (flags->patterns == NULL) {
    error = 1;
    free(flags->patterns);
  }
  if (flags->files != NULL && flags->patterns != NULL) {
    for (int i = 0; i < 2048 && !error; i++) {
      flags->files[i] = (char *)calloc(2048, sizeof(char));
      if (flags->files[i] == NULL) {
        for (int j = 0; j < i; j++) {
          free(flags->files[i]);
        }
        free(flags->files);
        free(flags->patterns);
        error = 1;
      }
    }
  }
  return error;
}

void free_memory(grep_flags *flags) {
  for (long unsigned int i = 0; i < 2048; i++) {
    free(flags->files[i]);
  }
  free(flags->files);
  free(flags->patterns);
}

void add_pattern_from_file(char *filename, grep_flags *flags) {
  FILE *stream = NULL;
  stream = fopen(filename, "r");
  char str[1024] = {0};
  if (stream) {
    while (!feof(stream)) {
      str[0] = '\0';
      fgets(str, sizeof(str), stream);
      for (int c = 0; str[c] != EOF && str[c] != '\0'; c++) {
        if (str[c] == '\n' && c > 0 && strlen(str) > 1) {
          str[c] = '\0';
        }
      }
      add_pattern(str, flags);
    }
    fclose(stream);
  } else {
    free_memory(flags);
    exit(1);
  }
}

void add_pattern(char *patternname, grep_flags *flags) {
  if (flags->len_pattern != 0) {
    flags->patterns[flags->len_pattern++] = '|';
  }
  if (patternname[0] == ')') {
    flags->patterns[flags->len_pattern++] = '\'';
    flags->patterns[flags->len_pattern++] = '\\';
    flags->patterns[flags->len_pattern++] = ')';
    flags->patterns[flags->len_pattern++] = '\'';
  } else {
    strcpy(&flags->patterns[flags->len_pattern], patternname);
    flags->len_pattern += strlen(patternname);
  }
}

void add_file(char *filename, grep_flags *flags) {
  strcpy(flags->files[flags->count_files], filename);
  flags->count_files += 1;
}

void make_pcre_patterns(grep_flags *flags) {
  pcre *re = NULL;
  char str[1024] = {0};
  str[0] = '\0';
  int buffer[1024] = {0};
  int count = 0, string_num = 0;
  int c_flag_count = 0, out = 1;
  FILE *stream = NULL;
  re = compile_pattern(flags, flags->patterns);
  for (long unsigned int i = 0; i < flags->count_files; i++) {
    stream = fopen(flags->files[i], "r");
    if (stream) {
      while (!(feof(stream))) {
        string_num++;
        str[0] = '\0';
        fgets(str, sizeof(str), stream);
        count =
            pcre_exec(re, NULL, str, strlen(str), 0, 0, buffer, sizeof(buffer));
        if (count && strlen(str)) {
          if ((count > 0 && !(flags->flag_v)) || (count < 0 && flags->flag_v)) {
            if (flags->count_files > 1 && (!(flags->flag_l) || flags->flag_c) &&
                !(flags->flag_h) && out) {
              printf("%s:", flags->files[i]);
            }
            if (flags->flag_n && !(flags->flag_l) && !(flags->flag_c)) {
              printf("%d:", string_num);
            }
            if (flags->flag_c) {
              out = 0;
              c_flag_count++;
            }
            if (flags->flag_l) {
              if (flags->flag_c) {
                printf("%d\n", c_flag_count);
              }
              printf("%s\n", flags->files[i]);
              break;
            }
            if (!(flags->flag_c) && (!(flags->flag_o) || (flags->flag_v))) {
              printf("%s", str);
              if (str[strlen(str) - 1] != '\n') {
                printf("\n");
              }
            }
            if (flags->flag_o && !(flags->flag_c)) {
              do_flag_o(flags, str);
            }
          }
        }
      }
      if (count && flags->flag_c && (!(flags->flag_l) || !c_flag_count)) {
        if (flags->count_files > 1 && !(flags->flag_h) && c_flag_count == 0) {
          printf("%s:", flags->files[i]);
        }
        printf("%d\n", c_flag_count);
      }
      out = 1;
      c_flag_count = 0;
      fclose(stream);
    }
    string_num = 0;
  }
  if (re) {
    free(re);
  }
}

pcre *compile_pattern(grep_flags *flags, char *pattername) {
  const char *error = NULL;
  int num_error = 0;
  pcre *re = NULL;
  if (flags->flag_i) {
    re = pcre_compile(pattername, PCRE_CASELESS, &error, &num_error, NULL);
  } else {
    re = pcre_compile(pattername, 0, &error, &num_error, NULL);
  }
  if (!re) {
    free_memory(flags);
    free(re);
    exit(num_error);
  }
  return re;
}

void do_flag_o(grep_flags *flags, char *str) {
  pcre *re = NULL;
  int count = 0;
  int buffer[1024] = {0};
  char *start_pos = NULL;
  int single_pattern_pos = 0;
  long unsigned int present_pos = 0;
  char single_pattern[1024] = {0};
  single_pattern[0] = '\0';
  start_pos = str;
  for (long unsigned int c = 0; c <= strlen(flags->patterns); c++) {
    if (flags->patterns[c] == '|' || flags->patterns[c] == '\0') {
      single_pattern[single_pattern_pos] = '\0';
      single_pattern_pos = 0;
      re = compile_pattern(flags, single_pattern);
      while ((strlen(start_pos) >= strlen(single_pattern)) &&
             (count = pcre_exec(re, NULL, start_pos, strlen(str), 0, 0, buffer,
                                sizeof(buffer))) > 0 &&
             (strlen(str) >= (present_pos + buffer[1] - 1))) {
        for (int i = buffer[0]; i < buffer[1]; i++) {
          putchar(start_pos[i]);
        }
        printf("\n");
        start_pos = &start_pos[buffer[1]];
        present_pos += buffer[1];
      }
      free(re);
    } else {
      single_pattern[single_pattern_pos++] = flags->patterns[c];
    }
  }
  start_pos = NULL;
}
