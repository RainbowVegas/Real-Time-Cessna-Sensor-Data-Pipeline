# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Source files
SRCS = main.cpp SensorReader.cpp Logger.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = sensor_pipeline

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	-del /Q $(OBJS) $(TARGET) 2>nul
