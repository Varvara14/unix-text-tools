#include <errno.h> 
#include <getopt.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  

typedef struct {
  int number_nonblank;  
  int E;               
  int e;
  int number;         
  int squeeze_blank;  
  int T;             
  int t;
  int v;  
} Flags;

void read_file(char *filename, Flags flags);
void parse(int argc, char *argv[], Flags *flags, char ***files,
           int *file_count);
void process(FILE *file, Flags flags);
void print_file(char *line, size_t len, Flags *flags, int *line_number);
