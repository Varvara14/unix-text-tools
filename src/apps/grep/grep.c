#include "grep.h"

int main(int argc, char *argv[]) {
  Flags flags = {0};
  char **patterns = NULL;
  int pattern_count = 0;
  char **files = NULL;
  int file_count = 0;

  parse(argc, argv, &flags, &patterns, &pattern_count, &files, &file_count);

  if (pattern_count == 0) {
    fprintf(stderr, "no pattern\n");
    exit(EXIT_FAILURE);
  }

  if (file_count == 0) {
    read_file(NULL, flags, patterns, pattern_count);
  } else {
    for (int i = 0; i < file_count; i++) {
      read_file(files[i], flags, patterns, pattern_count);
    }
  }

  free(patterns);
  free(files);
  return 0;
}

void parse(int argc, char *argv[], Flags *flags, char ***patterns,
           int *pattern_count, char ***files, int *file_count) {
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhs")) != -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        char **tmp = realloc(*patterns, (*pattern_count + 1) * sizeof(char *));
        if (tmp == NULL) {
          free(*patterns);
          fprintf(stderr, "memory allocation failed\n");
          exit(EXIT_FAILURE);
        }
        *patterns = tmp;
        (*patterns)[(*pattern_count)++] = optarg;
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case '?':
        fprintf(stderr, "?????? unknown option\n");
        exit(EXIT_FAILURE);
    }
  }

  if (*pattern_count == 0 && optind < argc) {  // шаблон не задан через е
    *patterns = malloc(sizeof(char *));
    (*patterns)[0] = argv[optind++];
    *pattern_count = 1;
  }

  *file_count = argc - optind;
  flags->multiple_files = (*file_count > 1);

  if (*file_count > 0) {
    *files = malloc(*file_count * sizeof(char *));
    for (int i = 0; i < *file_count; i++) {
      (*files)[i] = argv[optind + i];
    }
  }
}

void read_file(char *filename, Flags flags, char **patterns,
               int pattern_count) {
  FILE *file = filename ? fopen(filename, "r") : stdin;
  if (!file) {
    fprintf(stderr, "No such file or directory\n");
    return;
  }

  process(file, flags, patterns, pattern_count, filename);

  if (file != stdin) {
    fclose(file);
  }
}

void str_process(FILE *file, char **patterns, int pattern_count, Flags flags,
                 int *match_count_ptr, const char *filename) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_number = 0;
  regex_t re[pattern_count];
  int reti;

  for (int i = 0; i < pattern_count; i++) {
    int options = REG_EXTENDED;
    if (flags.i) options |= REG_ICASE;

    reti = regcomp(&re[i], patterns[i], options);
    if (reti) {
      char error_message[100];
      regerror(reti, &re[i], error_message, sizeof(error_message));
      fprintf(stderr, "compilation failed: %s\n", error_message);
      exit(EXIT_FAILURE);
    }
  }

  while ((read = getline(&line, &len, file)) != -1) {
    line_number++;
    int match = 0;
    int i = 0;

    while (i < pattern_count && !match) {
      reti = regexec(&re[i], line, 0, NULL, 0);
      if (!reti) {
        match = 1;
      }
      i++;
    }

    if ((match && !flags.v) || (!match && flags.v)) {
      (*match_count_ptr)++;
      if (!flags.c && !flags.l) {
        print_match(line, line_number, flags, filename);
      }
    }
  }

  free(line);
  for (int i = 0; i < pattern_count; i++) {
    regfree(&re[i]);
  }
}

void process(FILE *file, Flags flags, char **patterns, int pattern_count,
             char *filename) {
  int match_count = 0;
  str_process(file, patterns, pattern_count, flags, &match_count, filename);

  if (flags.c) {
    print_count(match_count, filename, flags);
  }
  if (flags.l && match_count > 0) {
    print_filename(filename);
  }
}

void print_match(const char *line, int line_number, Flags flags,
                 const char *filename) {
  if (flags.multiple_files) {
    printf("%s:", filename);
  }
  if (flags.n) {
    printf("%d:", line_number);
  }
  printf("%s", line);
}

void print_count(int count, const char *filename, Flags flags) {
  if (flags.multiple_files) {
    printf("%s:", filename);
  }
  printf("%d\n", count);
}

void print_filename(const char *filename) { printf("%s\n", filename); }