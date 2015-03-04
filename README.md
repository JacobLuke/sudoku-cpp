To build the solver from source:

    make sudoku

To run the solver:

    ./solver path-to-file

where path-to-file is a file containing a 9x9 sudoku grid, with empty squares represented by zeroes.

The solver is written in C++, with heavy reliance on C++11 feature (GCC 4.8 or later). The program takes in a puzzle in standard form and outputs the solved puzzle or (if early termination occurs) the original puzzle. The bulk of the logic is in Solver.cpp.
