CXX = clang++
CPPFLAGS = -I/opt/homebrew/opt/sfml@2/include
LDFLAGS = -L/opt/homebrew/opt/sfml@2/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
CXXFLAGS = -std=c++17 -Wall

TARGET = main
SRC = main.cpp

# Default target: build and run
all: $(TARGET)
	./$(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)