# Makefile - build for solver.cpp
CXX := g++
SRC := src/solver.cpp
BIN := src/binary
PLOT_SCRIPT := res/plot.py
LOG_FILE := res/iterations.txt

# Flags
CXX_DEBUG_FLAGS   := -std=c++17 -g -O0 -Wall -Wextra -DLOCAL_DEBUG -fsanitize=address,undefined
CXX_RELEASE_FLAGS := -std=c++17 -O2 -Wall -Wextra

# Argumentos para a execução: make debug RUNARGS="..."
RUNARGS ?=

.PHONY: all debug release clean clean-res plot

all: debug

debug: $(SRC)
	$(CXX) $(CXX_DEBUG_FLAGS) -o $(BIN) $(SRC)
	@echo "Running ./$(BIN) $(RUNARGS)"
	./$(BIN) $(RUNARGS)

release: $(SRC)
	$(CXX) $(CXX_RELEASE_FLAGS) -o $(BIN) $(SRC)
	@echo "Running ./$(BIN) $(RUNARGS)"
	./$(BIN) $(RUNARGS)
	@$(MAKE) plot
	@open res/iterations.png

plot:
	@echo "Gerando gráfico de iterações..."
	@python3 $(PLOT_SCRIPT) $(LOG_FILE)

clean:
	-rm -f $(BIN)

clean-res:
	-rm -rf res/*.pgm res/*.mp4

