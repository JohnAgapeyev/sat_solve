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
    preprocess(clause_list);
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

void preprocess(std::vector<std::vector<int32_t>>& clause_list) noexcept {
    bool list_changed = false;

    do {
        list_changed = false;

        //Remove empty clauses
        clause_list.erase(std::remove_if(clause_list.begin(), clause_list.end(), [](const auto& clause){return clause.empty();}), clause_list.end());

        //Remove clauses which are already true
        clause_list.erase(std::remove_if(clause_list.begin(), clause_list.end(),
                    [&](const auto& clause){
                    return std::any_of(clause.cbegin(), clause.cend(),
                            [&](const auto term){
                            return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::FALSE : state::TRUE);
                            });
                    }
                    ), clause_list.end());

        //Set unit clauses
        for (auto& clause : clause_list) {
            //I don't like the double linear search here, maybe improve later
            if (std::count_if(clause.cbegin(), clause.cend(),
                        [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;}) == 1) {
                auto term = std::find_if(clause.cbegin(), clause.cend(), [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;});
                //Unit clause, the decision is forced to set the term to true
                variable_status[std::abs(*term) - 1].value = (*term < 0) ? state::FALSE : state::TRUE;
                variable_status[std::abs(*term) - 1].chosen_arbitrarily = false;
                variable_status[std::abs(*term) - 1].decision_level = -1;

                list_changed = true;

                //Delete the now redundant clause
                clause.clear();
            }
        }
    } while (list_changed);
}
