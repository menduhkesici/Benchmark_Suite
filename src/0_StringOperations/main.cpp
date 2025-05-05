#include <benchmark/benchmark.h>

class StringOperations : public benchmark::Fixture
{
public:
    void SetUp(::benchmark::State& state)
    {
    }

    void TearDown(::benchmark::State& state)
    {
    }
};

BENCHMARK_F(StringOperations, EmptyStringCreate)(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto output = std::string();
        benchmark::DoNotOptimize(output);
    }
}

BENCHMARK_F(StringOperations, ShortStringCopy)(benchmark::State& state)
{
    const auto shortString = std::string("Hello world!");
    for (auto _ : state)
    {
        auto output = std::string(shortString);
        benchmark::DoNotOptimize(output);
    }
}

BENCHMARK_F(StringOperations, LongStringCopy)(benchmark::State& state)
{
    const auto longString = std::string(100000, '*');
    for (auto _ : state)
    {
        auto output = std::string(longString);
        benchmark::DoNotOptimize(output);
    }
}

BENCHMARK_MAIN();
