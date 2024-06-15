#include <iostream>
#include <fstream>
#include<bits/stdc++.h>

std::map<std::string, std::vector<std::vector<std::string>>> readCFG(const std::string& filename) {
    std::map<std::string, std::vector<std::vector<std::string>>> grammar;

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

std::vector<std::vector<std::unordered_set<std::string>>> cyk(const std::string& input, 
    const std::map<std::string, std::vector<std::vector<std::string>>>& grammar, const std::string& start_symbol) {
    
    int n = input.size();
    std::vector<std::vector<std::unordered_set<std::string>>> table(n, std::vector<std::unordered_set<std::string>>(n));
    for (int i = 0; i < n; ++i) {
        std::string terminal(1, input[i]);
        for (const auto entry : grammar) {
            const std::string non_terminal = entry.first;
            const std::vector<std::vector<std::string>>productions = entry.second;
            for (const auto prod : productions) {
                if (prod.size() == 1 && prod[0] == terminal) {
                    table[i][i].insert(non_terminal);
                }
            }
        }
    }

    for (int len = 2; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            for (int k = i; k < j; k++) {
                for (const auto entry : grammar) {
                    const std::string non_terminal = entry.first;
                    const std::vector<std::vector<std::string>>productions = entry.second;
                    for (const auto prod : productions) {
                        if (prod.size() == 2) {
                            const std::string A = prod[0];
                            const std::string B = prod[1];
                            if (table[i][k].count(A) && table[k + 1][j].count(B)) {
                                table[i][j].insert(non_terminal);
                            }
                        }
                    }
                }
            }
        }
    }

    return table;
}

void visualizeCYKTable(const std::vector<std::vector<std::unordered_set<std::string>>>& table, const std::string& input, const std::string& filename) {
    std::ofstream file(filename + ".dot");
    file << "digraph CYK {\n";
    file << "rankdir=TB;\n";
    file << "node [shape=plaintext];\n";
    file << "CYKTable [label=<\n";
    file << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";

    int n = input.size();
    file << "<TR><TD></TD>"; 
    for (int i = 0; i < n; ++i) {
        file << "<TD>" << input[i] << "</TD>";
    }
    file << "</TR>\n";

    for (int i = 0; i < n; ++i) {
        file << "<TR><TD>" << i + 1 << "</TD>\n"; 
        for (int j = 0; j < n; ++j) { 
            file << "<TD>";
            if(j <= i) {
                for (int x = 0; x < table[j][i].size(); x++) {
                    file << *std::next(table[j][i].begin(), x) ;
                    if (x < table[j][i].size() - 1) {
                        file << ",";
                    }
                }
            }
            file << "</TD>";
        }
        file << "</TR>\n";
    }

    file << "</TABLE>\n";
    file << ">];\n";
    file << "}\n";
    file.close();
    std::string command = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    system(command.c_str());
}

int main() {
    std::string filename = "cfg.txt"; 
    std::string input;
    std::cout << "Enter the input string: ";
    std::cin >> input;


    std::map<std::string, std::vector<std::vector<std::string>>> grammar = readCFG(filename);
    std::string start_symbol = "S"; 
    std::vector<std::vector<std::unordered_set<std::string>>> table = cyk(input, grammar, start_symbol);
    bool isParse = table[0][input.size() - 1].count(start_symbol) > 0;

    if (isParse) {
        std::cout << "Input \"" << input << "\" can be derived from the grammar." << std::endl;
    } else {
        std::cout << "Input \"" << input << "\" cannot be derived from the grammar." << std::endl;
    }
    visualizeCYKTable(table, input, "cyk_table");

    std::string command = "rm -f cyk_table.dot";
    system(command.c_str());

    return 0;
}
