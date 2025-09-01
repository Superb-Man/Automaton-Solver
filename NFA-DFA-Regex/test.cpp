#include"DFA.hpp"

int run_regex(const std::string& regex, const std::vector<std::string>& inputs) {
    std::vector<Token> tokenStream = lexer(regex);
    std::shared_ptr<AstNode> root = ParseRegex(tokenStream).parse();
    NFA nfa(root);
    DFA dfa(nfa);
    dfa.minimize();

    for (int i = 0; i < inputs.size(); i++) {
        std::string input = inputs[i] == "empty" ? "" : inputs[i];
        bool result = dfa.match(input);
        std::cout << "[" << inputs[i] << " = " << (result ? "yes" : "no") << "]";
        if (i != inputs.size()-1) std::cout << ", ";
    }
    std::cout << "\n";
    return 0;
}

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <regex> <input1> [input2...]\n";
        return 1;
    }
    std::string regex = argv[1];
    std::vector<std::string> inputs(argv+2, argv+argc);
    return run_regex(regex, inputs);
}