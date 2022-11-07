#ifndef _SRC_GREP_FUNCTIONS_H_
#define _SRC_GREP_FUNCTIONS_H_
#include <pcre.h>

typedef struct {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
  long unsigned int count_files;
  int len_pattern;
  char **files;
  char *patterns;
} grep_flags;

void sort_bash_first(int argc, char *argv[], grep_flags *flags);
void sort_bash_second(int argc, char *argv[], grep_flags *flags);
int find_flags(char ch, grep_flags *flags);
int get_memory(grep_flags *flags);
void free_memory(grep_flags *flags);
void add_pattern_from_file(char *filename, grep_flags *flags);
void add_pattern(char *patternname, grep_flags *flags);
void add_file(char *filename, grep_flags *flags);
void make_pcre_patterns(grep_flags *flags);
pcre *compile_pattern(grep_flags *flags, char *patternname);
void do_flag_o(grep_flags *flags, char *patternname);

#endif  // _SRC_GREP_FUNCTIONS_H_
