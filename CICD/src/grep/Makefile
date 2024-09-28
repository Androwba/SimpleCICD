CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_grep
SRC = s21_grep.c
OBJ = $(SRC:.c=.o)
TEST_SCRIPT = tests/test_grep.sh
VALGRIND_LOG = valgrind-report.txt
ERR_FILE = errors.txt
CLANG_FORMAT = clang-format -style=google
RM = rm -f

.PHONY: all clean rebuild test leaks clang style

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	bash $(TEST_SCRIPT)

clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) -f $(ERR_FILE) $(VALGRIND_LOG)

rebuild: clean all

leaks: all
ifeq ($(shell uname), Linux)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --log-file=$(VALGRIND_LOG) bash $(TEST_SCRIPT)
else ifeq ($(shell uname), Darwin)
	leaks --atExit -- ./$(TARGET) -e hello tests/pattern.txt
endif

clang:
	$(CLANG_FORMAT) -i $(SRC) s21_grep.h

style:
	$(CLANG_FORMAT) -n -Werror $(SRC) s21_grep.h
