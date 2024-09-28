#ifndef CAT_S21_CAT_H_
#define CAT_S21_CAT_H_

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define S21_NULL (void *)0
#define S21_EXIT_SUCCESS 0
#define S21_EXIT_FAILURE 1

#define CONTROL_CHAR_MIN 0
#define CONTROL_CHAR_MAX 31
#define DEL_CHAR 127
#define EXTENDED_CTRL_CHAR_MIN 128
#define EXTENDED_CTRL_CHAR_MAX 159
#define EXTENDED_PRINTABLE_CHAR_MIN 160
#define ASCII_PRINTABLE_MIN 32
#define ASCII_PRINTABLE_MAX 126
#define ASCII_OFFSET 64
#define IS_NON_PRINTABLE_CHAR(ch) \
  (((ch) >= 0 && (ch) <= 8) || ((ch) >= 11 && (ch) <= 31) || ((ch) == 127))

typedef struct {
  int numberNonBlank;
  int showEnds;
  int number;
  int squeezeBlank;
  int showTabs;
  int showNonPrinting;
} Flags;

typedef struct {
  int currentChar;
  int isNewLine;
  int lineNumber;
  int consecutiveNewLines;
} FileProcessingState;

static struct option longOptions[] = {{"number-nonblank", no_argument, 0, 'b'},
                                      {"show-ends", no_argument, 0, 'e'},
                                      {"number", no_argument, 0, 'n'},
                                      {"squeeze-blank", no_argument, 0, 's'},
                                      {"show-tabs", no_argument, 0, 't'},
                                      {"show-nonprinting", no_argument, 0, 'v'},
                                      {0, 0, 0, 0}};

int parseArguments(int argc, char *argv[], Flags *flags);
int processFile(const char *fileName, Flags *flags);
void printCharacter(FileProcessingState *state, Flags *flags);
void printNonPrintingChar(int character);
void printLineNumber(FileProcessingState *state, Flags *flags);
void printTabOrChar(int character, Flags *flags);
void setFlag(int option, Flags *flags);

#endif  // CAT_S21_CAT_H_
