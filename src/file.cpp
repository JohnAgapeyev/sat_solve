#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>
#include "file.h"

/*
 * Expects file in DIMACS format according to
 * http://www.satcompetition.org/2009/format-benchmarks2009.html
 * and
 * https://fairmut3x.wordpress.com/2011/07/29/cnf-conjunctive-normal-form-dimacs-format-explained/
 */
std::vector<std::vector<int64_t>> read_file(const char *path) {
    std::ifstream file(path);

    bool in_comments = true;
    int64_t variables = -1;
    int64_t clauses = -1;

    std::vector<std::vector<int64_t>> clause_list;

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
        std::istringstream iss{std::move(line)};

        int64_t token_num;
        std::vector<int64_t> clause_tokens;

        while (iss >> token_num) {
            if (token_num == 0) {
                break;
            }
            clause_tokens.emplace_back(std::move(token_num));
        }
        clause_list.emplace_back(std::move(clause_tokens));
    }

    return clause_list;
}
