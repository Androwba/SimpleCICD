CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_cat
SRC = s21_cat.c
OBJ = $(SRC:.c=.o)
TEST_SCRIPT = tests/test_cat.sh
VALGRIND_LOG = valgrind-report.txt
ERR_FILE = log.txt
CLANG_FORMAT = clang-format -style=google
RM = rm -f

.PHONY: all clean test leaks clang style

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

leaks: all
ifeq ($(shell uname), Linux)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --log-file=$(VALGRIND_LOG) bash $(TEST_SCRIPT)
else ifeq ($(shell uname), Darwin)
	leaks --atExit --noContent -- ./$(TARGET) -b tests/ascii.txt
endif

clang:
	$(CLANG_FORMAT) -i $(SRC) s21_cat.h

style:
	$(CLANG_FORMAT) -n -Werror $(SRC) s21_cat.h
