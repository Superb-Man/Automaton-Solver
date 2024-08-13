#include "parseRegX.hpp"
int state_id = 0;

using State = std::string;
using StateSet = std::set<State>;
using TransitionTable = std::unordered_map<State, std::unordered_map<std::string, StateSet>>;


std::unordered_map<State, int> StateToNumber(std::vector<State> states);

std::string generateState() {
    return "s" + std::to_string(state_id++);
}

class NFA {
public:
    NFA(const std::shared_ptr<AstNode>& ast) {
        construct_NFA(ast, starting_state, final_state, states);

    }

    std::unordered_map<State, std::unordered_map<std::string, std::vector<State>>> nfaStruct() {

        std::vector<State> nfa_states;
        for (const auto& state_pair : this->states) {
            nfa_states.push_back(state_pair.first);
        }

        std::unordered_map<State ,int> state_map = StateToNumber(nfa_states);
        std::unordered_map<State, std::unordered_map<std::string, std::vector<State>>> nfa_dict;
        nfa_dict["start"][""] = {starting_state};

        // Add all states to the dictionary
        
        for (const auto& state_pair : states) {
            const auto& state_name = state_pair.first;
            const auto& transitions = state_pair.second;

            // Add all transitions to the dictionary
            // If the transition is empty, use "epsilon" as the symbol
            // If the state is the final state, add "true" to the ending list
            // Otherwise, add "false"

            for (const auto& transition_pair : transitions) { 
                std::string symbol = transition_pair.first.empty() ? "epsilon" : transition_pair.first;
                for (const auto& next_state : transition_pair.second) {
                    nfa_dict[state_name][symbol].push_back(next_state);
                }
            }

            nfa_dict[state_name]["ending"].push_back(state_name == final_state ? "true" : "false");
        }

        // nfaTable(state_map);

        return nfa_dict;
    }

    void nfaTable(std::unordered_map<State ,int>& state_map) const {
        std::ofstream nfa_table("nfa_table.csv");
        nfa_table << "state,number\n";
        for (const auto& [state, number] : state_map) {
            nfa_table << state << "," << number << "\n";
        }

        nfa_table << "state,symbol,next_state\n";
        for (const auto& [state, transitions] : states) {
            for (const auto& [symbol, nextStates] : transitions) {
                for (const auto& nextState : nextStates) {
                    nfa_table << state_map[state] << "," << (symbol.empty() ? "epsilon" : symbol) << "," << state_map[nextState] << "\n";
                }
            }
        }
        nfa_table.close();
    }

    const TransitionTable& getStates() const {
        return states;
    }

    const State& getStartState() const {
        return starting_state;
    }
    const State& getFinalState() const {
        return final_state;
    }

    



private:
    State starting_state;
    State final_state;
    TransitionTable states;
    /**
     * @brief Construct a NFA from an AST
     * Process the AST recursively and construct the NFA
     * 
     * Procedure
     * 1. If the node is a literal character, create a starting state and a final state
     *   and add a transition from the starting state to the final state with the character
     * 2. If the node is a plus node, create a sub NFA for the left and right nodes
     *  and add transitions from the starting state to the left starting state
     * and from the left final state to the right starting state
     * and from the right final state to the final state
     * 3. If the node is a sequence node, create a sub NFA for the left and right nodes
     * and add transitions from the left final state to the right starting state
     * 4. If the node is an or node, create a sub NFA for the left and right nodes
     * and add transitions from the starting state to the left and right starting states
     * and from the left and right final states to the final state
     * 5. If the node is a star node, create a sub NFA for the left node
     * and add transitions from the starting state to the left starting state
     * and from the left final state to the right starting state
     * and from the right final state to the final state
     * 
     * 
     * @param node 
     * @param starting_state 
     * @param final_state 
     * @param states 
     */
    


    void construct_NFA(const std::shared_ptr<AstNode>& node, State& starting_state, State& final_state, TransitionTable& states) {
        if (auto literal_node = std::dynamic_pointer_cast<LiteralCharacterAstNode>(node)) {
            starting_state = generateState();
            final_state = generateState();
            states[starting_state][std::string(1, literal_node->ch)].insert(final_state);
            states[final_state][""].clear(); // Empty transition for final state
            return;
        }

        if (auto plus_node = std::dynamic_pointer_cast<PlusAstNode>(node)) {
            State sub_starting_state, sub_final_state;
            TransitionTable sub_states;
            construct_NFA(plus_node->left, sub_starting_state, sub_final_state, sub_states);

            starting_state = generateState();
            final_state = generateState();
            states = sub_states;
            states[starting_state][""].insert(sub_starting_state);
            states[sub_final_state][""].insert({starting_state, final_state});
            states[final_state][""].clear();
            return;
        }

        if (auto seq_node = std::dynamic_pointer_cast<SeqAstNode>(node)) {
            State left_starting_state, left_final_state;
            State right_starting_state, right_final_state;
            TransitionTable left_states, right_states;

            construct_NFA(seq_node->left, left_starting_state, left_final_state, left_states);
            construct_NFA(seq_node->right, right_starting_state, right_final_state, right_states);

            starting_state = left_starting_state;
            final_state = right_final_state;
            states = left_states;
            for (const auto& state_pair : right_states) {
                states[state_pair.first] = state_pair.second;
            }
            states[left_final_state][""].insert(right_starting_state);
            return;
        }

        if (auto or_node = std::dynamic_pointer_cast<OrAstNode>(node)) {
            State left_starting_state, left_final_state;
            State right_starting_state, right_final_state;
            TransitionTable left_states, right_states;


            construct_NFA(or_node->left, left_starting_state, left_final_state, left_states);
            construct_NFA(or_node->right, right_starting_state, right_final_state, right_states);

            starting_state = generateState();
            final_state = generateState();
            states = left_states;
            for (const auto& state_pair : right_states) {
                states[state_pair.first] = state_pair.second;
            }
            states[starting_state][""].insert({left_starting_state, right_starting_state});
            states[left_final_state][""].insert(final_state);
            states[right_final_state][""].insert(final_state);
            states[final_state][""].clear();
            return;
        }

        if (auto star_node = std::dynamic_pointer_cast<StarAstNode>(node)) {
            State sub_starting_state, sub_final_state;
            TransitionTable sub_states;
            construct_NFA(star_node->left, sub_starting_state, sub_final_state, sub_states);

            starting_state = generateState();
            final_state = generateState();
            states = sub_states;
            states[starting_state][""].insert({sub_starting_state, final_state});
            states[sub_final_state][""].insert({starting_state, final_state});
            states[final_state][""].clear();
            return;
        }

        throw std::runtime_error("Unknown AST node type");
    }
};


std::unordered_map<State, int> StateToNumber(std::vector<State> states) {
    int index = 0 ;
    std::unordered_map<State, int> state_map;
    for(const auto& state : states) {
        state_map[state] = index++;
    }

    return state_map;
}