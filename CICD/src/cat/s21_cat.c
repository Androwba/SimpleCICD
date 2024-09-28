#include "s21_cat.h"

int main(int argc, char *argv[]) {
  int status = S21_EXIT_SUCCESS;
  Flags flags = {0};
  if (parseArguments(argc, argv, &flags) != S21_EXIT_SUCCESS) {
    status = S21_EXIT_FAILURE;
  } else {
    for (; optind < argc; optind++) {
      if (processFile(argv[optind], &flags) != S21_EXIT_SUCCESS) {
        status = S21_EXIT_FAILURE;
        break;
      }
    }
  }
  return status;
}

int parseArguments(int argc, char *argv[], Flags *flags) {
  int status = S21_EXIT_SUCCESS;
  int option = 0;
  while ((option = getopt_long(argc, argv, "benstvET", longOptions,
                               S21_NULL)) != -1) {
    setFlag(option, flags);
    if (status == S21_EXIT_FAILURE) {
      break;
    }
  }
  return status;
}

void setFlag(int option, Flags *flags) {
  switch (option) {
    case 'b':
      flags->numberNonBlank = 1;
      flags->number = 0;
      break;
    case 'e':
      flags->showEnds = 1;
      flags->showNonPrinting = 1;
      break;
    case 'n':
      if (!flags->numberNonBlank) {
        flags->number = 1;
      }
      break;
    case 's':
      flags->squeezeBlank = 1;
      break;
    case 't':
      flags->showTabs = 1;
      flags->showNonPrinting = 1;
      break;
    case 'v':
      flags->showNonPrinting = 1;
      break;
    case 'E':
      flags->showEnds = 1;
      break;
    case 'T':
      flags->showTabs = 1;
      break;
    default:
      fprintf(stderr, "Usage: cat [-benstvET] [file ...]\n");
      exit(S21_EXIT_FAILURE);
  }
}

void printNonPrintingChar(int character) {
  if (character >= CONTROL_CHAR_MIN && character <= CONTROL_CHAR_MAX) {
    printf("^%c", character + ASCII_OFFSET);
  } else if (character == DEL_CHAR) {
    printf("^?");
  } else if (character >= EXTENDED_CTRL_CHAR_MIN &&
             character <= EXTENDED_CTRL_CHAR_MAX) {
    printf("M-^%c", character - EXTENDED_CTRL_CHAR_MIN + ASCII_OFFSET);
  } else if (character >= EXTENDED_PRINTABLE_CHAR_MIN) {
#ifdef __APPLE__
    printf("%c", character);
#else
    printf("M-%c", character - EXTENDED_CTRL_CHAR_MIN);
#endif
  } else {
    printf("%c", character);
  }
}

void printLineNumber(FileProcessingState *state, Flags *flags) {
  if (flags->number || (flags->numberNonBlank && state->currentChar != '\n')) {
    printf("%6d\t", state->lineNumber++);
  }
}

void printTabOrChar(int character, Flags *flags) {
  if (character == '\t' && flags->showTabs) {
    printf("^I");
  } else if (flags->showNonPrinting &&
             (IS_NON_PRINTABLE_CHAR(character) || character >= 128)) {
    printNonPrintingChar(character);
  } else {
    printf("%c", character);
  }
}

void printCharacter(FileProcessingState *state, Flags *flags) {
  if (state->currentChar == '\n') {
    state->consecutiveNewLines++;
    if (flags->squeezeBlank && state->consecutiveNewLines > 2) {
      return;
    }
    if (state->isNewLine) {
      printLineNumber(state, flags);
    }
    if (flags->showEnds) {
      printf("$");
    }
    printf("\n");
    state->isNewLine = 1;
  } else {
    state->consecutiveNewLines = 0;
    if (state->isNewLine) {
      printLineNumber(state, flags);
    }
    state->isNewLine = 0;
    printTabOrChar(state->currentChar, flags);
  }
}

int processFile(const char *fileName, Flags *flags) {
  int status = S21_EXIT_SUCCESS;
  FILE *fp = fopen(fileName, "r");
  if (fp == S21_NULL) {
    fprintf(stderr, "s21_cat: %s: No such file or directory\n", fileName);
    status = S21_EXIT_FAILURE;
  } else {
    FileProcessingState state = {0, 1, 1, 0};
    while ((state.currentChar = fgetc(fp)) != EOF) {
      printCharacter(&state, flags);
    }
    fclose(fp);
  }
  return status;
}
