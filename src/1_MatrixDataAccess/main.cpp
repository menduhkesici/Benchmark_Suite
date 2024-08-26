#include <benchmark/benchmark.h>

template <typename T>
class Matrix2D : private std::vector<T>
{
public:
    Matrix2D(size_t width, size_t height)
        : std::vector<T>(width * height)
        , width_(width)
        , height_(height)
    {
    }

    T getElem(size_t x, size_t y) const
    {
        return std::vector<T>::at(x + y * width_);
    }

private:
    const size_t width_;
    const size_t height_;
};

class MatrixOperations : public benchmark::Fixture
{
public:
    void SetUp(::benchmark::State& state)
    {
    }

    void TearDown(::benchmark::State& state)
    {
    }

protected:
    static constexpr size_t dimension = 1 << 14;
};

BENCHMARK_DEFINE_F(MatrixOperations, PlainForLoop)(benchmark::State& state)
{
    const auto a = Matrix2D<double>(dimension, dimension);
    const auto b = std::vector<double>(dimension);
    auto output = std::vector<double>(dimension);

    for (auto _ : state)
    {
        for (size_t i = 0; i < dimension; ++i)
        {
            for (size_t j = 0; j < dimension; ++j)
            {
                output.at(i) += a.getElem(i, j) + b.at(j);
            }
        }

        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK_REGISTER_F(MatrixOperations, PlainForLoop) //
    ->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(MatrixOperations, UnrollAndJam)(benchmark::State& state)
{
    const auto a = Matrix2D<double>(dimension, dimension);
    const auto b = std::vector<double>(dimension);
    auto output = std::vector<double>(dimension);

    const size_t unrollSize = state.range(0);

    for (auto _ : state)
    {
        for (size_t i = 0; i < dimension; i += unrollSize)
        {
            for (size_t j = 0; j < dimension; ++j)
            {
                for (size_t k = 0; k < unrollSize; ++k)
                {
                    output.at(i + k) += a.getElem(i + k, j) + b.at(j);
                }
            }
        }

        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK_REGISTER_F(MatrixOperations, UnrollAndJam)
    ->Unit(benchmark::kMillisecond)
    ->ArgsProduct({
        benchmark::CreateRange(2, 1024, /*multiplier=*/2) // Unroll size
    });

BENCHMARK_DEFINE_F(MatrixOperations, LoopTiling)(benchmark::State& state)
{
    const auto a = Matrix2D<double>(dimension, dimension);
    const auto b = std::vector<double>(dimension);
    auto output = std::vector<double>(dimension);

    const size_t tileSizeX = state.range(0);
    const size_t tileSizeY = state.range(1);

    for (auto _ : state)
    {
        for (size_t i = 0; i < dimension; i += tileSizeX)
        {
            for (size_t j = 0; j < dimension; j += tileSizeY)
            {
                for (size_t k = 0; k < tileSizeX; ++k)
                {
                    for (size_t m = 0; m < tileSizeY; ++m)
                    {
                        output.at(i + k) += a.getElem(i + k, j + m) + b.at(j + m);
                    }
                }
            }
        }

        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK_REGISTER_F(MatrixOperations, LoopTiling)
    ->Unit(benchmark::kMillisecond)
    ->ArgsProduct({
        benchmark::CreateRange(2, 1024, /*multiplier=*/2), // Tile size for x dimension
        benchmark::CreateRange(2, 1024, /*multiplier=*/2), // Tile size for y dimension
    });

BENCHMARK_MAIN();
