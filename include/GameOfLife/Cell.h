#pragma once

#include <compare>

namespace GameOfLife {

struct Cell
{
  int x;
  int y;

  friend inline std::strong_ordering operator<=>(Cell const &,
                                                 Cell const &) = default;
};

} // namespace GameOfLife
