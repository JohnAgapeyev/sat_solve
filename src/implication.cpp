#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>
#include "implication.h"

//This list is needed for generation of the conflict clause that is learnt
std::vector<decision> arbitrary_choices;

//Choice list is filled at initialization with all variables, which are incrementally updated
//Conflict is determined by attempting to add a decision to the choice list that is contradicting
std::vector<decision> variable_status;

//Returns true if conflict is found, and updates variable_status with implicit decisions based on the current status
bool unit_propagation(const std::vector<std::vector<int32_t>>& clause_list, std::vector<decision>& variable_status) noexcept {
    bool variable_set = false;
    //Repeat the clause loop if a variable is set, since that can change the answer
    do {
        variable_set = false;
        for (const auto& clause : clause_list) {
            if (std::any_of(clause.cbegin(), clause.cend(),
                        [&](const auto term){return variable_status[std::abs(term) - 1].value == state::TRUE;})) {
                //Clause is complete, move on
                continue;
            }
            //I don't like the double linear search here, maybe improve later
            if (std::count_if(clause.cbegin(), clause.cend(),
                        [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;}) == 1) {
                auto term = std::find_if(clause.cbegin(), clause.cend(), [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;});
                //Unit clause, the decision is forced to set the term to true
                variable_status[std::abs(*term) - 1].value = (*term < 0) ? state::FALSE : state::TRUE;
                variable_status[std::abs(*term) - 1].chosen_arbitrarily = false;
                variable_set = true;
                continue;
            }

            if (std::all_of(clause.cbegin(), clause.cend(),
                        [&](const auto term){
                            return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::TRUE : state::FALSE);
                        })) {
                //Conflict detected
                return true;
            }
        }
    } while(variable_set);
    return false;
}

//Returns decision level to backtrack to
int32_t conflict_analysis(const std::vector<std::vector<int32_t>>& clause_list, const std::vector<decision>& variable_status) noexcept {
    return 0;
}

//Backtrack according to provided decision level and add the learnt clause to the clause list
void backtrack(std::vector<decision>& arbitrary_choices, std::vector<decision>& variable_status, std::vector<std::vector<int32_t>>& clause_list) noexcept {
    //Learnt clause can be obtained by creating a clause consisting of arbitrary decisions made up to and previously from the current level
    //I might have to add implicit choices made previously as well, not sure though
    //Explicit assignments should cover the implicit choices, at least I hope they do
    std::vector<int32_t> learnt_clause;
    for (const auto choice : arbitrary_choices) {
        learnt_clause.push_back(choice.variable);
    }
    clause_list.emplace_back(std::move(learnt_clause));

    //Handle backtracking scheme here
    //Need to backtrack to just before last made decision
    //AKA decisions 12345 would result in 123, where 4 is now the opposite value

    if (arbitrary_choices.size() < 2) {
        arbitrary_choices.clear();
    } else {
        //Delete the last 2 elements
        arbitrary_choices.pop_back();
        arbitrary_choices.pop_back();
    }
}

