CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++20 -g 
LDFLAGS =  -fsanitize=address

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = test

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)