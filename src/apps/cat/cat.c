#include "cat.h"

int main(int argc, char *argv[]) {
  Flags flags = {0};
  char **files = NULL;
  int file_count = 0;

  parse(argc, argv, &flags, &files, &file_count);
  if (file_count == 0) {
    read_file(NULL, flags);
  } else {
    for (int i = 0; i < file_count; i++) {
      read_file(files[i], flags);
    }
  }

  free(files);
  return 0;
}

void parse(int argc, char *argv[], Flags *flags, char ***files,
           int *file_count) {
  int opt;
  static struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  while ((opt = getopt_long(argc, argv, "bensvtET", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        flags->number_nonblank = 1;
        flags->number = 0;
        break;
      case 'E':
        flags->E = 1;
        break;
      case 'e':
        flags->e = 1;
        flags->v = 1;
        break;
      case 'n':
        if (!flags->number_nonblank) {
          flags->number = 1;
        }
        break;
      case 's':
        flags->squeeze_blank = 1;
        break;
      case 't':
        flags->t = 1;
        flags->v = 1;
        break;
      case 'T':
        flags->T = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case '?':
        fprintf(stderr, "?????? unknown option");
        exit(EXIT_FAILURE);
    }
  }

  *file_count = argc - optind;
  if (*file_count > 0) {
    *files = malloc(*file_count * sizeof(char *));
    for (int i = 0; i < *file_count; i++) {
      (*files)[i] = argv[optind + i];
    }
  }
}

void read_file(char *filename, Flags flags) {
  FILE *file = NULL;
  struct stat file_stat;
  int should_process = 1;

  if (filename == NULL) {
    file = stdin;
  } else {
    if (stat(filename, &file_stat) == 0 && S_ISDIR(file_stat.st_mode)) {
      fprintf(stderr, "is a directory");
      should_process = 0;
    }

    if (should_process) {
      file = fopen(filename, "r");
      if (file == NULL) {
        if (errno == ENOENT) {
          fprintf(stderr, "no such file or directory\n");
        } else if (errno == EACCES) {
          fprintf(stderr, "permission denied\n");
        } else {
          fprintf(stderr, "cannot open file\n");
        }
        should_process = 0;
      }
    }
  }

  if (should_process) {
    process(file, flags);

    if (file != stdin) {
      fclose(file);
    }
  }
}

void process(FILE *file, Flags flags) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_number = 1;
  int prev_empty = 0;
  int last_char_newfile = 0;

  while ((read = getline(&line, &len, file)) != -1) {
    int empty = (read == 1 && line[0] == '\n');
    if (flags.squeeze_blank && empty) {  // s
      if (prev_empty) {
        continue;
      }
      prev_empty = 1;
    } else {
      prev_empty = 0;
    }

    print_file(line, read, &flags, &line_number);
    last_char_newfile = (line[read - 1] == '\n');
  }

  if (!last_char_newfile && (flags.E || flags.e)) {
    printf("$");
  }
  free(line);
}

void print_file(char *line, size_t len, Flags *flags, int *line_number) {
  if (flags->number ||
      (flags->number_nonblank && !(len == 1 && line[0] == '\n'))) {
    printf("%6d\t", (*line_number)++);
  }

  for (size_t i = 0; i < len; i++) {
    unsigned char c = line[i];
    int processed = 0;

    if ((flags->t || flags->T) && c == '\t') {
      printf("^I");
      processed = 1;
    }

    if ((flags->e || flags->E) && c == '\n') {
      printf("$");
    }

    if (!processed && flags->v) {
      if (c < 32 && c != '\t' && c != '\n') {
        printf("^%c", c + 64);
      } else if (c == 127) {
        printf("^?");
      }
    }

    if (!processed) {
      putchar(c);
    }
  }
}