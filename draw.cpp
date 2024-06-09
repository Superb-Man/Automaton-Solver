#include"DFA.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void drawParseTree(const std::shared_ptr<AstNode>& root, const std::string& filename) {
    std::ofstream dot_file(filename + ".dot");
    if (!dot_file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    dot_file << "digraph ParseTree {\n";
    dot_file << "    node [shape = circle];\n";

    std::stack<std::pair<std::shared_ptr<AstNode>, std::string>> stack;
    stack.push({root, "root"});

    int nodeCount = 0;
    while (!stack.empty()) {
        auto [node, label] = stack.top();
        stack.pop();

        if (!node) continue;

        std::string nodeId = "node" + std::to_string(nodeCount++);
        dot_file << "    \"" << nodeId << "\" [label=\"" << node->getLabel() << "\"];\n";
        if (label != "root") {
            dot_file << "    \"" << label << "\" -> \"" << nodeId << "\";\n";
        }

        if (auto binaryNode = std::dynamic_pointer_cast<OrAstNode>(node)) {
            stack.push({binaryNode->right, nodeId});
            stack.push({binaryNode->left, nodeId});
        } else if (auto binaryNode = std::dynamic_pointer_cast<SeqAstNode>(node)) {
            stack.push({binaryNode->right, nodeId});
            stack.push({binaryNode->left, nodeId});
        } else if (auto unaryNode = std::dynamic_pointer_cast<StarAstNode>(node)) {
            stack.push({unaryNode->left, nodeId});
        } else if (auto unaryNode = std::dynamic_pointer_cast<PlusAstNode>(node)) {
            stack.push({unaryNode->left, nodeId});
        }
    }

    dot_file << "}\n";
    dot_file.close();

    std::string command = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    system(command.c_str());
}


void draw_nfa(const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>& nfa, const std::string& filename) {
    std::ofstream dot_file(filename + ".dot");

    dot_file << "digraph NFA {\n";
    dot_file << "    rankdir=LR;\n";
    dot_file << "    size=\"8,5\"\n";
    dot_file << "    node [shape = point ]; startH\n";
    for (const auto& state : nfa) {
        if (state.first == "start") continue;
        if (state.second.at("ending")[0] == "true") {
            dot_file << "    node [shape = doublecircle]; " << state.first << ";\n";
        } else {
            dot_file << "    node [shape = circle]; " << state.first << ";\n";
        }
    }

    for (const auto& state : nfa) {
        if (state.first == "start") continue;
        for (const auto& transition : state.second) {
            if (transition.first == "ending") continue;
            for (const auto& next_state : transition.second) {
                std::string label = (transition.first == "epsilon") ? "ε" : transition.first;
                dot_file << "    " << state.first << " -> " << next_state << " [ label = \"" << label << "\" ];\n";
            }
        }
    }
    dot_file << "    startH -> " << nfa.at("start").at("").front() << ";\n";
    dot_file << "}\n";
    dot_file.close();
    std::string command = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    system(command.c_str());
}


//draw dfa

void draw_dfa(const std::unordered_map<State, std::unordered_map<std::string,State>>& dfa, const std::string& filename) {
    std::ofstream dot_file(filename + ".dot");

    dot_file << "digraph DFA {\n";
    dot_file << "    rankdir=LR;\n";
    dot_file << "    size=\"8,5\"\n";
    dot_file << "    node [shape = point ]; startH\n";
    for (const auto& state : dfa) {
        if (state.first == "start") continue;
        if (state.second.at("ending") == "true") {
            dot_file << "    node [shape = doublecircle]; " << state.first << ";\n";
        } else {
            dot_file << "    node [shape = circle]; " << state.first << ";\n";
        }
    }

    for (const auto& state : dfa) {
        if (state.first == "start") continue;
        for (const auto& transition : state.second) {
            if (transition.first == "ending") continue;
            std::string label = transition.first;
            dot_file << "    " << state.first << " -> " << transition.second << " [ label = \"" << label << "\" ];\n";
        }
    }
    dot_file << "    startH -> " << dfa.at("start").at("") << ";\n";
    dot_file << "}\n";
    dot_file.close();

    std::string command = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    system(command.c_str());
}



void save_json_NFA(const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>& nfa, const std::string& filename) {
    json nfa_json;
    for (const auto& state : nfa) {
        for (const auto& transition : state.second) {
            for (const auto& next_state : transition.second) {
                nfa_json[state.first][transition.first].push_back(next_state);
            }
        }
    }

    std::ofstream file(filename);
    file << nfa_json.dump(4);
    file.close();
}
void save_json_DFA(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& dfa, const std::string& filename) {
    json dfa_json;
    for (const auto& state : dfa) {
        for (const auto& transition : state.second) {
            dfa_json[state.first][transition.first] = transition.second;
        }
    }

    std::ofstream file(filename);
    file << dfa_json.dump(4);
    file.close();
}


void display_image(const std::string& filename) {
    std::string image_file = filename + ".png";
    std::string command = "xdg-open " + image_file;
    system(command.c_str());
}

int main() {
    while(true) {
        std::string regex ;
        std::cout << "Enter the regex : ";
        std::cin >> regex;

        auto tokenStream = lexer(regex);
        auto parser = ParseRegex(tokenStream);
        std::shared_ptr<AstNode> root = parser.parse();
        NFA nfa(root);


        drawParseTree(root, "parse_tree");
        auto nfa_dict = nfa.nfaStruct() ;
        save_json_NFA(nfa_dict, "nfa.json");

        // // Draw NFA and display
        draw_nfa(nfa_dict, "nfa");
        display_image("nfa");

        DFA dfa(nfa);
        auto dfa_dict = dfa.dfaStruct();
        draw_dfa(dfa_dict, "dfa");
        save_json_DFA(dfa_dict, "dfa.json");
        display_image("dfa");

        
        std::string command = "rm -f parse_tree.dot nfa.dot dfa.dot nfa.json dfa.json";
        system(command.c_str());
        
        
        std::string input ;
        std::cout << "Enter the input string : ";
        std::cin >> input;
        (dfa.match(input)) ? std::cout << "Matched\n" : std::cout << "Not Matched\n";
        std::cout << "\n\n\n\n\n\n" ;
    }

    return 0;
}