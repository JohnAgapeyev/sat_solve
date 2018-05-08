#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <iostream>
#include <openssl/bn.h>
#include "brute.h"

void brute_force_solve(const std::vector<std::vector<int64_t>>& clause_list, const uint64_t variable_count) {
    BIGNUM *solution = BN_new();
    BN_zero(solution);

    BIGNUM *max_range = BN_new();
    BN_lshift(max_range, BN_value_one(), variable_count);

    std::cout << "Variable count: " << variable_count << "\n";

    unsigned int i = 0;

    for (; BN_cmp(solution, max_range) != 1; BN_add_word(solution, 1)) {
#if 1
        if (i++ % USHRT_MAX == 0) {
            for (int i = 0; i < BN_num_bits(solution); ++i) {
                std::cout << BN_is_bit_set(solution, i);
            }
            std::cout << "\n";
        }
#endif
        bool satisfiable = true;
        for (const auto& clause : clause_list) {
            bool result = false;
            for (const auto term : clause) {
                if (term < 0) {
                    result |= !BN_is_bit_set(solution, std::abs(term - 1));
                } else {
                    result |= BN_is_bit_set(solution, term - 1);
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
            for (int i = 0; i < BN_num_bits(solution); ++i) {
                std::cout << BN_is_bit_set(solution, i);
            }
            std::cout << "\n";
            goto cleanup;
        }
    }
    std::cout << "No solution found\n";

cleanup:
    BN_free(solution);
    BN_free(max_range);
}

