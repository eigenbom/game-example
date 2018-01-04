TARGET_EXEC ?= rl

BUILD_DIR ?= ./build
HTML_DIR ?= ./html
SRC_DIRS  ?= ./src ./deps

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
SRCS_CPP := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INCS := $(shell find $(INC_DIRS) -name *.h)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP
CXXFLAGS := -std=c++14
LDFLAGS  := -std=c++14 -lstdc++

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# emscripten
$(HTML_DIR)/index.html: $(SRCS_CPP) $(INCS) em/curses.js em/shell.html
	$(MKDIR_P) html
	emcc $(SRCS_CPP) -std=c++14 -s WASM=1 -O2 $(INC_FLAGS) -o $@ --shell-file em/shell.html
	cp -f em/curses.js $(HTML_DIR)/curses.js

.PHONY: clean emscripten

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(HTML_DIR)

emscripten: $(HTML_DIR)/index.html

-include $(DEPS)

MKDIR_P ?= mkdir -p

