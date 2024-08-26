#include <benchmark/benchmark.h>
#include <omp.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class Utility
{
public:
    template <typename... Args>
    static std::string argsToString(Args&&... args)
    {
        std::ostringstream oss;
        argsToStringHelper(oss, std::forward<Args>(args)...);
        return oss.str();
    }

private:
    template <typename... Args>
    static void argsToStringHelper(std::ostringstream& oss, Args&&... args)
    {
        (oss << ... << std::forward<Args>(args));
    }
};

template <int SudokuDimension>
class SudokuMap
{
public:
    SudokuMap(std::vector<int> elements)
        : elements_(std::move(elements))
    {
        if (elements_.size() != (SudokuDimension * SudokuDimension))
        {
            throw std::runtime_error(Utility::argsToString("Number of elements in the sudoku map '", elements_.size(),
                                                           "' does not match the dimension '", SudokuDimension, "'!\n"));
        }
    }

    int getElem(size_t x, size_t y) const
    {
        return elements_.at(x + y * SudokuDimension);
    }

    void setElem(size_t x, size_t y, int i)
    {
        elements_.at(x + y * SudokuDimension) = i;
    }

    bool isCandidate(int x, int y, int value) const
    {
        // Check the row
        for (int col = 0; col < SudokuDimension; col++)
        {
            if (getElem(x, col) == value)
            {
                return false;
            }
        }

        // Check the column
        for (int row = 0; row < SudokuDimension; row++)
        {
            if (getElem(row, y) == value)
            {
                return false;
            }
        }

        // Check the subgrid
        constexpr int subgridSize = std::sqrt(SudokuDimension);
        int subgridRowStart = (x / subgridSize) * subgridSize;
        int subgridColStart = (y / subgridSize) * subgridSize;

        for (int row = subgridRowStart; row < subgridRowStart + subgridSize; row++)
        {
            for (int col = subgridColStart; col < subgridColStart + subgridSize; col++)
            {
                if (getElem(row, col) == value)
                {
                    return false;
                }
            }
        }

        // If no conflicts, return true
        return true;
    }

    void printBoard() const
    {
        int i = 0;
        for (const auto elem : elements_)
        {
            std::cout << elem << ", ";
            i++;
            if (i == SudokuDimension)
            {
                std::cout << "//" << std::endl;
                i = 0;
            }
        }
    }

private:
    std::vector<int> elements_;
};

class SudokuSolver
{
public:
    SudokuSolver(int maxParallelizationDepth)
        : maxParallelizationDepth_(maxParallelizationDepth)
    {
    }

    template <int SudokuDimension>
    std::shared_ptr<SudokuMap<SudokuDimension>> run(SudokuMap<SudokuDimension>& sudoku, int x = 0, int y = 0,
                                                    int depth = 1) const
    {
        // If x is beyond the last column, move to the next row
        if (x >= SudokuDimension)
        {
            x = 0;
            y++;
            if (y >= SudokuDimension)
            {
                // If y is also beyond the last row, the puzzle is solved
                return std::make_shared<SudokuMap<SudokuDimension>>(sudoku);
            }
        }

        // If the current cell is already filled, move to the next one
        if (sudoku.getElem(x, y) != 0)
        {
            return run(sudoku, x + 1, y, depth);
        }

        // Only use OpenMP parallelization until maximum depth to avoid creating too many tasks
        if (depth < maxParallelizationDepth_)
        {
            std::shared_ptr<SudokuMap<SudokuDimension>> solution;

#pragma omp parallel shared(solution)
            {
#pragma omp single
                {
                    // Try placing possible values
                    for (int i = 1; i <= SudokuDimension; i++)
                    {
                        if (sudoku.isCandidate(x, y, i))
                        {
#pragma omp task firstprivate(sudoku, x, y, i, depth) shared(solution)
                            {
                                sudoku.setElem(x, y, i);
                                auto subSolution = run(sudoku, x + 1, y, depth + 1);
                                if (subSolution != nullptr)
                                {
#pragma omp critical
                                    {
                                        if (!solution)
                                        {
                                            solution = std::move(subSolution);
                                        }
                                    }
                                }
                            }
                        }
                    }
#pragma omp taskwait
                }
            }

            return solution;
        }
        else
        {
            // Try placing possible values
            for (int i = 1; i <= SudokuDimension; i++)
            {
                if (sudoku.isCandidate(x, y, i))
                {
                    auto newSudoku = sudoku;
                    newSudoku.setElem(x, y, i);
                    auto subSolution = run(newSudoku, x + 1, y, depth + 1);
                    if (subSolution != nullptr)
                    {
                        return subSolution;
                    }
                }
            }

            return nullptr;
        }
    }

private:
    const int maxParallelizationDepth_{1};
};

class SudokuSolverTest : public benchmark::Fixture
{
public:
    void SetUp(::benchmark::State& state)
    {
    }

    void TearDown(::benchmark::State& state)
    {
    }

    template <int SudokuDimension>
    inline static void Run(benchmark::State& state, const SudokuMap<SudokuDimension>& inputSudokuMap)
    {
        const int numOfThreads = state.range(0);
        omp_set_num_threads(numOfThreads);

        const int maxParallelizationDepth = state.range(1);
        const auto sudokuSolver = SudokuSolver(maxParallelizationDepth);

        for (auto _ : state)
        {
            auto sudokuMap = inputSudokuMap;
            auto solution = sudokuSolver.run(sudokuMap);

            if (!solution)
                throw std::runtime_error("Solution could not be found!");

            benchmark::DoNotOptimize(*solution);
        }
    }
};

