#ifndef CDCL_H
#define CDCL_H

#include <vector>
#include <cstdint>
#include "implication.h"

decision pick_arbitrarily(const std::vector<std::vector<int32_t>>& clause_list, const int32_t level) noexcept;

void CDCL_solve(std::vector<std::vector<int32_t>>& clause_list) noexcept;

#endif
