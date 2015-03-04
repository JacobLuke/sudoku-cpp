#include <fstream>

#include "Sudoku.h"
#include "Solver.h"

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " path-to-file" << std::endl;
    return 1;
  }
  std::ifstream fin(argv[1]);
  if (!fin) {
    std::cerr << "Could not read file " << argv[1] << std::endl;
    return 1;
  }
  sudoku::Sudoku problem;
  fin>>problem;
  size_t num_steps;
  auto solved = sudoku::Solve(problem, num_steps);
//  std::cout << solved;
  std::cout << num_steps << std::endl;
}
