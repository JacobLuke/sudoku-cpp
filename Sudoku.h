#pragma once
#include <array>
#include <iostream>

namespace sudoku {
// The value in a board
enum class Square : int {
 EMPTY=0,
 ONE,
 TWO,
 THREE,
 FOUR,
 FIVE,
 SIX,
 SEVEN,
 EIGHT,
 NINE,
};

std::ostream& operator<<(std::ostream& out, Square s);
std::istream& operator<<(std::istream& in, Square s);

class Sudoku {
 public:
  // Subsquare width
  static constexpr size_t SQUARE_DIM = 3;
  // Total width
  static constexpr size_t SIZE = SQUARE_DIM * SQUARE_DIM;
  // Utility accessors for "entire [row|column|subsquare]"
  static std::array<std::pair<size_t, size_t>, SIZE> rowCoords(const size_t y);
  static std::array<std::pair<size_t, size_t>, SIZE> colCoords(const size_t x);
  static std::array<std::pair<size_t, size_t>, SIZE> squareCoords(const size_t i, const size_t j);
  Sudoku() = default;
  Sudoku(const Sudoku&) = default;
  Sudoku(Sudoku&&) = default;
  ~Sudoku() = default;

  // Returns all square values in arbitrary (but fixed) order
  std::array<Square, SIZE> row (const size_t index) const;
  std::array<Square, SIZE> column (const size_t index) const ;
  std::array<Square, SIZE> square (const size_t i, const size_t j) const;

  // Accessor/mutators for squares
  inline Square& at(size_t i, size_t j) { return squares[i][j]; }
  inline Square& at(std::pair<size_t, size_t> coord) {
    return at(coord.first, coord.second);
  }
  inline const Square& at(size_t i, size_t j) const { return squares[i][j]; }
  inline const Square& at(std::pair<size_t, size_t> coord) const {
     return at(coord.first, coord.second);
  }
 private:
  Square squares[SIZE][SIZE];
};
// I/O utilities
std::ostream& operator<<(std::ostream& o, const Sudoku& s);
std::istream& operator>>(std::istream& i, Sudoku& s);

}
