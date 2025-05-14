#include <factorial.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    auto args = std::span(argv, size_t(argc));
    if (argc > 1)
    {
        std::cout << factorial(std::stoll(args[1])) << std::endl;
    }

    return 0;
}
