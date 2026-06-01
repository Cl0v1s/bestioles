CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

.PHONY: test clean

test: tests/test_pet tests/test_utils
	./tests/test_pet
	./tests/test_utils

tests/test_pet: tests/test_pet.cpp engine/pet.h engine/caremiss.h
	$(CXX) $(CXXFLAGS) -o $@ $<

tests/test_utils: tests/test_utils.cpp utils/memory.h utils/string.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f tests/test_pet tests/test_utils
