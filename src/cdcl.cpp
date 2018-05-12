#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include "cdcl.h"
#include "implication.h"

decision pick_arbitrarily(const std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept {
    auto elem = std::find_if(variable_status.begin(), variable_status.end(), [&](const auto& status){return status.value == state::UNDEFINED;});

    elem->value = state::FALSE;
    elem->chosen_arbitrarily = true;
    elem->decision_level = level + 1;

    std::cout << "Choosing " << elem->variable + 1 << "\n";

    arbitrary_choices.push_back({elem->variable, elem->decision_level, elem->value, elem->chosen_arbitrarily});

    return {elem->variable, elem->decision_level, elem->value, elem->chosen_arbitrarily};
}

void CDCL_solve(std::vector<std::vector<int32_t>>& clause_list) noexcept {
    int32_t decision_level = 0;
    if (unit_propagation(clause_list, -1)) {
        //Conflict detected at start
        std::cout << "Equation is UNSATISFIABLE\n";
        return;
    }
    //Run until no variables are undefined
    while (std::count_if(variable_status.cbegin(), variable_status.cend(),
                [&](const auto& status){return status.value == state::UNDEFINED;}) > 0) {

        auto d = pick_arbitrarily(clause_list, decision_level);

        ++decision_level;

        if (unit_propagation(clause_list, decision_level)) {
            std::cout << "Conflict reached\n";
            const auto level = conflict_analysis(clause_list);

            //Need to backtrack to before start
            if (level < 0) {
                std::cout << "Equation is UNSATISFIABLE\n";
                return;
            }

            backtrack(clause_list, level);
        }
    }
    std::cout << "Equation is SATISFIABLE\n";

    std::cout << "Solution:\n";
    for (const auto& dec : variable_status) {
        if (dec.value == state::TRUE) {
            std::cout << dec.variable + 1 << " ";
        } else if (dec.value == state::FALSE) {
            std::cout << -(dec.variable + 1) << " ";
        } else {
            std::cerr << "Variable " << dec.variable << " is still undefined\n";
            return;
        }
    }
    std::cout << std::endl;
}

