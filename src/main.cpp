#include <cstdlib>
#include <iostream>

#include "file.h"
#include "brute.h"
#include "implication.h"
#include "cdcl.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Needs a filename\n";
        return EXIT_FAILURE;
    }

    auto clause_list = read_file(argv[1]);

    std::cout << clause_list.first.size() << std::endl;
#if 0
    for (const auto& clause : clause_list.first) {
        std::cout << "Clause: ";
        for (const auto elem : clause) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
#endif

    variable_status.reserve(clause_list.second);

    for (int32_t i = 0; i < clause_list.second; ++i) {
        variable_status.push_back({i, 0, state::UNDEFINED, false});
    }

    CDCL_solve(clause_list.first);

    return EXIT_SUCCESS;
}
