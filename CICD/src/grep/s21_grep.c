#include "s21_grep.h"

int main(int argc, char *argv[]) {
  int exit_status = S21_EXIT_SUCCESS;
  GrepOptions options = {.pattern_count = 0,
                         .ignore_case = 0,
                         .invert_match = 0,
                         .count_only = 0,
                         .list_files = 0,
                         .line_number = 0,
                         .no_filename = 0,
                         .suppress_errors = 0,
                         .only_matching = 0};
  if (argc < 3) {
    print_usage(argv[0]);
    exit_status = S21_EXIT_FAILURE;
  } else {
    parse_options(argc, argv, &options);
    if (options.pattern_count == 0) {
      print_usage(argv[0]);
      exit_status = S21_EXIT_FAILURE;
    } else {
      int num_files = argc - optind;
      for (int i = optind; i < argc; i++) {
        Grep(&options, argv[i], num_files);
      }
      free_patterns(&options);
    }
  }
  return exit_status;
}

void parse_options(int argc, char *argv[], GrepOptions *options) {
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsof:")) != -1) {
    switch (opt) {
      case 'e':
        options->patterns[options->pattern_count++] = strdup(optarg);
        break;
      case 'i':
        options->ignore_case = 1;
        break;
      case 'v':
        options->invert_match = 1;
#if defined(__linux__)
        options->only_matching = 0;
#endif
        break;
      case 'c':
        options->count_only = 1;
        options->only_matching = 0;
        break;
      case 'l':
        options->list_files = 1;
        options->only_matching = 0;
        break;
      case 'n':
        options->line_number = 1;
        break;
      case 'h':
        options->no_filename = 1;
        break;
      case 's':
        options->suppress_errors = 1;
        break;
      case 'o':
        if (!options->count_only && !options->list_files) {
          options->only_matching = 1;
        }
        break;
      case 'f':
        read_patterns_from_file(optarg, options);
        break;
      default:
        print_usage(argv[0]);
        exit(S21_EXIT_FAILURE);
    }
  }

#if defined(__linux__)
  if (options->invert_match && options->only_matching) {
    options->invert_and_only_matching = 1;
    options->only_matching = 0;
  }
#endif
  if (options->pattern_count == 0 && optind < argc) {
    options->patterns[options->pattern_count++] = strdup(argv[optind++]);
  }
}

FILE *open_file(const char *filename, const GrepOptions *options) {
  FILE *file = fopen(filename, "r");
  if (file == S21_NULL) {
    if (!options->suppress_errors) {
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    }
    exit(S21_EXIT_FAILURE);
  }
  return file;
}

void Grep(const GrepOptions *options, const char *filename, int num_files) {
  FILE *file = open_file(filename, options);
  regex_t regex[options->pattern_count];
  compile_patterns(options, regex);
  process_matches(file, regex, options->pattern_count, options, filename,
                  num_files);
  for (int i = 0; i < options->pattern_count; i++) {
    regfree(&regex[i]);
  }
  fclose(file);
}

void compile_patterns(const GrepOptions *options, regex_t *regex) {
  int cflags = REG_EXTENDED;
  if (options->ignore_case) {
    cflags |= REG_ICASE;
  }
  for (int i = 0; i < options->pattern_count; i++) {
    int ret = regcomp(&regex[i], options->patterns[i], cflags);
    if (ret) {
      char errbuf[BUFSIZ];
      regerror(ret, &regex[i], errbuf, sizeof(errbuf));
      fprintf(stderr, "Could not compile regex for pattern '%s': %s\n",
              options->patterns[i], errbuf);
      exit(S21_EXIT_FAILURE);
    }
  }
}

void print_match(const char *line, const GrepOptions *options, int line_number,
                 const char *filename, int num_files) {
  if (options->line_number) {
    printf("%d:", line_number);
  }
  if (!options->no_filename && num_files > 1) {
    printf("%s:", filename);
  }
  printf("%s", line);
}

