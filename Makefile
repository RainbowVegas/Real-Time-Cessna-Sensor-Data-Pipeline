# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Ivendor/include -DGLFW_STATIC
CCFLAGS = -Wall -Wextra -Ivendor/include
LDLIBS = -Lvendor/lib -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lws2_32

# Directories
SRC_DIR = src
VENDOR_SRC_DIR = vendor/src
TARGET = sensor_pipeline

# Source files
SRC_SRCS = \
    $(SRC_DIR)/pipe_main.cpp \
    $(SRC_DIR)/SensorReader.cpp \
    $(SRC_DIR)/Logger.cpp \
    $(SRC_DIR)/AlertManager.cpp \
    $(SRC_DIR)/GUI.cpp

VENDOR_SRCS = \
    $(VENDOR_SRC_DIR)/imgui.cpp \
    $(VENDOR_SRC_DIR)/imgui_draw.cpp \
    $(VENDOR_SRC_DIR)/imgui_widgets.cpp \
    $(VENDOR_SRC_DIR)/imgui_tables.cpp \
    $(VENDOR_SRC_DIR)/imgui_demo.cpp \
    $(VENDOR_SRC_DIR)/implot.cpp \
    $(VENDOR_SRC_DIR)/implot_items.cpp \
    $(VENDOR_SRC_DIR)/implot_demo.cpp \
    $(VENDOR_SRC_DIR)/imgui_impl_glfw.cpp \
    $(VENDOR_SRC_DIR)/imgui_impl_opengl3.cpp \
    $(VENDOR_SRC_DIR)/glad.c

SRCS = $(SRC_SRCS) $(VENDOR_SRCS)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	del /Q $(subst /,\,$(OBJS)) $(TARGET) 2>nul || true