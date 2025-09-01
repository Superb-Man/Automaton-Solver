#pragma once 
#include <bits/stdc++.h>

std::unordered_map<std::string, std::vector<std::vector<std::string>>> readCFG(const std::string& filename);

std::vector<std::vector<std::unordered_set<std::string>>> cyk(const std::string& input, 
    const std::unordered_map<std::string, std::vector<std::vector<std::string>>>& grammar, const std::string& start_symbol);



std::unordered_map<std::string, std::vector<std::vector<std::string>>> readCFG(const std::string& filename) {
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return grammar;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string non_terminal, arrow;
        ss >> non_terminal >> arrow;
        std::string symbol;
        std::vector<std::string> production;
        while (ss >> symbol) {
            if (symbol == "|") {
                grammar[non_terminal].push_back(production);
                production.clear();
            } else {
                production.push_back(symbol);
            }
        }
        grammar[non_terminal].push_back(production);
    }

    file.close();
    return grammar;
}

/**
 * @brief CYK algorithm to check if a given string can be derived from a given CFG
 * Procedure
 * 1. Initialize a table of size n x n where n is the length of the input string
 * 2. Fill the table with the non-terminals that can derive the terminal symbols in the input string
 * 3. For each length of the substring, iterate over the table and check if the non-terminals can derive the substring
 * 4. If the start symbol is present in the table[0][n-1], then the input string can be derived from the grammar
 * 
 * @param input 
 * @param grammar 
 * @param start_symbol 
 * @return std::vector<std::vector<std::unordered_set<std::string>>> 
 */
 std::vector<std::vector<std::unordered_set<std::string>>> cyk(const std::string& input, 
    const std::unordered_map<std::string, std::vector<std::vector<std::string>>>& grammar, const std::string& start_symbol) {
    
    int n = input.size();
    std::vector<std::vector<std::unordered_set<std::string>>> table(n, std::vector<std::unordered_set<std::string>>(n));
    for (int i = 0; i < n; ++i) { //Fill the diagonal of the table with the non-terminals that can derive the terminal symbols
        std::string terminal(1, input[i]);
        for (const auto entry : grammar) { 
            const std::string non_terminal = entry.first;
            const std::vector<std::vector<std::string>>productions = entry.second;
            for(const auto prod : productions) {
                if (prod.size() == 1 && prod[0] == terminal) {
                    table[i][i].insert(non_terminal);
                }
            }
        }
    }


    //Fill the table with the non-terminals that can derive the terminal symbols in the input string
    //For each length of the substring, iterate over the table and check if the non-terminals can derive the substring

    for (int len = 2; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) { 
            for (int k = i; k < i + len - 1; k++) {
                for (const auto [non_terminal,productions] : grammar) { //For each non-terminal in the grammar
                    for(const auto prod : productions) {
                        if (prod.size() == 2) { //If the production is of the form A -> BC
                            if (table[i][k].count(prod[0]) && table[k + 1][i+ len -1].count(prod[1])) {
                                table[i][i + len -1].insert(non_terminal); //If the non-terminal can derive the substring, add it to the table
                            }
                        }
                    }
                }
            }
        }
    }

    return table;
}