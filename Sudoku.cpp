#include "Sudoku.h"

namespace sudoku {

std::ostream& operator<<(std::ostream& out, Square s) {
  if (s == Square::EMPTY) {
    out << ' ';
  } else {
    out << static_cast<int>(s);
  }
  return out;
}
std::istream& operator>>(std::istream& in, Square& s) {
  int i;
  in >> i;
  s = static_cast<Square>(i);
  return in;
}

std::array<std::pair<size_t, size_t>, Sudoku::SIZE>
    Sudoku::rowCoords(const size_t i) {
  std::array<std::pair<size_t, size_t>, SIZE> ret;
  for (size_t dx = 0; dx < SIZE; ++dx) {
    ret[dx] = std::make_pair(i, dx);
  }
  return ret;
}

std::array<std::pair<size_t, size_t>, Sudoku::SIZE>
    Sudoku::colCoords(const size_t j) {
  std::array<std::pair<size_t, size_t>, SIZE> ret;
  for (size_t dy = 0; dy < SIZE; ++dy) {
    ret[dy] = std::make_pair(dy, j);
  }
  return ret;
}
std::array<std::pair<size_t, size_t>, Sudoku::SIZE>
    Sudoku::squareCoords(const size_t i, const size_t j) {
  std::array<std::pair<size_t, size_t>, SIZE> ret;
  size_t startX = j / SQUARE_DIM * SQUARE_DIM;
  size_t startY = i / SQUARE_DIM * SQUARE_DIM;
  size_t idx = 0;
  for (size_t dy = 0; dy < SQUARE_DIM; ++dy) {
    for (size_t dx = 0; dx < SQUARE_DIM; ++dx) {
      ret[idx++] = std::make_pair(startY + dy, startX + dx);
    }
  }
  return ret;
}

std::array<Square, Sudoku::SIZE> Sudoku::column (const size_t i) const {
  std::array<Square, SIZE> ret;
  size_t j = 0;
  for (auto coord: colCoords(i)) {
    ret[j++] = at(coord);
  }
  return ret;
}

std::array<Square, Sudoku::SIZE> Sudoku::square (const size_t i, const size_t j) const {
  std::array<Square, SIZE> ret;
  size_t idx = 0;
  for (auto coord: squareCoords(i, j)) {
    ret[idx++] = at(coord);
  }
  return ret;
}

std::array<Square, Sudoku::SIZE> Sudoku::row(const size_t i) const {
  std::array<Square, SIZE> ret;
  size_t j = 0;
  for (auto coord: rowCoords(i)) {
    ret[j++] = at(coord);
  }
  return ret;
}

std::istream& operator>>(std::istream& in, Sudoku& s) {
  for (size_t i = 0; in && i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; in && j < Sudoku::SIZE; ++j) {
      // Assume valid input
      in >> s.at(i, j);
    }
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const Sudoku& s) {
  for (size_t i = 0; out && i < Sudoku::SIZE; ++i) {
    for (size_t j = 0; out && j < Sudoku::SIZE; ++j) {
      if (j) {
        out << ' ';
      }
      out << s.at(i, j);
    }
    out << std::endl;
  }
  return out;
}

}
