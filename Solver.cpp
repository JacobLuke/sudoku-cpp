#include "Solver.h"

#include <algorithm>
#include <cassert>
#include <set>
#include <deque>
#include <vector>

namespace sudoku {

namespace {

// type such that vals[i][j][k] is truthy iff board[i][j] cannot be k
typedef std::array<std::array<std::array<char, Sudoku::SIZE>, Sudoku::SIZE>, Sudoku::SIZE> constrained_vals_t; 
// array such that arr[i][j] is the number of values that board[i][j] cannot be
typedef std::array<std::array <size_t, Sudoku::SIZE>, Sudoku::SIZE> num_constraints_t;

// Translating a Square value to an index (one-indexed => zero-indexed)
inline int intValFromSquare(Square s) {
  return static_cast<size_t>(s) - 1;
}

// Translating from an index to a square (zero-indexed => one-indexed)
inline Square squareFromIntVal(int i) {
  return static_cast<Square>(i + 1);
}

// See if any squares in an array are filled in the same
bool findDupes(const std::array<Square, Sudoku::SIZE> arr) {
  std::set<Square> seen;
  for (auto sq : arr) {
    if (sq != Square::EMPTY) {
      auto it = seen.find(sq);
      if (it != seen.end()) {
        return true;
      }
      seen.insert(sq);
    }
  }
  return false;
}

// Take a newly-set square, return the change in constraints
constrained_vals_t ConstrainVariables(const size_t i, const size_t j, const size_t val, const constrained_vals_t& constraints) {

  constrained_vals_t newly_constrained = {0};
  // first the row
  for (auto coord: Sudoku::rowCoords(i)) {
    if (constraints[coord.first][coord.second][val] == 0) {
      newly_constrained[coord.first][coord.second][val] = 1;
    }
  }
  // next the column
  for (auto coord: Sudoku::colCoords(j)) {
    if (constraints[coord.first][coord.second][val] == 0) {
      newly_constrained[coord.first][coord.second][val] = 1;
    }
  }

  // finally the square
  for (auto coord: Sudoku::squareCoords(i, j)) {    
    if(constraints[coord.first][coord.second][val] == 0) {
      newly_constrained[coord.first][coord.second][val] = 1;
    }
  }
  return newly_constrained;
}

// Undo constraint changes (for backtracking)
void UnapplyConstraints(constrained_vals_t& constraints, num_constraints_t& num_constraints, const constrained_vals_t& delta) {
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; j < Sudoku::SIZE; ++j) {
      for (size_t k = 0; k < Sudoku::SIZE; ++k) {
        if (delta[i][j][k] == 1) {
          constraints[i][j][k] = 0;
          --num_constraints[i][j];
        }
      }
    }
  }
}

// Apply new constraints from adding a square
void ApplyConstraints(constrained_vals_t& constraints, num_constraints_t& num_constraints, const constrained_vals_t& delta) {
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; j < Sudoku::SIZE; ++j) {
      for (size_t k = 0; k < Sudoku::SIZE; ++k) {
        if (delta[i][j][k] == 1) {
          constraints[i][j][k] = 1;
          ++num_constraints[i][j];
        }
      }
    }
  }
}

// Find the empty square with the most constrained value
std::pair<int, int> MostConstrained(const num_constraints_t& num_constraints, const Sudoku& board) {
  int max_i = -1, max_j = -1;
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; j < Sudoku::SIZE; ++j) {
      if (board.at(i, j) == Square::EMPTY && (
          max_i == -1 || 
          num_constraints[i][j] > num_constraints[max_i][max_j])) {
        max_i = i;
        max_j = j;
      }
    }
  }
  return std::make_pair(max_i, max_j);
}

// Make sure a board is valid (ie. no duplicates in rows, columns, or subsquares)
bool valid(const Sudoku& board, const num_constraints_t& constraints) {
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    if (findDupes(board.row(i))) {
      return false;
    }
    if (findDupes(board.column(i))) {
      return false;
    }
  }
  for (size_t i = 0, j = 0; i < Sudoku::SIZE && j < Sudoku::SIZE; i+=Sudoku::SQUARE_DIM, j+=Sudoku::SQUARE_DIM) {
    if (findDupes(board.square(i, j))) {
      return false;
    }
  }
  return true;
}

