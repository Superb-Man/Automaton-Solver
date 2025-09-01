#include "utils.hpp"

void visualizeCYKTable(const std::vector<std::vector<std::unordered_set<std::string>>>& table, 
                                                        const std::string& input, const std::string& filename);

void visualizeFirstFollowSets(const std::unordered_map<std::string, std::unordered_set<std::string>>& first_sets, 
                                                        const std::unordered_map<std::string, std::unordered_set<std::string>>& follow_sets, 
                                                        const std::string& filename);                                                        

void visualizeFirstFollowSets(const std::unordered_map<std::string, std::unordered_set<std::string>>& first_sets, const std::unordered_map<std::string, std::unordered_set<std::string>>& follow_sets, const std::string& filename) {
    std::ofstream file(filename + ".dot");
    file << "digraph CFG {\n";
    file << "rankdir=TB;\n";
    file << "node [shape=plaintext];\n";
    file << "FirstSets [label=<\n";
    file << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";

    for (const auto& entry : first_sets) {
        file << "<TR><TD>" << entry.first << "</TD><TD>";
        for (const auto& symbol : entry.second) {
            file << symbol << ",";
        }
        file << "</TD></TR>\n";
    }

    file << "</TABLE>\n";
    file << ">];\n";
    file << "}\n";
    file.close();
    std::string command = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    system(command.c_str());
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
            if (j <= i) {
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

int main(int argc, char* argv[]) {
    std::string filename = ""; 
    std::string input;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <cfg_file> <input_string>\n";
        return 1;
    }
    else {
        filename = argv[1];
        input = argv[2];
    }

    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar = readCFG(filename);
    std::string start_symbol = "S"; 
    std::vector<std::vector<std::unordered_set<std::string>>> table = cyk(input, grammar, start_symbol);
    bool isParse = table[0][input.size() - 1].count(start_symbol) > 0;

    if(isParse) {
        std::cout << "Input \"" << input << "\" can be derived from the grammar." << std::endl;
    } 
    else{
        std::cout << "Input \"" << input << "\" cannot be derived from the grammar." << std::endl;
    }
    visualizeCYKTable(table, input, "cyk_table");

    std::string command = "rm -f cyk_table.dot";
    system(command.c_str());

    return 0;
}
