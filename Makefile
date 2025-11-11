# --- Compiler and Flags ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -MMD -MP -Iinclude

GUROBI_VERSION = 120

# --- Gurobi ---
GUROBI_INCLUDE = -I$(GUROBI_HOME)/include/
GUROBI_LIB_DIR = -L$(GUROBI_HOME)/lib/
GUROBI_LIBS = -lgurobi_c++ -lgurobi$(GUROBI_VERSION) -lm -lpthread -ldl

CXXFLAGS += $(GUROBI_INCLUDE)
LDFLAGS = $(GUROBI_LIB_DIR)
LDLIBS = $(GUROBI_LIBS)

# --- Project Files ---
SRC_DIR = src
BUILD_DIR = build

SRCS_MAIN = $(SRC_DIR)/main.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/construction.cpp $(SRC_DIR)/local_search.cpp
OBJS_MAIN = $(SRCS_MAIN:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

EXEC_MAIN = solver

DEPS = $(OBJS_MAIN:.o=.d)

# --- Build Targets ---

.PHONY: all clean

all: $(EXEC_MAIN)

$(EXEC_MAIN): $(OBJS_MAIN)
	@echo "==> Linking $@"
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "==> Build complete: $(EXEC_MAIN)"

# Compile .cpp -> .o in build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "==> Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean
clean:
	@echo "==> Cleaning build files..."
	rm -rf $(BUILD_DIR) $(EXEC_MAIN)
	@echo "==> Done."

-include $(DEPS)
