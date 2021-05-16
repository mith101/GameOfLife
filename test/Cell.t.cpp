#include <GameOfLife/Cell.h>

#include <doctest/doctest.h>

TEST_CASE("Cell")
{
  int x = 1;
  int y = 2;
  GameOfLife::Cell const cell(x, y);

  REQUIRE_NE(GameOfLife::Cell(x + 1, y), cell);
  REQUIRE_NE(GameOfLife::Cell(x, y + 1), cell);
  REQUIRE_NE(GameOfLife::Cell(x + 1, y + 1), cell);

  REQUIRE_EQ(GameOfLife::Cell(x, y), cell);

  REQUIRE_LE(GameOfLife::Cell(x, y), cell);
  REQUIRE_LE(GameOfLife::Cell(x - 1, y), cell);
  REQUIRE_LE(GameOfLife::Cell(x, y - 1), cell);
  REQUIRE_LE(GameOfLife::Cell(x - 1, y - 1), cell);

  REQUIRE_LT(GameOfLife::Cell(x - 1, y), cell);
  REQUIRE_LT(GameOfLife::Cell(x, y - 1), cell);
  REQUIRE_LT(GameOfLife::Cell(x - 1, y - 1), cell);

  REQUIRE_GE(GameOfLife::Cell(x, y), cell);
  REQUIRE_GE(GameOfLife::Cell(x + 1, y), cell);
  REQUIRE_GE(GameOfLife::Cell(x, y + 1), cell);
  REQUIRE_GE(GameOfLife::Cell(x + 1, y + 1), cell);

  REQUIRE_GT(GameOfLife::Cell(x + 1, y), cell);
  REQUIRE_GT(GameOfLife::Cell(x, y + 1), cell);
  REQUIRE_GT(GameOfLife::Cell(x + 1, y + 1), cell);
}
