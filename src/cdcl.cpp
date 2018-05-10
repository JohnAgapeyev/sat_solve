#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include "cdcl.h"
#include "implication.h"

decision pick_arbitrarily(const std::vector<std::vector<int32_t>>& clause_list, std::vector<decision>& arbitrary_choices, std::vector<decision>& variable_status, const int32_t level) noexcept {
    auto elem = std::find_if(variable_status.begin(), variable_status.end(), [&](const auto& status){return status.value == state::UNDEFINED;});

    std::cout << "Choosing " << elem->variable << "\n";
    std::cout << "Choosing " << (elem->value == state::UNDEFINED) << "\n";

    elem->value = state::FALSE;
    elem->chosen_arbitrarily = true;
    elem->decision_level = level + 1;


    arbitrary_choices.push_back({elem->variable, elem->decision_level, elem->value, elem->chosen_arbitrarily});

    return {elem->variable, elem->decision_level, elem->value, elem->chosen_arbitrarily};
}

void CDCL_solve(std::vector<std::vector<int32_t>>& clause_list) noexcept {
    if (unit_propagation(clause_list, variable_status, 0)) {
        //Conflict detected at start
        std::cout << "Start\n";
        std::cout << "Equation is UNSATISFIABLE\n";
        return;
    }
    int32_t decision_level = 1;
    //Run until no variables are undefined
    while (std::count_if(variable_status.cbegin(), variable_status.cend(),
                [&](const auto& status){return status.value == state::UNDEFINED;}) > 0) {

        auto d = pick_arbitrarily(clause_list, arbitrary_choices, variable_status, decision_level);

        ++decision_level;

        if (unit_propagation(clause_list, variable_status, decision_level)) {
            //std::cout << "Conflict reached\n";
            const auto level = conflict_analysis(clause_list);

            //Need to backtrack to before start
            if (level < 0) {
                std::cout << "Equation is UNSATISFIABLE\n";
                return;
            }

            std::cout << "Start: " << decision_level << "\t" << level << "\n";

            backtrack(clause_list, level);

            decision_level = level;

#if 0
            std::cout << "YES AGAIN\n";
            for (const auto choice : arbitrary_choices) {
                std::cout << ((-1 * (choice.value == state::FALSE)) * (choice.variable + 1)) << " ";
            }
            std::cout << "\n";
#endif
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

