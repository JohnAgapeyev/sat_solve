#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <iostream>
#include "brute.h"

void brute_force_solve(const std::vector<std::vector<int64_t>>& clause_list, const uint64_t variable_count) {
    if (variable_count >= 64) {
        std::cerr << "Too many variables\n";
        return;
    }

    std::vector<bool> solution;
    solution.reserve(variable_count);

    std::cout << "Variable count: " << variable_count << "\n";

    for (uint64_t perm = 0; perm < (1ull << variable_count); ++perm) {
        for (unsigned int i = 0; i < 64; ++i) {
            solution[i] = (perm >> i) & 1;
        }
        for (unsigned int i = 0; i < 64; ++i) {
            std::cout << solution[i];
        }
        std::cout << std::endl;
        bool satisfiable = true;
        for (const auto& clause : clause_list) {
            bool result = false;
            for (const auto term : clause) {
                if (term < 0) {
                    result |= !solution[std::abs(term - 1)];
                } else {
                    result |= solution[term - 1];
                }
            }
            if (!result) {
                //Clause does not evaluate to true
                satisfiable = false;
                break;
            }
            //Clause evaluates to true
        }
        if (satisfiable) {
            std::cout << "Found a solution\n";
            for (unsigned int i = 0; i < 64; ++i) {
                std::cout << solution[i];
            }
            std::cout << std::endl;
            return;
        }
    }
    std::cout << "No solution found\n";
}

