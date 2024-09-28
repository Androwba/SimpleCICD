#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0

EXECUTABLE="./s21_grep"
PATTERN_FILE="tests/pattern.txt"
REGEX_FILE="tests/regex_patterns.txt"
ERR_FILE="errors.txt"

: > "$ERR_FILE"

run_test() {
  local test_case="$1"
  local description="$2"
  
  echo "$test_case"
  eval "$EXECUTABLE $test_case > s21_grep.txt"
  eval "grep $test_case > grep.txt"
  DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
  if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]; then
    (( COUNTER_SUCCESS++ ))
  else
    {
      echo "$description"
      echo "$test_case"
      echo "$DIFF_RES"
    } >> "$ERR_FILE"
    (( COUNTER_FAIL++ ))
  fi
  rm s21_grep.txt grep.txt
}

run_test "-e hello $PATTERN_FILE" "Test Case 1"
run_test "-e ^Hello $PATTERN_FILE" "Test Case 2"
run_test "-e hello -e ^Hello -e '}' -i $PATTERN_FILE" "Test Case 3"
run_test "-i hello $PATTERN_FILE" "Test Case 4"
run_test "-e hello -v $PATTERN_FILE" "Test Case 5"
run_test "-e hello -c $PATTERN_FILE" "Test Case 6"
run_test "-e hello -l $PATTERN_FILE" "Test Case 7"
run_test "-e hello -n $PATTERN_FILE" "Test Case 8"
run_test "-e hello -h $PATTERN_FILE" "Test Case 9"
run_test "-e hello -s nonexistentfile.txt" "Test Case 10"
run_test "-f $REGEX_FILE $PATTERN_FILE" "Test Case 11"
run_test "-o -e hello $PATTERN_FILE" "Test Case 12"
run_test "-o -f $REGEX_FILE $PATTERN_FILE" "Test Case 13"
run_test "-e hello -f $REGEX_FILE $PATTERN_FILE" "Test Case -e and -f"
run_test "-e hello -f $PATTERN_FILE $PATTERN_FILE" "Test Case same pattern"
run_test "-e unexisting_pattern -cl $PATTERN_FILE" "Test Case unexisting pattern"

FLAGS=("i" "v" "c" "l" "n" "h" "s" "o")
NUM_FLAGS=${#FLAGS[@]}

for ((i = 0; i < (1 << NUM_FLAGS); i++)); do
  combination=""
  for ((j = 0; j < NUM_FLAGS; j++)); do
    if ((i & (1 << j))); then
      combination+="${FLAGS[j]}"
    fi
  done
  if [ -n "$combination" ]; then
    run_test "-${combination} -e hello $PATTERN_FILE" "Test Case -${combination} with -e"
    run_test "-${combination} -f $REGEX_FILE $PATTERN_FILE" "Test Case -${combination} with -f"
    run_test "-${combination} -e hello -f $REGEX_FILE $PATTERN_FILE" "Test Case -${combination} with -e and -f"
    run_test "-${combination} hello $PATTERN_FILE" "Test Case -${combination} without -e or -f"
  fi
done

echo "SUCCESSFUL TESTS: $COUNTER_SUCCESS"
echo "FAILED TESTS: $COUNTER_FAIL"

if [ $COUNTER_FAIL -ne 0 ]; then
  echo "See $ERR_FILE for details of failed tests."
fi
