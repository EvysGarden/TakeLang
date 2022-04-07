#include "src/taker.hpp"
#include <iostream>

using std::filesystem::path;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "give me a file to run >:(" << std::endl;
        return 1;
    }

    tlg::Taker taker { path(argv[1]) };
    return 0;
}