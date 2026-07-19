# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -static-libgcc -static-libstdc++ -lpthread

# Include libraries via pkg-config
# This assumes you have the -dev or -devel packages installed
LIBS = $(shell pkg-config --libs libsodium miniupnpc)

# Target executable name
TARGET = ep2ep

# Main build rule
all: $(TARGET)

$(TARGET): main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(LIBS) $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# Clean up build artifacts
clean:
	rm -f *.o $(TARGET)

# Phony targets
.PHONY: all clean