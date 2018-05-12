#ifndef IMPLICATION_H
#define IMPLICATION_H

#include <cstdint>
#include <vector>
#include <memory>

enum class state : uint8_t {
    FALSE = 0,
    TRUE = 1,
    UNDEFINED = 2
};

struct decision {
    int32_t variable;
    int32_t decision_level;
    state value;
    bool chosen_arbitrarily;
};

//This list is needed for generation of the conflict clause that is learnt
extern std::vector<decision> arbitrary_choices;

//Choice list is filled at initialization with all variables, which are incrementally updated
//Conflict is determined by attempting to add a decision to the choice list that is contradicting
extern std::vector<decision> variable_status;

//Returns true if conflict is found, and updates variable_status with implicit decisions based on the current status
bool unit_propagation(const std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept;

//Returns decision level to backtrack to
int32_t conflict_analysis(std::vector<std::vector<int32_t>>& clause_list) noexcept;

//Backtrack according to provided decision level and add the learnt clause to the clause list
void backtrack(std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept;


#if 0
template<typename T>
class implication_graph {
    struct node {
        T data;
        std::vector<std::unique_ptr<node>> connections;
    };

    std::vector<std::unique_ptr<node>> roots;

public:
    implication_graph() noexcept = default;
    implication_graph(const implication_graph& g) noexcept = default;
    implication_graph(implication_graph&& g) noexcept = default;

    ~implication_graph() noexcept = default;

    implication_graph& operator=(const implication_graph& g) noexcept = default;
    implication_graph& operator=(implication_graph&& g) noexcept = default;

    void add();
};
#endif

#endif
