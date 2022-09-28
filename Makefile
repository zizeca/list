CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++20 -g 
EXEC = test
SRC = main.cpp
HRC = List.hpp

all: $(SRC) $(HRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -rf *.o $(EXEC)