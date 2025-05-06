#include <factorial.hpp>

#include <benchmark/benchmark.h>

class FactorialCalculatorBenchmark : public benchmark::Fixture
{
public:
    void SetUp(::benchmark::State&)
    {
    }

    void TearDown(::benchmark::State&)
    {
    }
};

BENCHMARK_DEFINE_F(FactorialCalculatorBenchmark, Factorial)(benchmark::State& state)
{
    const std::int64_t input = state.range(0);

    for (auto _ : state)
    {
        auto result = factorial(input);

        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK_REGISTER_F(FactorialCalculatorBenchmark, Factorial)
    ->Unit(benchmark::kMillisecond)
    ->ArgsProduct({
        benchmark::CreateRange(1'000, 200'000, /*multiplier=*/10) // Input
    });

BENCHMARK_MAIN();
