#pragma once

#include <GameOfLife/Cell.h>

#include <cstdint>
#include <set>

namespace GameOfLife {

class GameOfLife
{
public:
  using iterator = std::set<Cell>::iterator;
  using const_iterator = std::set<Cell>::const_iterator;

  GameOfLife() = default;
  GameOfLife(uint32_t maxNbTasks);

  void Add(int32_t x, int32_t y);
  void Evolve();
  void Reset();
  void RandomInit(int32_t x1, int32_t x2, int32_t y1, int32_t y2, int32_t p);
  void RandomInit(int32_t a, int32_t p);

  bool IsAlive(int32_t x, int32_t y) const;
  uint32_t GetPopulation() const;
  uint32_t GetGeneration() const;
  uint32_t GetCellsChecked() const;
  uint32_t GetCellsRemoved() const;
  uint32_t GetCellsInserted() const;
  uint32_t GetEvolutionTime() const;
  uint32_t GetParallelTasks() const;

  const_iterator begin() const;
  const_iterator end() const;

  const_iterator lower_bound(Cell const &cell) const;

private:
  std::set<Cell> mCells;
  uint32_t const mMaxNbTasks = 4;
  uint32_t const mMinPopulationPerTask = 1000;
  uint32_t mGeneration = 0;
  uint32_t mCellsChecked = 0;
  uint32_t mCellsRemoved = 0;
  uint32_t mCellsInserted = 0;
  uint32_t mEvolutionTime = 0;
  uint32_t mParallelTasks = 0;
};

} // namespace GameOfLife
