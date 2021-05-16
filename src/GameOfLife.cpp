#include <GameOfLife/GameOfLife.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <forward_list>
#include <functional>
#include <future>

namespace GameOfLife {

inline static void check(Cell const &key,
                         std::set<Cell> const &cells,
                         uint8_t &aliveNeigboures,
                         std::set<Cell> &cellsToCheck)
{
  if (cells.contains(key)) {
    ++aliveNeigboures;
  } else {
    cellsToCheck.insert(key);
  }
}

struct TaskResult
{
  uint32_t cellsChecked;
  std::forward_list<GameOfLife::iterator> cellsToRemove;
  std::forward_list<Cell> cellsToInsert;
};

TaskResult EvolutionTask(GameOfLife::iterator begin,
                         GameOfLife::iterator end,
                         std::set<Cell> const &cells)
{
  TaskResult result;

  std::set<Cell> cellsToCheck;
  auto &cellsToRemove = result.cellsToRemove;
  for (auto it = begin; it != end; ++it) {
    uint8_t aliveNeighbours = 0;
    check(Cell(it->x - 1, it->y - 1), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x - 0, it->y - 1), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x + 1, it->y - 1), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x - 1, it->y - 0), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x + 1, it->y - 0), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x - 1, it->y + 1), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x - 0, it->y + 1), cells, aliveNeighbours, cellsToCheck);
    check(Cell(it->x + 1, it->y + 1), cells, aliveNeighbours, cellsToCheck);
    if (aliveNeighbours != 2 && aliveNeighbours != 3)
      cellsToRemove.push_front(it);
  }
  result.cellsChecked = cellsToCheck.size();

  auto &cellsToInsert = result.cellsToInsert;
  for (auto it = cellsToCheck.begin(); it != cellsToCheck.end(); ++it) {
    uint8_t aliveNeighbours = 0;
    aliveNeighbours += cells.contains({it->x - 1, it->y - 1});
    aliveNeighbours += cells.contains({it->x - 0, it->y - 1});
    aliveNeighbours += cells.contains({it->x + 1, it->y - 1});
    aliveNeighbours += cells.contains({it->x - 1, it->y - 0});
    aliveNeighbours += cells.contains({it->x + 1, it->y - 0});
    aliveNeighbours += cells.contains({it->x - 1, it->y + 1});
    aliveNeighbours += cells.contains({it->x - 0, it->y + 1});
    aliveNeighbours += cells.contains({it->x + 1, it->y + 1});
    if (aliveNeighbours == 3)
      cellsToInsert.emplace_front(it->x, it->y);
  }

  cellsToRemove.sort([](GameOfLife::iterator const &a,
                        GameOfLife::iterator const &b) { return *a < *b; });
  cellsToInsert.sort();

  return result;
}

GameOfLife::GameOfLife(uint32_t maxNbTasks) : mMaxNbTasks{maxNbTasks}
{
}

void GameOfLife::Add(int32_t x, int32_t y)
{
  mCells.emplace(x, y);
}

void GameOfLife::Evolve()
{
  auto const tStart = std::chrono::high_resolution_clock::now();

  mParallelTasks = (GetPopulation() / mMinPopulationPerTask) + 1;
  mParallelTasks = std::min(mParallelTasks, mMaxNbTasks);

  TaskResult evolution;
  if (mParallelTasks == 1) {
    evolution = EvolutionTask(begin(), end(), mCells);
  } else {
    std::function TaskFunction = EvolutionTask;
    int const cellsPerTask = GetPopulation() / mParallelTasks;
    std::forward_list<std::future<TaskResult>> results;
    std::forward_list<iterator> beginIters;
    auto it = begin();
    beginIters.push_front(it);
    for (uint32_t task = 0; task != mParallelTasks - 1; ++task) {
      int count;
      for (count = 0; count <= cellsPerTask; ++count, ++it)
        ; // empty loop
      results.push_front(std::async(std::launch::async,
                                    EvolutionTask,
                                    beginIters.front(),
                                    it,
                                    mCells));
      beginIters.push_front(it);
    }
    evolution = EvolutionTask(beginIters.front(), end(), mCells);

    evolution.cellsToRemove.sort(
        [](GameOfLife::iterator const &a, GameOfLife::iterator const &b) {
          return *a < *b;
        });
    evolution.cellsToInsert.sort();

    while (!results.empty()) {
      auto result = results.front().get();
      evolution.cellsChecked += result.cellsChecked;
      evolution.cellsToRemove.merge(
          result.cellsToRemove,
          [](GameOfLife::iterator const &a, GameOfLife::iterator const &b) {
            return *a < *b;
          });
      evolution.cellsToInsert.merge(result.cellsToInsert);
      results.pop_front();
    }
  }
  evolution.cellsToRemove.unique();
  evolution.cellsToInsert.unique();
  mCellsChecked = evolution.cellsChecked;
  auto &cellsToRemove = evolution.cellsToRemove;
  auto &cellsToInsert = evolution.cellsToInsert;

  mCellsRemoved = 0;
  for (auto &iter : cellsToRemove) {
    mCells.erase(iter);
    ++mCellsRemoved;
  }
  mCellsInserted = 0;
  for (auto const &cell : cellsToInsert) {
    Add(cell.x, cell.y);
    ++mCellsInserted;
  }

  ++mGeneration;
  auto const tEnd = std::chrono::high_resolution_clock::now();
  mEvolutionTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart)
          .count();
}

void GameOfLife::Reset()
{
  mCells.clear();
  mGeneration = 0;
  mCellsChecked = 0;
  mCellsRemoved = 0;
  mCellsInserted = 0;
  mEvolutionTime = 0;
  mParallelTasks = 0;
}

void GameOfLife::RandomInit(int32_t x1,
                            int32_t x2,
                            int32_t y1,
                            int32_t y2,
                            int32_t p)
{
  for (int32_t x = x1; x <= x2; ++x) {
    for (int32_t y = y1; y <= y2; ++y) {
      if ((rand() % 101) < p)
        Add(x, y);
    }
  }
}

void GameOfLife::RandomInit(int32_t a, int32_t p)
{
  assert(a > 0);

  RandomInit(-a, a, -a, a, p);
}

bool GameOfLife::IsAlive(int32_t x, int32_t y) const
{
  return mCells.contains(Cell(x, y));
}

uint32_t GameOfLife::GetPopulation() const
{
  return mCells.size();
}

uint32_t GameOfLife::GetGeneration() const
{
  return mGeneration;
}

uint32_t GameOfLife::GetCellsChecked() const
{
  return mCellsChecked;
}

uint32_t GameOfLife::GetCellsRemoved() const
{
  return mCellsRemoved;
}

uint32_t GameOfLife::GetCellsInserted() const
{
  return mCellsInserted;
}

uint32_t GameOfLife::GetEvolutionTime() const
{
  return mEvolutionTime;
}

uint32_t GameOfLife::GetParallelTasks() const
{
  return mParallelTasks;
}

GameOfLife::const_iterator GameOfLife::begin() const
{
  return mCells.begin();
}

GameOfLife::const_iterator GameOfLife::end() const
{
  return mCells.end();
}

GameOfLife::const_iterator GameOfLife::lower_bound(Cell const &cell) const
{
  return mCells.lower_bound(cell);
}

} // namespace GameOfLife
