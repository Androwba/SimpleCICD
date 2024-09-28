#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
TEST_FILE="tests/ascii.txt"
PLATFORM=$(uname)

OUTPUT_S21="tests/s21_cat.txt"
OUTPUT_ORIGINAL="tests/cat.txt"
LOG_FILE="tests/log.txt"

run_test() {
  local test_case="$1"
  local diff_res

  echo "Running test: $test_case"
  read -ra args <<< "$test_case"
  ./s21_cat "${args[@]}" > "$OUTPUT_S21"
  cat "${args[@]}" > "$OUTPUT_ORIGINAL"
  diff_res="$(diff -s "$OUTPUT_S21" "$OUTPUT_ORIGINAL")"

  if [ "$diff_res" == "Files $OUTPUT_S21 and $OUTPUT_ORIGINAL are identical" ]; then
    (( COUNTER_SUCCESS++ ))
  else
    if [ $COUNTER_FAIL -eq 0 ]; then
      echo "" > "$LOG_FILE"
    fi
    echo "$test_case" >> "$LOG_FILE"
    (( COUNTER_FAIL++ ))
  fi
  rm "$OUTPUT_S21" "$OUTPUT_ORIGINAL"
}

skip_combination() {
  local flags=("$@")
  if [ "$PLATFORM" == "Darwin" ]; then
    for ((i = 0; i < ${#flags[@]}; i++)); do
      for ((j = i + 1; j < ${#flags[@]}; j++)); do
        if { [ "${flags[i]}" == "-b" ] && [ "${flags[j]}" == "-e" ]; } || { [ "${flags[i]}" == "-e" ] && [ "${flags[j]}" == "-b" ]; }; then
          return 0
        fi
      done
    done
  fi
  return 1
}

FLAGS=(-b -e -n -s -t -v)

for var in "${FLAGS[@]}"; do
  run_test "$var $TEST_FILE"
done

for var in "${FLAGS[@]}"; do
  for var2 in "${FLAGS[@]}"; do
    if [ "$var" != "$var2" ]; then
      if skip_combination "$var" "$var2"; then
        continue
      fi
      run_test "$var $var2 $TEST_FILE"
    fi
  done
done

for var in "${FLAGS[@]}"; do
  for var2 in "${FLAGS[@]}"; do
    for var3 in "${FLAGS[@]}"; do
      if [ "$var" != "$var2" ] && [ "$var2" != "$var3" ] && [ "$var" != "$var3" ]; then
        if skip_combination "$var" "$var2" "$var3"; then
          continue
        fi
        run_test "$var $var2 $var3 $TEST_FILE"
      fi
    done
  done
done

for var in "${FLAGS[@]}"; do
  for var2 in "${FLAGS[@]}"; do
    for var3 in "${FLAGS[@]}"; do
      for var4 in "${FLAGS[@]}"; do
        if [ "$var" != "$var2" ] && [ "$var2" != "$var3" ] && [ "$var" != "$var3" ] && [ "$var" != "$var4" ] && [ "$var2" != "$var4" ] && [ "$var3" != "$var4" ]; then
          if skip_combination "$var" "$var2" "$var3" "$var4"; then
            continue
          fi
          run_test "$var $var2 $var3 $var4 $TEST_FILE"
        fi
      done
    done
  done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"

exit "$COUNTER_FAIL"
