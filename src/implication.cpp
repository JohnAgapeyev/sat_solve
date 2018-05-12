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
bool unit_propagation(const std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept {

    bool variable_set = false;
    //Repeat the clause loop if a variable is set, since that can change the answer
    do {
        variable_set = false;
        for (const auto& clause : clause_list) {
            if (std::any_of(clause.cbegin(), clause.cend(),
                        [&](const auto term){
                        return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::FALSE : state::TRUE);\
                        })) {
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
                variable_status[std::abs(*term) - 1].decision_level = level;
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
//This needs tweaking obviously
int32_t conflict_analysis(std::vector<std::vector<int32_t>>& clause_list) noexcept {
    int32_t backtrack_level = arbitrary_choices.back().decision_level;
    std::vector<int32_t> learnt_clause;
    learnt_clause.reserve(arbitrary_choices.size());

    //I know this could be more efficient
retry:
    for (const auto& choice : arbitrary_choices) {
        int32_t term = choice.variable + 1;
        if (choice.value == state::TRUE) {
            term *= -1;
        }
        learnt_clause.emplace_back(std::move(term));
    }
    clause_list.push_back(learnt_clause);

    //Remove arbitrary choices up to the backtrack level
    arbitrary_choices.erase(std::remove_if(arbitrary_choices.begin(), arbitrary_choices.end(),
                [&](const auto& d){return d.decision_level > backtrack_level;}), arbitrary_choices.end());

    //Replace all implicit choices in that range with undefined state
    std::transform(variable_status.begin(), variable_status.end(), variable_status.begin(),
            [backtrack_level](auto& d){
            if (d.decision_level > backtrack_level) {
            d.decision_level = 0;
            d.value = state::UNDEFINED;
            }
            return d;
            });

    if (unit_propagation(clause_list, backtrack_level)) {
        --backtrack_level;
        learnt_clause.clear();
        clause_list.pop_back();
        if (backtrack_level < 0) {
            return -1;
        }
        goto retry;
    }

    return backtrack_level;
}

//Backtrack according to provided decision level and add the learnt clause to the clause list
void backtrack(std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept {
#if 0
    std::cout << "Backtracking\n";
    //Learnt clause can be obtained by creating a clause consisting of arbitrary decisions made up to and previously from the current level
    //I might have to add implicit choices made previously as well, not sure though
    //Explicit assignments should cover the implicit choices, at least I hope they do
    std::vector<int32_t> learnt_clause;
    for (const auto choice : arbitrary_choices) {
        learnt_clause.push_back((-1 * (choice.value == state::FALSE)) * (choice.variable + 1));
        //std::cout << "Variable: " << (-1 * (choice.value == state::FALSE)) * (choice.variable + 1) << "\n";
    }

    //for (const auto term : learnt_clause) {
    //std::cout << term << " ";
    //}
    //std::cout << "\n";

    clause_list.emplace_back(std::move(learnt_clause));

    //Handle backtracking scheme here
    //Need to backtrack to just before last made decision
    //AKA decisions 12345 would result in 123, where 4 is now the opposite value

    for (const auto& d : arbitrary_choices) {
        std::cout << d.variable << ":" << (d.value == state::TRUE) << " ";
    }
    std::cout << "\n";

    if (arbitrary_choices.size() < 2) {
        arbitrary_choices.clear();
    } else {
        //Delete the last 2 elements
        arbitrary_choices.pop_back();

        //Make the opposite choice
        arbitrary_choices.back().value = (arbitrary_choices.back().value == state::TRUE) ? state::FALSE : state::TRUE;
        //arbitrary_choices.pop_back();
    }

    for (const auto& d : arbitrary_choices) {
        std::cout << d.variable << ":" << (d.value == state::TRUE) << " ";
    }
    std::cout << "\n";
#else

#if 0
    //Remove arbitrary choices up to the backtrack level
    arbitrary_choices.erase(std::remove_if(arbitrary_choices.begin(), arbitrary_choices.end(),
                [&, level](const auto& d){return d.decision_level > level;}), arbitrary_choices.end());

    //Replace all implicit choices in that range with undefined state
    std::transform(variable_status.begin(), variable_status.end(), variable_status.begin(),
            [=](auto& d){
            if (d.decision_level > level) {
            d.decision_level = 0;
            d.value = state::UNDEFINED;
            }
            return d;
            });

    //Make the opposite choice at the backtrack level
    //arbitrary_choices.back().value = (arbitrary_choices.back().value == state::TRUE) ? state::FALSE : state::TRUE;
    //unit_propagation(clause_list, variable_status, level);
#else
    ;
#endif

#endif
}

