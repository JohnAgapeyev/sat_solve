#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iterator>
#include <utility>
#include <algorithm>
#include "file.h"

/*
 * Expects file in DIMACS format according to
 * http://www.satcompetition.org/2009/format-benchmarks2009.html
 * and
 * https://fairmut3x.wordpress.com/2011/07/29/cnf-conjunctive-normal-form-dimacs-format-explained/
 */
std::pair<std::vector<std::vector<int32_t>>, int32_t> read_file(const char *path) {
    std::ifstream file{path};

    bool in_comments = true;
    int32_t variables = -1;
    int32_t clauses = -1;

    std::vector<std::vector<int32_t>> clause_list;

    for (std::string line; std::getline(file, line);) {
        //Ignore comments
        if (in_comments) {
            if (line.front() != 'c') {
                //Parse initial configuration header
                if (line.front() == 'p') {
                    std::istringstream iss{line};

                    //Used to ignore the "p" and "cnf" tokens in the problem line
                    std::string tmp_1, tmp_2;
                    if (!(iss >> tmp_1 >> tmp_2 >> variables >> clauses) || variables <= 0 || clauses <= 0) {
                        std::cerr << "Error parsing CNF file Problem Line\n";
                        return {};
                    }
                }
                in_comments = false;
            }
            continue;
        }
        if (variables <= 0 || clauses <= 0) {
            std::cerr << "Problem line was not found in file\n";
            return {};
        }
        int32_t token_num;
        std::vector<int32_t> clause_tokens;

        std::istringstream iss{std::move(line)};

        clause_tokens.assign(std::istream_iterator<int32_t>(iss),
                std::istream_iterator<int32_t>());
        clause_tokens.erase(std::remove(clause_tokens.begin(), clause_tokens.end(), 0),
                clause_tokens.end());
        clause_tokens.shrink_to_fit();

        clause_list.emplace_back(std::move(clause_tokens));
    }

    return {clause_list, variables};
}
