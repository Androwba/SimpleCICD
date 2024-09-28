.PHONY: all cat grep test test_cat test_grep clean clean_cat clean_grep leaks leaks_cat leaks_grep clang clang_cat clang_grep style style_cat style_grep

CAT_DIR = cat
GREP_DIR = grep

all: cat grep

cat:
	$(MAKE) -C $(CAT_DIR)

grep:
	$(MAKE) -C $(GREP_DIR)

test: test_cat test_grep

test_cat:
	$(MAKE) -C $(CAT_DIR) test

test_grep:
	$(MAKE) -C $(GREP_DIR) test

clean: clean_cat clean_grep

clean_cat:
	$(MAKE) -C $(CAT_DIR) clean

clean_grep:
	$(MAKE) -C $(GREP_DIR) clean

leaks: leaks_cat leaks_grep

leaks_cat:
	$(MAKE) -C $(CAT_DIR) leaks

leaks_grep:
	$(MAKE) -C $(GREP_DIR) leaks

clang: clang_cat clang_grep

clang_cat:
	$(MAKE) -C $(CAT_DIR) clang

clang_grep:
	$(MAKE) -C $(GREP_DIR) clang

style: style_cat style_grep

style_cat:
	$(MAKE) -C $(CAT_DIR) style

style_grep:
	$(MAKE) -C $(GREP_DIR) style
