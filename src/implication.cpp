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
bool unit_propagation(const std::vector<std::vector<int32_t>>& clause_list, std::vector<decision>& variable_status, const int32_t level) noexcept {
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
#if 1
                //Unit clause, the decision is forced to set the term to true
                variable_status[std::abs(*term) - 1].value = (*term < 0) ? state::FALSE : state::TRUE;
                variable_status[std::abs(*term) - 1].chosen_arbitrarily = false;
                variable_status[std::abs(*term) - 1].decision_level = level;
                variable_set = true;
#else
                for (const auto term1 : clause) {
                    std::cout << term1 << " ";
                }
                std::cout << "\n";
                for (const auto term2 : clause) {
                    switch(variable_status[std::abs(term2) - 1].value) {
                        case state::TRUE:
                            std::cout << 1 << " ";
                            break;
                        case state::FALSE:
                            std::cout << 0 << " ";
                            break;
                        default:
                            std::cout << 'U' << " ";
                            break;
                    }
                    //std::cout << (variable_status[std::abs(term2) -1].value == state::TRUE) << " ";
                }
                std::cout << "\n";
                std::cout << "Setting " << *term << " to True through unit clause\n";
                //std::cout << variable_status[std::abs(*term) - 1].variable << "\n";

                variable_status[std::abs(*term) - 1].value = (*term < 0) ? state::FALSE : state::TRUE;
                variable_status[std::abs(*term) - 1].chosen_arbitrarily = false;
                variable_status[std::abs(*term) - 1].decision_level = level;
                variable_set = true;
#endif
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
    //std::cout << "Start level " << backtrack_level << "\n";
    std::vector<int32_t> learnt_clause;
    learnt_clause.reserve(arbitrary_choices.size());

    //I know this could be more efficient
#if 0
retry:
    for (const auto choice : arbitrary_choices) {
        if (choice.decision_level <= backtrack_level) {
            learnt_clause.push_back((-1 * (choice.value == state::FALSE)) * (choice.variable + 1));
        }
    }
    std::cout << "Decision level " << backtrack_level << "\n";

    //See if the default already exists
    if (std::find(clause_list.crbegin(), clause_list.crend(), learnt_clause) != clause_list.crend()) {
        //Make the opposite choice
        learnt_clause.back() *= -1;

        //Check if the modified exists as well
        if (std::find(clause_list.crbegin(), clause_list.crend(), learnt_clause) != clause_list.crend()) {
            //We need to go up a level and retry this
            --backtrack_level;
            learnt_clause.clear();

            if (backtrack_level < 0) {
                std::cout << "Returning -1\n";
                return -1;
            }
            goto retry;
        }
    }
    clause_list.emplace_back(std::move(learnt_clause));
#else
#if 0
    std::vector<int32_t> conflict_clause;
    for (const auto& clause : clause_list) {
        if (std::any_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::FALSE : state::TRUE);\
                    })) {
            //Clause is complete, move on
            continue;
        }
        if (std::all_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::TRUE : state::FALSE);
                    })) {
            //Conflict detected

            std::cout << "Conflict reached in here\n";

            for (const auto term1 : clause) {
                std::cout << term1 << " ";
            }
            std::cout << "\n";

            conflict_clause.assign(clause.begin(), clause.end());

            break;
            //return true;
        }
    }
    if (conflict_clause.empty()) {
        std::cerr << "How could this happen\n";
        return -1;
    }
