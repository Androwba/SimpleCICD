#ifndef GREP_S21_GREP_H_
#define GREP_S21_GREP_H_

#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S21_NULL (void *)0
#define S21_EXIT_SUCCESS 0
#define S21_EXIT_FAILURE 1

#define MAX_PATTERN_LENGTH 8192

typedef struct {
  char *patterns[MAX_PATTERN_LENGTH];
  int pattern_count;
  int ignore_case;
  int invert_match;
  int count_only;
  int list_files;
  int line_number;
  int no_filename;
  int suppress_errors;
  int only_matching;
#if defined(__linux__)
  int invert_and_only_matching;
#endif
} GrepOptions;

void Grep(const GrepOptions *options, const char *filename, int num_files);
void parse_options(int argc, char *argv[], GrepOptions *options);
void compile_patterns(const GrepOptions *options, regex_t *regex);
void print_usage(const char *prog_name);
void read_patterns_from_file(const char *filename, GrepOptions *options);
void free_patterns(GrepOptions *options);
void print_match(const char *line, const GrepOptions *options, int line_number,
                 const char *filename, int num_files);
void process_matches(FILE *file, const regex_t *regex, int pattern_count,
                     const GrepOptions *options, const char *filename,
                     int num_files);
void handle_count_only(const GrepOptions *options, int match_count,
                       const char *filename);
void handle_list_files(const GrepOptions *options, int match_count,
                       const char *filename, int list_file_printed);
int find_matches_in_line(char *line, size_t len, const regex_t *regex,
                         int pattern_count, const GrepOptions *options,
                         const char *filename, int num_files, int line_number);
FILE *open_file(const char *filename, const GrepOptions *options);
void print_count_and_filename(int match_count, const char *filename);
void print_zero_or_count(int match_count);
void print_filename_if_matches(int match_count, const char *filename);
void handle_error(const char *message, char *line, FILE *file);

#endif  // GREP_S21_GREP_H_
