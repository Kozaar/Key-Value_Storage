# SHELL=/bin/sh
CC=g++
CFLAGS=-std=c++17 -Wall -Werror -Wextra

APP_SOURCE=main.cpp \
		   interface/interface.cpp \
		   controller/controller.cpp
COMMON_SOURCE=model/data.cpp \
			  model/dispatchers/dispatcher_base.cpp \
			  model/dispatchers/ttl_manager.cpp
HASH_TABLE_SOURCE=model/hash_table/hash_table.cpp
RBTREE_SOURCE=model/self_balancing_binary_search_tree/self_balancing_binary_search_tree.cpp
TEST_SOURCE=tests/main.cpp \
			tests/rbtree_tests.cpp \
			tests/hashtable_tests.cpp

COMMON_OBJ=$(COMMON_SOURCE:.cpp=.o)
HASH_TABLE_OBJ=$(HASH_TABLE_SOURCE:.cpp=.o)
RBTREE_OBJ=$(RBTREE_SOURCE:.cpp=.o)

HASH_TABLE_FLAG=-ls21_hash_table
RBTREE_FLAG=-ls21_self_balancing_binary_search_tree

TEST_FLAGS= -lgtest

OS=$(shell uname)
ifeq ($(OS), Linux)
	LDFLAGS=-pthread
else
	LDFLAGS=
endif

all: hash_table.a self_balancing_binary_search_tree.a
	$(CC) $(CFLAGS) $(LDFLAGS) $(APP_SOURCE) -L. $(HASH_TABLE_FLAG) $(RBTREE_FLAG)
	./a.out

hash_table.a: $(HASH_TABLE_OBJ) $(COMMON_OBJ)
	ar rcs libs21_hash_table.a $(HASH_TABLE_OBJ) $(COMMON_OBJ)

self_balancing_binary_search_tree.a: $(RBTREE_OBJ) $(COMMON_OBJ)
	ar rcs libs21_self_balancing_binary_search_tree.a $(RBTREE_OBJ) $(COMMON_OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

tests: $(TEST_SOURCE) $(COMMON_SOURCE) $(RBTREE_SOURCE) $(HASH_TABLE_SOURCE)
	$(CC) $(TEST_SOURCE) $(COMMON_SOURCE) $(RBTREE_SOURCE) $(HASH_TABLE_SOURCE) $(CFLAGS) $(LDFLAGS) $(TEST_FLAGS)
	./a.out

clean:
	find -name '*.o' -print0 | xargs -0 rm -f "{}"
	rm -f *.out *.clang-format *.a *.o */*.o */*/*.o *.gcda *.gcno *.info

.PHONY: all hash_table.a self_balancing_binary_search_tree.a tests clean
