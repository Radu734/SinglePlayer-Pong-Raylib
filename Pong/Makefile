# ─────────────────────────────────────────────
#   Raylib C++ Makefile (Windows / MinGW)
# ─────────────────────────────────────────────

# Project name (default = folder name)
PROJECT_NAME ?= main

# Paths
RAYLIB_PATH ?= C:/raylib/raylib
CC = g++
CFLAGS = -Wall -std=c++17 -I$(RAYLIB_PATH)/src

# Raylib libs for Windows
LDFLAGS = -L$(RAYLIB_PATH)/src \
          -lraylib \
          -lopengl32 \
          -lgdi32 \
          -lwinmm \
          -static-libgcc \
          -static-libstdc++

# Debug / Release
ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -O2
endif

# Source and object files
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

# Default target
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	del *.o *.exe 2>nul
