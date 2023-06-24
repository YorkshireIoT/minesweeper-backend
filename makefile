# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -rf
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -rf
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

UNITY_ROOT=Unity

CFLAGS=-std=c11
CFLAGS += -fprofile-arcs
CFLAGS += -ftest-coverage
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
#CFLAGS += -Wno-misleading-indentation

TARGET_BASE=test
TARGET = $(TARGET_BASE)$(TARGET_EXTENSION)
SRC_FILES=$(UNITY_ROOT)/src/unity.c src/minesweeper.c  tests/test.c
INC_DIRS=-Iinclude -I$(UNITY_ROOT)/src
SYMBOLS=

all: clean default

default: $(SRC_FILES)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET)
	- ./$(TARGET)

test/test.c: tests/test.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb tests/test.c

clean:
	$(CLEANUP) $(TARGET) *.out *.o *.so *.gcno *.gcda *.gcov lcov-report gcovr-report

coverage: ## Run code coverage
	gcov src/minesweeper.c

lcov-report: coverage ## Generate lcov report
	mkdir lcov-report
	lcov --capture --directory . --output-file lcov-report/coverage.info
	genhtml lcov-report/coverage.info --output-directory lcov-report

deps: ## Install dependencies
	sudo apt update
	sudo apt-get install lcov clang-format

ci: CFLAGS += -Werror
ci: default
