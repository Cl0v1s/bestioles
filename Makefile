CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

TEST_SRC = tests/test_pet.cpp
TEST_BIN = tests/test_pet

.PHONY: test clean

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) engine/pet.h engine/caremiss.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TEST_BIN)
