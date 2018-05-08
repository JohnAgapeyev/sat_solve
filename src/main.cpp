#include <cstdlib>
#include <iostream>

#include "file.h"

int main(void) {
    std::cout << "Hello world!\n";
    auto clause_list = read_file("test.cnf");
    return EXIT_SUCCESS;
}
