.PHONY: clean all

all: clean sudoku

sudoku: main.cpp Sudoku.cpp Solver.cpp
	g++  $^ -o $@ --std=c++11 -O3 -g

clean:
	rm sudoku
