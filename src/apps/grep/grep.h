#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int multiple_files;
} Flags;

void parse(int argc, char *argv[], Flags *flags, char ***patterns,
           int *pattern_count, char ***files, int *file_count);
void read_file(char *filename, Flags flags, char **patterns, int pattern_count);
void str_process(FILE *file, char **patterns, int pattern_count, Flags flags,
                 int *match_count_ptr, const char *filename);
void process(FILE *file, Flags flags, char **patterns, int pattern_count,
             char *filename);
void print_match(const char *line, int line_number, Flags flags,
                 const char *filename);
void print_count(int count, const char *filename, Flags flags);
void print_filename(const char *filename);