// Returns true iff all squares are filled in (does not necessitate validity)
bool solved(const Sudoku& board) {
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; j < Sudoku::SIZE; ++j) {
      if (board.at(i, j) == Square::EMPTY)
        return false;
    }
  }
  return true;
}

// Returns candidate values for a square in order of how much they increase others' constraints
std::vector<Square> LeastConstrainingValue(const size_t i, const size_t j, const constrained_vals_t& constrained_vals) {
  std::array<size_t, Sudoku::SIZE> vals = {0};
  
  // up the row
  for (auto coord : Sudoku::rowCoords(i)) {
    for (size_t val = 0; val < Sudoku::SIZE; ++val) { 
      if (!constrained_vals[coord.first][coord.second][val]) {
        ++vals[val];
      }
    }
  }
  
  // down the column
  for (auto coord : Sudoku::colCoords(j)) {
    for (size_t val = 0; val < Sudoku::SIZE; ++val) {
       if (!constrained_vals[coord.first][coord.second][val]) {
         ++vals[val];
       }
    }
  }

  // then the square
  for (auto coord : Sudoku::squareCoords(i, j)) {
    // Don't double-count rows/cols
    if (coord.first != i && coord.second != j) {
      for (size_t val = 0; val < Sudoku::SIZE; ++val) {
        if(!constrained_vals[coord.first][coord.second][val]) {
          ++vals[val];
        }
      }
    }
  }
  std::vector<size_t> indices;
  for (size_t index = 0; index < Sudoku::SIZE; ++index) {
    if (!constrained_vals[i][j][index]) {
      indices.push_back(index);
    }
  }
  std::sort(indices.begin(), indices.end(), [&vals](size_t i1, size_t i2) {
   return vals[i1] < vals[i2];
  });
  std::vector<Square> ret;
  for (auto id : indices) {
    ret.push_back(squareFromIntVal(id));
  }
  return ret;
}

// The recursive (backtracking) solution
bool solveRecursive(Sudoku& board, constrained_vals_t& constrained_values, num_constraints_t& num_constraints, size_t& numSteps) {
  // If we're given an invalid problem, something has gone wrong
  if (!valid(board, num_constraints)) {
    return false;
  }
  
  // If we're done, exit
  if (solved(board)) {
    return true;
  }
  // Get the next square to consider
  auto coord = MostConstrained(num_constraints, board);
  // Get a sorted list of candidates
  auto candidates = LeastConstrainingValue(coord.first, coord.second, constrained_values);
  // If there are no candidates, something has gone horribly wrong
  if (candidates.empty()) {
    return false;
  }
  // Consider candidates in order
  for (auto candidate : candidates) {
    board.at(coord) = candidate;
    auto delta = ConstrainVariables(coord.first, coord.second, intValFromSquare(candidate), constrained_values);
    ApplyConstraints(constrained_values, num_constraints, delta);
    if (solveRecursive(board, constrained_values, num_constraints, ++numSteps)) {
      return true;
    }
    // Backtracking
    ++numSteps;
    UnapplyConstraints(constrained_values, num_constraints, delta);
    board.at(coord) = Square::EMPTY;
  }
  return false;
}

} // Anonymous Namespace

Sudoku Solve(const Sudoku& problem, size_t& steps) {
  constrained_vals_t constrained_values = {0};
  num_constraints_t num_constraints = {0};
  
  Sudoku board = problem;

  // Initial Constraints
  for (size_t i = 0; i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; j < Sudoku::SIZE; ++j) {
      if (board.at(i, j) != Square::EMPTY) {
        auto delta = ConstrainVariables(i, j, intValFromSquare(board.at(i, j)), constrained_values);
        ApplyConstraints(constrained_values, num_constraints, delta);
      }
    } 
  }
  if (!solveRecursive(board, constrained_values, num_constraints, steps)) {
    std::cerr << "DID NOT SOLVE PROBLEM" << std::endl;
  }

  return board;
}

}
