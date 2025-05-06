#include <factorial.hpp>

#include <cassert>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

constexpr std::uint64_t pow(std::uint64_t base, std::uint64_t exp)
{
    std::uint64_t result = 1;
    for (std::uint64_t i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

constexpr std::uint64_t digits_per_vector_element = 12;
constexpr std::uint64_t max_element_per_vector_element = pow(10, digits_per_vector_element);
constexpr std::int64_t max_allowed_input = std::numeric_limits<std::uint64_t>::max() / max_element_per_vector_element - 1;

std::string factorial(std::int64_t n)
{
    if (n > max_allowed_input)
    {
        throw std::runtime_error("The input exceeds the maximum allowed input!");
    }

    std::vector<std::uint64_t> result_vec(1, 1);
    for (std::int64_t i = 2; i <= n; i++)
    {
        std::uint64_t carry = 0;
        for (auto& result_vec_item : result_vec)
        {
            std::uint64_t tmp_result = result_vec_item * i + carry;
            result_vec_item = tmp_result % max_element_per_vector_element;
            carry = tmp_result / max_element_per_vector_element;
        }
        while (carry > 0)
        {
            result_vec.push_back(carry % max_element_per_vector_element);
            carry /= max_element_per_vector_element;
        }
    }

    std::ostringstream ostream;
    ostream << result_vec.back();
    for (auto it = std::rbegin(result_vec) + 1; it != std::rend(result_vec); ++it)
    {
        ostream << std::setw(digits_per_vector_element) << std::setfill('0') << *it;
    }

    return ostream.str();
}