BENCHMARK_DEFINE_F(SudokuSolverTest, NullDifficulty)(benchmark::State& state)
{
    const auto sudokuMapComplete_ = SudokuMap<16>({
        3,  7,  6,  8,  5,  14, 10, 9,  13, 2,  1,  15, 11, 12, 16, 4,  //
        13, 16, 15, 10, 12, 11, 1,  2,  7,  9,  14, 4,  8,  6,  5,  3,  //
        12, 4,  14, 9,  13, 3,  6,  16, 8,  10, 5,  11, 1,  15, 2,  7,  //
        11, 5,  1,  2,  8,  15, 7,  4,  6,  3,  16, 12, 13, 10, 14, 9,  //
        10, 13, 5,  3,  15, 6,  11, 7,  2,  16, 9,  8,  14, 1,  4,  12, //
        1,  8,  9,  11, 3,  5,  2,  14, 4,  6,  12, 13, 7,  16, 15, 10, //
        14, 12, 16, 7,  4,  8,  9,  10, 3,  1,  15, 5,  2,  11, 13, 6,  //
        4,  6,  2,  15, 1,  13, 16, 12, 10, 14, 11, 7,  9,  5,  3,  8,  //
        16, 15, 7,  4,  9,  12, 8,  1,  5,  13, 6,  3,  10, 2,  11, 14, //
        9,  1,  8,  6,  16, 10, 5,  3,  11, 12, 2,  14, 4,  13, 7,  15, //
        5,  3,  12, 13, 11, 2,  14, 15, 9,  7,  4,  10, 16, 8,  6,  1,  //
        2,  10, 11, 14, 6,  7,  4,  13, 16, 15, 8,  1,  3,  9,  12, 5,  //
        6,  14, 13, 12, 2,  1,  3,  8,  15, 11, 7,  9,  5,  4,  10, 16, //
        15, 2,  4,  1,  10, 9,  13, 6,  14, 5,  3,  16, 12, 7,  8,  11, //
        8,  9,  3,  5,  7,  16, 15, 11, 12, 4,  10, 2,  6,  14, 1,  13, //
        7,  11, 10, 16, 14, 4,  12, 5,  1,  8,  13, 6,  15, 3,  9,  2,  //
    });
    Run(state, sudokuMapComplete_);
}
BENCHMARK_REGISTER_F(SudokuSolverTest, NullDifficulty)
    ->Unit(benchmark::kMicrosecond)
    ->ArgsProduct({
        {1, 4}, // Number of threads
        {1, 8}, // Maximum depth for parallelization
    });

BENCHMARK_DEFINE_F(SudokuSolverTest, EasyDifficulty)(benchmark::State& state)
{
    const auto sudokuMapEasy_ = SudokuMap<16>({
        0,  0,  6,  0,  0,  14, 10, 00, 13, 2,  0,  15, 0,  0,  0,  4,  //
        0,  16, 15, 0,  12, 0,  0,  2,  7,  9,  0,  4,  0,  0,  5,  3,  //
        12, 0,  14, 0,  13, 3,  6,  0,  0,  0,  5,  0,  1,  0,  0,  0,  //
        0,  0,  1,  2,  8,  15, 7,  4,  6,  0,  16, 12, 0,  0,  0,  9,  //
        10, 0,  5,  0,  15, 6,  11, 0,  0,  16, 9,  8,  0,  0,  4,  0,  //
        0,  8,  0,  11, 3,  0,  0,  0,  0,  0,  0,  13, 7,  16, 15, 0,  //
        0,  12, 0,  7,  0,  8,  0,  10, 0,  1,  15, 0,  2,  11, 0,  0,  //
        0,  0,  2,  15, 0,  0,  16, 0,  10, 0,  11, 7,  9,  0,  3,  8,  //
        0,  15, 0,  4,  0,  12, 0,  0,  5,  13, 6,  0,  10, 2,  0,  0,  //
        9,  1,  8,  0,  0,  0,  5,  0,  0,  12, 2,  14, 4,  0,  7,  15, //
        0,  3,  12, 0,  11, 2,  0,  15, 9,  0,  0,  10, 16, 0,  6,  1,  //
        0,  0,  11, 14, 0,  0,  0,  13, 0,  15, 0,  1,  3,  0,  12, 5,  //
        0,  0,  0,  0,  2,  1,  0,  8,  15, 11, 0,  0,  5,  4,  10, 0,  //
        0,  2,  0,  0,  0,  0,  13, 6,  14, 5,  3,  16, 0,  7,  8,  0,  //
        0,  9,  3,  0,  0,  0,  0,  11, 0,  0,  10, 0,  0,  14, 0,  13, //
        0,  0,  10, 16, 14, 0,  0,  5,  0,  0,  13, 0,  0,  0,  0,  0   //
    });
    Run(state, sudokuMapEasy_);
}
BENCHMARK_REGISTER_F(SudokuSolverTest, EasyDifficulty)
    ->Unit(benchmark::kMicrosecond)
    ->ArgsProduct({
        benchmark::CreateDenseRange(1, 16, /*step=*/1),  // Number of threads
        benchmark::CreateRange(1, 64, /*multiplier=*/2), // Maximum depth for parallelization
    });

BENCHMARK_MAIN();
