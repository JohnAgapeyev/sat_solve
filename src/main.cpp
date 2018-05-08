#include <cstdlib>
#include <iostream>

#include "file.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Needs a filename\n";
        return EXIT_FAILURE;
    }

    auto clause_list = read_file(argv[1]);

    std::cout << clause_list.size() << std::endl;
#if 0
    for (const auto& clause : clause_list) {
        std::cout << "Clause: ";
        for (const auto elem : clause) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
#endif

    return EXIT_SUCCESS;
}
