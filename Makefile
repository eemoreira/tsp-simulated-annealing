# Makefile - build for solver.cpp
CXX := g++
SRC := src/solver.cpp
BIN := src/binary
PLOT_SCRIPT := res/plot.py
PLOT_ANALYSIS_SCRIPT := res/analyze.py

# Flags
CXX_DEBUG_FLAGS   := -std=c++17 -g -O0 -Wall -Wextra -DLOCAL_DEBUG -fsanitize=address,undefined
CXX_RELEASE_FLAGS := -std=c++17 -O2 -Wall -Wextra

# Argumentos para a execução: make debug RUNARGS="..."
RUNARGS ?=

.PHONY: all debug release clean clean-res plot

all: debug

debug:
	$(CXX) $(CXX_DEBUG_FLAGS) -o $(BIN) $(SRC)
	@echo "Running ./$(BIN) $(RUNARGS)"
	./$(BIN) $(RUNARGS)

release-51: clean-res
	$(CXX) $(CXX_RELEASE_FLAGS) -o $(BIN) $(SRC) -DFIFTY_ONE
	@echo "Running ./$(BIN) $(RUNARGS)"
	./$(BIN) $(RUNARGS)
	@$(MAKE) plot
	@open res/iterations_comparacao.png

release-100: clean-res
	$(CXX) $(CXX_RELEASE_FLAGS) -o $(BIN) $(SRC) -DONE_HUNDRED
	@echo "Running ./$(BIN) $(RUNARGS)"
	./$(BIN) $(RUNARGS)
	@$(MAKE) plot
	@open res/iterations_comparacao.png

plot:
	@echo "Gerando gráfico de iterações..."
	@python3 $(PLOT_SCRIPT)
	@python3 $(PLOT_ANALYSIS_SCRIPT)

clean:
	-rm -f $(BIN)

clean-res:
	-rm -rf res/Cooling*.txt
	-rm -rf res/results.txt

