#include <GameOfLife/GameOfLife.h>

#include <doctest/doctest.h>

TEST_CASE("GameOfLife")
{
  GameOfLife::GameOfLife game;

  SUBCASE("Empty State")
  {
    REQUIRE_EQ(game.GetPopulation(), 0);
    REQUIRE_EQ(game.GetGeneration(), 0);
    REQUIRE_EQ(game.GetCellsChecked(), 0);
    REQUIRE_EQ(game.GetCellsRemoved(), 0);
    REQUIRE_EQ(game.GetCellsInserted(), 0);
    REQUIRE_EQ(game.GetEvolutionTime(), 0);
    REQUIRE_EQ(game.begin(), game.end());
  }

  SUBCASE("Adding")
  {
    int x = 1;
    int y = 1;

    REQUIRE_UNARY_FALSE(game.IsAlive(x, y));

    game.Add(1, 1);
    REQUIRE_UNARY(game.IsAlive(x, y));
    REQUIRE_EQ(game.GetPopulation(), 1);
    REQUIRE_NE(game.begin(), game.end());
    REQUIRE_EQ(++game.begin(), game.end());
  }

  SUBCASE("Random Inititialization")
  {
    int a = 5;
    int p = 60;

    game.RandomInit(a, p);
    REQUIRE_GT(game.GetPopulation(), 0);
    REQUIRE_NE(game.begin(), game.end());
  }

  SUBCASE("Alive cells with 2 or 3 alive neighbours stay alive")
  {
    int x = 0;
    int y = 0;

    game.Add(x, y);
    game.Add(x - 1, y - 1);
    game.Add(x - 1, y);
    game.Evolve();
    REQUIRE_UNARY(game.IsAlive(x, y));

    game.Reset();
    game.Add(x, y);
    game.Add(x - 1, y - 1);
    game.Add(x - 1, y);
    game.Add(x - 1, y + 1);
    game.Evolve();
    REQUIRE_UNARY(game.IsAlive(x, y));
  }

  SUBCASE("Alive cells with < 2 or > 3 alive neighbours die")
  {
    int x = 0;
    int y = 0;

    game.Add(x, y);
    game.Add(x - 1, y - 1);

    REQUIRE_UNARY(game.IsAlive(x, y));
    game.Evolve();
    REQUIRE_UNARY_FALSE(game.IsAlive(x, y));

    game.Reset();
    game.Add(x, y);
    game.Add(x - 1, y - 1);
    game.Add(x - 1, y);
    game.Add(x - 1, y + 1);
    game.Add(x + 1, y + 1);

    REQUIRE_UNARY(game.IsAlive(x, y));
    game.Evolve();
    REQUIRE_UNARY_FALSE(game.IsAlive(x, y));
  }

  SUBCASE("Dead cells with 3 alive neighbours become alive")
  {
    int x = 0;
    int y = 0;

    game.Add(x - 1, y - 1);
    game.Add(x - 1, y);
    game.Add(x - 1, y + 1);

    REQUIRE_UNARY_FALSE(game.IsAlive(x, y));
    game.Evolve();
    REQUIRE_UNARY(game.IsAlive(x, y));
  }

  // xxx
  // xox
  // xxx
}
