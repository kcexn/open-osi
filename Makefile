CXX = g++
AR = gcc-ar
GCC_PATH = /usr/lib/gcc/x86_64-linux-gnu/10
SRC_DIR = src
OBJ_DIR = objects
INCLUDE_DIR = /workspaces/open-osi/lib/boost/include/
BIN_DIR = bin
CXX_FLAGS = -I$(INCLUDE_DIR) \
			-std=c++17 \
			# -Wpedantic \
			-Wall \
			-Wextra
LD_FLAGS = -L/workspaces/open-osi/lib/boost/lib/ -lboost_system -lpthread
VPATH = src:objects:src/session-layer:src/session-layer/unix-domain-sockets

OBJECTS = unix-session
TARGET = open-osi

# DEBUG SETTINGS
DEBUG_CXX_FLAGS = -g -D DEBUG -Og
DEBUG_TARGET = $(addsuffix -dbg, $(addprefix $(BIN_DIR)/, $(TARGET)))
DEBUG_OBJECTS = $(addsuffix -dbg.o, $(addprefix $(OBJ_DIR)/, $(OBJECTS)))
DEBUG_TESTS = $(addsuffix -dbg.o, $(addprefix $(OBJ_DIR)/, $(TESTS)))

# SHARED LIBRARY SETTINGS
SHARED_CXX_FLAGS = -O3 \
    -D NDEBUG \
	-fpic \
	-flto=auto \
	-fuse-linker-plugin \
	-fmodulo-sched \
	-fmodulo-sched-allow-regmoves \
	-fdevirtualize-at-ltrans \
	-fsched-pressure \
	-fsched-spec-load \
	-freschedule-modulo-scheduled-loops \
	-fno-semantic-interposition \
	-fipa-pta \
	-ffat-lto-objects \
	-march=x86-64-v2
# SHARED_CXX_FLAGS = -Og -D NDEBUG -flto=auto -fuse-linker-plugin -fpic \
				-ffat-lto-objects \
				-g \
				-march=x86-64-v2
# SHARED_CXX_FLAGS = -g -Og -fpic
STATIC_AR_FLAGS = rcs --plugin $(GCC_PATH)/liblto_plugin.so
SHARED_OBJECTS = $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(OBJECTS)))
SHARED_TARGET = $(addsuffix .so, $(addprefix $(LIB_DIR)/lib, $(TARGET)))
STATIC_TARGET = $(addsuffix .a, $(addprefix $(LIB_DIR)/lib, $(TARGET)))

.PHONY: clean debug shared

$(OBJ_DIR)/%.o: %.cpp %.hpp
	$(CXX) -c $(REL_CXX_FLAGS) $(CXX_FLAGS) $< -o $@

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): main.cpp $(DEBUG_OBJECTS) $(DEBUG_TESTS)
	$(CXX) $(DEBUG_CXX_FLAGS) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

$(OBJ_DIR)/%-dbg.o: %.cpp %.hpp
	$(CXX) -c $(DEBUG_CXX_FLAGS) $(CXX_FLAGS) $< -o $@

shared: $(INCLUDE_DIR) $(LIB_DIR)

$(SHARED_TARGET): $(SHARED_OBJECTS)
	$(CXX) -shared $^ -o $@

$(STATIC_TARGET): $(SHARED_OBJECTS)
	$(AR) $(STATIC_AR_FLAGS) $@ $^

$(OBJ_DIR)/%.o: %.cpp %.hpp
	$(CXX) -c $(SHARED_CXX_FLAGS) $(CXX_FLAGS) $< -o $@

clean:
	rm -f $(OBJ_DIR)/* $(BIN_DIR)/*