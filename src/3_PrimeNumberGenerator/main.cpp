#include <benchmark/benchmark.h>

#include <cstdint>
#include <generator>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <vector>

class PrimeGeneratorBasic
{
public:
    using ResultType = std::uint32_t;
    std::generator<ResultType> getNext()
    {
        while (true)
        {
            if (isPrime(currValue))
            {
                co_yield currValue;
            }
            ++currValue;
        }
    }

    static bool isPrime(ResultType num)
    {
        for (ResultType i = 2; i <= num / 2; ++i)
        {
            if (num % i == 0)
            {
                return false;
            }
        }
        return true;
    }

private:
    ResultType currValue{2};
};

class PrimeGeneratorSieveOfEratosthenes
{
public:
    using ResultType = std::uint32_t;
    std::generator<ResultType> getNext()
    {
        while (true)
        {
            const auto isCurrValuePrime = !compositeToPrimes.contains(currValue);
            if (isCurrValuePrime)
            {
                co_yield currValue;
                compositeToPrimes[currValue * currValue] = {currValue};
            }
            else
            {
                for (auto prime : compositeToPrimes[currValue])
                {
                    compositeToPrimes[currValue + prime].push_back(prime);
                }
                compositeToPrimes.erase(currValue);
            }
            ++currValue;
        }
    }

private:
    ResultType currValue{2};
    std::unordered_map<ResultType, std::vector<ResultType>> compositeToPrimes;
};

class PrimeNumberGeneratorBenchmark : public benchmark::Fixture
{
public:
    void SetUp(::benchmark::State& state)
    {
    }

    void TearDown(::benchmark::State& state)
    {
    }
};

BENCHMARK_DEFINE_F(PrimeNumberGeneratorBenchmark, Basic)(benchmark::State& state)
{
    const size_t numberOfPrimes = state.range(0);

    for (auto _ : state)
    {
        PrimeGeneratorBasic generator;
        auto result = generator.getNext() | std::views::take(numberOfPrimes) | std::ranges::to<std::vector>();

        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK_REGISTER_F(PrimeNumberGeneratorBenchmark, Basic)
    ->Unit(benchmark::kMillisecond)
    ->ArgsProduct({
        benchmark::CreateRange(10, 100'000, /*multiplier=*/10) // Number of prime numbers to generate
    });

BENCHMARK_DEFINE_F(PrimeNumberGeneratorBenchmark, SieveOfEratosthenes)(benchmark::State& state)
{
    const size_t numberOfPrimes = state.range(0);

    for (auto _ : state)
    {
        PrimeGeneratorSieveOfEratosthenes generator;
        auto result = generator.getNext() | std::views::take(numberOfPrimes) | std::ranges::to<std::vector>();

        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK_REGISTER_F(PrimeNumberGeneratorBenchmark, SieveOfEratosthenes)
    ->Unit(benchmark::kMillisecond)
    ->ArgsProduct({
        benchmark::CreateRange(10, 1'000'000, /*multiplier=*/10) // Number of prime numbers to generate
    });

BENCHMARK_MAIN();