void process_matches(FILE *file, const regex_t *regex, int pattern_count,
                     const GrepOptions *options, const char *filename,
                     int num_files) {
  char *line = S21_NULL;
  size_t len = 0;
  int match_count = 0;
  int line_number = 0;
  int list_file_printed = 0;
  while (getline(&line, &len, file) != -1) {
    line_number++;
    int match_found =
        find_matches_in_line(line, len, regex, pattern_count, options, filename,
                             num_files, line_number);
#if defined(__linux__)
    if (options->invert_and_only_matching) {
      continue;
    }
#endif
    if ((match_found && !options->invert_match) ||
        (!match_found && options->invert_match)) {
      if (options->only_matching && !options->invert_match) {
        continue;
      }
      match_count++;
      if (options->list_files && !list_file_printed) {
        if (!options->count_only) {
          printf("%s\n", filename);
        }
        list_file_printed = 1;
        break;
      }
      if (options->count_only) {
        continue;
      }
      print_match(line, options, line_number, filename, num_files);
    }
  }
  handle_count_only(options, match_count, filename);
  handle_list_files(options, match_count, filename, list_file_printed);
  free(line);
}

int find_matches_in_line(char *line, size_t len, const regex_t *regex,
                         int pattern_count, const GrepOptions *options,
                         const char *filename, int num_files, int line_number) {
  regmatch_t match;
  int match_found = 0;
  int matched_positions[len];
  memset(matched_positions, 0, sizeof(matched_positions));
  for (int i = 0; i < pattern_count; i++) {
    char *line_ptr = line;
    while (!regexec(&regex[i], line_ptr, 1, &match, 0)) {
      match_found = 1;
      if (options->only_matching && !options->invert_match) {
        int match_start = match.rm_so + (line_ptr - line);
        int match_end = match.rm_eo + (line_ptr - line);
        int already_printed = 0;
        for (int pos = match_start; pos < match_end; ++pos) {
          if (matched_positions[pos]) {
            already_printed = 1;
            break;
          }
        }
        if (!already_printed) {
          if (options->line_number) {
            printf("%d:", line_number);
          }
          if (!options->no_filename && num_files > 1) {
            printf("%s:", filename);
          }
          printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                 line_ptr + match.rm_so);
          for (int pos = match_start; pos < match_end; ++pos) {
            matched_positions[pos] = 1;
          }
        }
      }
      line_ptr += match.rm_eo;
      if (!options->only_matching || options->invert_match) {
        break;
      }
    }
  }
  return match_found;
}

void print_count_and_filename(int match_count, const char *filename) {
  if (match_count == 0) {
    printf("0\n");
  } else {
    printf("%d\n%s\n", match_count, filename);
  }
}

void print_zero_or_count(int match_count) { printf("%d\n", match_count); }

void print_filename_if_matches(int match_count, const char *filename) {
  if (match_count > 0) {
    printf("%s\n", filename);
  }
}

void handle_count_only(const GrepOptions *options, int match_count,
                       const char *filename) {
#if defined(__APPLE__) || (__MACH__)
  if (options->count_only) {
    if (options->list_files) {
      print_count_and_filename(match_count, filename);
    } else {
      print_zero_or_count(match_count);
    }
  }
#else
  if (options->count_only) {
    if (options->list_files) {
      print_filename_if_matches(match_count, filename);
    } else {
      print_zero_or_count(match_count);
    }
  }
#endif
}

void handle_list_files(const GrepOptions *options, int match_count,
                       const char *filename, int list_file_printed) {
#if defined(__APPLE__) || (__MACH__)
  if (options->list_files && !list_file_printed && !options->invert_match) {
    print_filename_if_matches(match_count, filename);
  }
#else
  if (options->list_files && !list_file_printed) {
    print_filename_if_matches(match_count, filename);
  }
#endif
}

void print_usage(const char *prog_name) {
  fprintf(stderr,
          "Usage: %s [-e pattern] [-i] [-v] [-c] [-l] [-n] [-h] [-s] [-f file] "
          "[pattern] [file...]\n",
          prog_name);
}

void free_patterns(GrepOptions *options) {
  for (int i = 0; i < options->pattern_count; i++) {
    free(options->patterns[i]);
  }
}

void handle_error(const char *message, char *line, FILE *file) {
  fprintf(stderr, "%s\n", message);
  if (line) free(line);
  if (file) fclose(file);
  exit(S21_EXIT_FAILURE);
}

void read_patterns_from_file(const char *filename, GrepOptions *options) {
  FILE *file = open_file(filename, options);
  char *line = S21_NULL;
  size_t len = 0;
  while (getline(&line, &len, file) != -1) {
    if (options->pattern_count >= MAX_PATTERN_LENGTH) {
      handle_error("Too many patterns in file", line, file);
    }
    line[strcspn(line, "\n")] = '\0';
    if (strlen(line) == 0) {
      options->patterns[options->pattern_count++] = strdup(".*");
    } else {
      options->patterns[options->pattern_count++] = strdup(line);
    }
  }
  free(line);
  fclose(file);
}
