#ifndef CDCL_H
#define CDCL_H

#include <vector>
#include <cstdint>
#include "implication.h"

decision pick_arbitrarily(const std::vector<std::vector<int32_t>>& clause_list, std::vector<decision>& arbitrary_choices, std::vector<decision>& variable_status, const int32_t decision_level) noexcept;

void CDCL_solve(std::vector<std::vector<int32_t>>& clause_list) noexcept;

#endif