#endif

    for (const auto choice : arbitrary_choices) {
        int32_t term = choice.variable + 1;
        if (choice.value == state::TRUE) {
            term *= -1;
        }
        //learnt_clause.push_back((-1 * (choice.value == state::TRUE)) * (choice.variable + 1));
        learnt_clause.push_back(term);

#if 0
        switch(variable_status[std::abs(((-1 * (choice.value == state::FALSE)) * (choice.variable + 1))) - 1].value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
#endif
    }

    std::cout << "Incoming list\n";
    for (const auto choice : arbitrary_choices) {
        std::cout << choice.variable << " ";
    }
    std::cout << "\n";
    for (const auto choice : arbitrary_choices) {
        std::cout << choice.decision_level << " ";
    }
    std::cout << "\n";
    for (const auto choice : arbitrary_choices) {
        switch(choice.value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
    }
    std::cout << "\n";

    //std::cout << "Got my loop element\n";

    std::cout << "Decision levels:\n";
    for (const auto term : learnt_clause) {
        std::cout << variable_status[std::abs(term) - 1].decision_level << " ";
    }
    std::cout << "\n";
    std::cout << "Learnt clause\n";
    for (const auto term : learnt_clause) {
        std::cout << term << " ";
    }
    std::cout << "\n";
    std::cout << "Clause variable status\n";
    for (const auto term : learnt_clause) {
        switch(variable_status[std::abs(term) - 1].value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
    }
    std::cout << "\n";

    auto it = std::max_element(learnt_clause.cbegin(), learnt_clause.cend(),
            [&](const auto a, const auto b){
            return variable_status[std::abs(a) - 1].decision_level < variable_status[std::abs(b) - 1].decision_level;
            });
    backtrack_level = variable_status[std::abs(*it) - 1].decision_level - 1;

    std::cout << "Chose to backtrack to level " << backtrack_level << "\n";
    for (const auto choice : arbitrary_choices) {
        std::cout << choice.variable << " ";
    }
    std::cout << "\n";
    for (const auto choice : arbitrary_choices) {
        switch(choice.value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
    }
    std::cout << "\n";

    //Remove arbitrary choices up to the backtrack level
    arbitrary_choices.erase(std::remove_if(arbitrary_choices.begin(), arbitrary_choices.end(),
                [&](const auto& d){return d.decision_level > backtrack_level;}), arbitrary_choices.end());

    //Replace all implicit choices in that range with undefined state
    std::transform(variable_status.begin(), variable_status.end(), variable_status.begin(),
            [&](auto& d){
            if (d.decision_level > backtrack_level) {
            d.decision_level = 0;
            d.value = state::UNDEFINED;
            }
            return d;
            });

    std::cout << "New choice list\n";
    for (const auto choice : arbitrary_choices) {
        std::cout << ((-1 * (choice.value == state::FALSE)) * (choice.variable + 1)) << " ";
    }
    std::cout << "\n";
    for (const auto choice : arbitrary_choices) {
        std::cout << choice.variable << " ";
    }
    std::cout << "\n";

    //Make the opposite choice at the backtrack level
    //arbitrary_choices.back().value = (arbitrary_choices.back().value == state::TRUE) ? state::FALSE : state::TRUE;

    if (std::count_if(learnt_clause.cbegin(), learnt_clause.cend(),
                [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;}) != 1) {
        std::cerr << "Result is not a unit clause somehow\n";
        auto i = std::count_if(learnt_clause.cbegin(), learnt_clause.cend(), [&](const auto term){return variable_status[std::abs(term) - 1].value == state::UNDEFINED;});
        std::cerr << i << "\n";
    }

#if 0
    for (const auto& clause : clause_list) {
        if (std::any_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::FALSE : state::TRUE);\
                    })) {
            //Clause is complete, move on
            continue;
        }
        if (std::all_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::TRUE : state::FALSE);
                    })) {
            //Conflict detected

            if (learnt_clause == clause) {
                std::cout << "Conflict was reesatblished\n";
            } else {
                std::cout << "Conflict changed but still exists?";
            }
            break;
        }
    }
#endif


    //arbitrary_choices.back().value = (arbitrary_choices.back().value == state::TRUE) ? state::FALSE : state::TRUE;

    clause_list.push_back(learnt_clause);

    std::cout << "Pre prop\n";


    unit_propagation(clause_list, variable_status, backtrack_level - 1);

    std::cout << "Post prop\n";
#if 0
    for (const auto& clause : clause_list) {
        if (std::any_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::FALSE : state::TRUE);\
                    })) {
            //Clause is complete, move on
            continue;
        }
        if (std::all_of(clause.cbegin(), clause.cend(),
                    [&](const auto term){
                    return variable_status[std::abs(term) - 1].value == ((term < 0) ? state::TRUE : state::FALSE);
                    })) {
            //Conflict detected
            if (learnt_clause == clause) {
                std::cout << "This should never happen\n";
            } else {
                std::cout << "This also should never happen\n";
            }
            break;
        }
    }
#endif

    std::cout << "New choice list AGAIN\n";
    for (const auto choice : arbitrary_choices) {
        //std::cout << ((-1 * (choice.value == state::FALSE)) * (choice.variable + 1)) << " ";
        std::cout << (choice.variable) << " ";
    }
    std::cout << "\n";
    for (const auto choice : arbitrary_choices) {
        //switch(variable_status[std::abs(((-1 * (choice.value == state::FALSE)) * (choice.variable + 1))) - 1].value) {
        switch(choice.value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
    }
    std::cout << "\n";
    for (const auto choice : variable_status) {
        //switch(variable_status[std::abs(((-1 * (choice.value == state::FALSE)) * (choice.variable + 1))) - 1].value) {
        switch(choice.value) {
            case state::TRUE:
                std::cout << 1 << " ";
                break;
            case state::FALSE:
                std::cout << 0 << " ";
                break;
            default:
                std::cout << 'U' << " ";
                break;
        }
    }
    std::cout << "\n";

    //return variable_status[std::abs(*it) - 1].decision_level;
    return backtrack_level;

#endif

    //clause_list.emplace_back(std::move(learnt_clause));

    //Go back one level
    //return arbitrary_choices.back().decision_level - 1;
    //return backtrack_level;
}

//Backtrack according to provided decision level and add the learnt clause to the clause list
void backtrack(std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept {
    //std::cout << "Backtracking\n";
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

    unit_propagation(clause_list, variable_status, level);
#else
    //Do nothing?
    ;
#endif

#endif
}

