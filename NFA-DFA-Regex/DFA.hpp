#include "NFA.hpp"
class DFA {
public:
    using DFATransitionTable = std::unordered_map<State, std::unordered_map<std::string,State>>;

    DFA() = default;

    DFA(const NFA& nfa) {
        convertFromNFA(nfa);
        // mooreMinimization();
    }

    void minimize() {
        mooreMinimization();
    }

    void setDFA(const DFATransitionTable& states, const State& start, const StateSet& acceptings) {
        this->states = states;
        this->start_state = start;
        this->accepting_states = acceptings;
    }

    /**
     * @brief Get the States object
     * 
     * @param s 
     * @return std::pair<std::unordered_map<State ,int>,std::unordered_map<State, std::unordered_map<std::string,State>>> 
     */

    std::pair<std::unordered_map<State ,int>,std::unordered_map<State, std::unordered_map<std::string,State>>> dfaStruct(int s = 0) const {
        std::vector<State> dfa_states;
        for (const auto& state_pair : this->states) { 
            dfa_states.push_back(state_pair.first); // add all states
        }
        for (const auto& state : accepting_states) {
            dfa_states.push_back(state); // add all accepting states
        }

        std::unordered_map<State ,int> state_map = StateToNumber(dfa_states);

        std::unordered_map<State, std::unordered_map<std::string,State>> dfa_dict;
        dfa_dict["start"][""] = std::to_string(state_map[start_state]);

        // Add all transitions

        for (const auto& state_pair : states) {
            const auto& state_name = state_pair.first;
            const auto& transitions = state_pair.second;

            for (const auto& transition_pair : transitions) { // For each transition in the state 
                // If the symbol is empty, set it to epsilon 
                // Add the transition to the DFA dictionary
                std::string symbol = transition_pair.first.empty() ? "epsilon" : transition_pair.first; 
                dfa_dict[std::to_string(state_map[state_name])][symbol] = std::to_string(state_map[transition_pair.second]);
            }
            dfa_dict[std::to_string(state_map[state_name])]["ending"] = accepting_states.count(state_name) ? "true" : "false";
        }

        for (const auto& state : accepting_states) {
            dfa_dict[std::to_string(state_map[state])]["ending"] = "true";
        }

        return {state_map,dfa_dict};
    }


    void dfaTable(std::unordered_map<State ,int>& state_map) const {
        std::ofstream file("DFA.csv");
        file << "State,Number\n";
        for (const auto& state : state_map) {
            file << state.first << "," << state.second << "\n";
        }

        file << "State,symbol,Next State\n";
        for (const auto& [state, transitions] : states) {
            for (const auto& [symbol, next_state] : transitions) {
                file << state << "," << symbol << "," << next_state << "\n";
            }
        }
        //start and accpting states
        file << "start," << state_map.at(start_state) << "\n";
        for (const auto& state : accepting_states) {
            file << "accepting," << state_map.at(state) << "\n";
        }

        file.close();
        
    }

    /**
     * @brief Match the input string with the DFA
     * 
     * procedure
     * 1. Start from the start state
     * 2. For each symbol in the input string, move to the next state based on the transition table
     * 3. If the input string is empty, check if the current state is an accepting state
     * 4. Return true if the current state is an accepting state, false otherwise
     * @param input 
     * @return true 
     * @return false 
     */

    bool match(const std::string& input) const {
        //for empty string
        if (input.empty()) {
            return accepting_states.find(start_state) != accepting_states.end();
        }
        std::string current_state = start_state;
        for (const char& symbol : input) {
            auto it = states.find(current_state);
            if (it != states.end() && it->second.find(std::string(1, symbol)) != it->second.end()) {
                current_state = it->second.at(std::string(1, symbol));
            } else {
                return false;
            }
        }
        return accepting_states.find(current_state) != accepting_states.end();
    }

    void csvToDFA(const std::string& filename) {
        DFATransitionTable states;
        State start_state;
        StateSet accepting_states;

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::string line, word;
        bool header = true;

        while (std::getline(file, line)) {
            if (header) {
                header = false;
                continue;
            }

            std::stringstream ss(line);
            std::vector<std::string> row;
            while (std::getline(ss, word, ',')) {
                row.push_back(word);
            }
            if (row.size() != 5) {
                throw std::runtime_error("Invalid CSV format");
            }

            State state = row[0];
            std::string symbol = row[1];
            State next_state = row[2];
            bool is_start = (row[3] == "true");
            bool is_accepting = (row[4] == "true");

            states[state][symbol] = next_state;
            if (is_start) {
                start_state = state;
            }
            if (is_accepting) {
                accepting_states.insert(state);
            }
        }
        file.close();

        setDFA(states, start_state, accepting_states);
    }
     
private:
    DFATransitionTable states;
    StateSet accepting_states;  
    State start_state;

    /**
     * @brief Epsilon closure of a state in the NFA
     * Procedure:
     * 1. Initialize an empty set to store the closure
     * 2. Initialize a stack with the state
     * 3. While the stack is not empty:
     *  a. Pop the top state from the stack
     *  b. If the state is not in the closure:
     *   i. Add the state to the closure
     *  ii. If the state has an epsilon transition, add the next state to the stack
     * 4. Return the closure
     * 
     * 
     * @param nfa 
     * @param state 
     * @return StateSet 
     */

    StateSet epsilonClosure(const TransitionTable& nfa, const State& state) const {
        
        // Initialize an empty set to store the closure
        StateSet closure;
        std::stack<State> stack;
        stack.push(state);

        // While the stack is not empty

        while (!stack.empty()) {
            State current = stack.top();
            stack.pop(); // Pop the top state from the stack

            if (closure.find(current) == closure.end()) { // If the state is not in the closure
                closure.insert(current);
                auto it = nfa.find(current);
                if (it != nfa.end() && it->second.find("") != it->second.end()) {
                    //debug
                    // std::cout << "epsilon closure" << std::endl;
                    for (const State& nextState : it->second.at("")) {
                        //debug
                        // std::cout << nextState << std::endl;

                        stack.push(nextState);
                    }
                }
            }
        }
        return closure;
    }

    StateSet epsilonClosure(const TransitionTable& nfa, const StateSet& states) const {
        StateSet closure;
        for (const State& state : states) {
            StateSet singleClosure = epsilonClosure(nfa, state);
            closure.insert(singleClosure.begin(), singleClosure.end());
        }
        return closure;
    }

    std::string stateSetToString(const StateSet& states) const {
        std::string result = "{";
        for (const auto& state : states) {
            result += state + " ";
        }
        if (!result.empty()) result.pop_back();
        result += "}";
        return result;
    }

    /**
     * @brief Convert NFA to DFA
     * Procedure:
     * 1. Start state of DFA is the epsilon closure of NFA's start state
     * 2. Mark start state as an accepting state if it contains any of the NFA's final states
     * 3. Repeat until no more states can be added to the DFA:
     *  a. For each state in the DFA, create a map of transitions to the next state
     *  b. Create a new state based on the map
     *  c. If the new state is not in the DFA, add it to the DFA
     * 4. Update DFA states and accepting states
     * 
     * @param nfa 
     */

    // Convert NFA to DFA
    void convertFromNFA(const NFA& nfa) {

        // Get NFA's states, start state, and final states
        const TransitionTable& nfaStates = nfa.getStates();
        const State& startState = nfa.getStartState();
        const StateSet& finalStates = {nfa.getFinalState()};  // Get NFA's final states

        
        std::map<StateSet, std::string> stateSetToDFAState;
        std::queue<StateSet> unmarkedStates;

        // Start state of DFA is the epsilon closure of NFA's start state
        StateSet startClosure = epsilonClosure(nfaStates, startState);
        start_state = stateSetToString(startClosure);  // Initialize start_state
        stateSetToDFAState[startClosure] = start_state;
        unmarkedStates.push(startClosure);

        // Mark start state as an accepting state if it contains any of the NFA's final states
        for (const auto& finalState : finalStates) {
            if (startClosure.find(finalState) != startClosure.end()) {
                accepting_states.insert(start_state);
                break;
            }
        }

        // Repeat until no more states can be added to the DFA
        // For each state in the DFA, create a map of transitions to the next state


        while (!unmarkedStates.empty()) { //
            StateSet current = unmarkedStates.front();
            unmarkedStates.pop();
            std::string currentState = stateSetToString(current);

            std::unordered_map<std::string, StateSet> transitions;

        

            for (const State& state : current) {
                auto it = nfaStates.find(state);
                if (it != nfaStates.end()) {
                    for (const auto& [symbol, nextStates] : it->second) { 
                        if (symbol != "") {
                            transitions[symbol].insert(nextStates.begin(), nextStates.end());
                        }
                    }
                }
            }

            // For each state in the DFA, create a map of transitions to the next state
            // Create a new state based on the map
            // If the new state is not in the DFA, add it to the DFA

            for (const auto& [symbol, nextStates] : transitions) {
                StateSet closure = epsilonClosure(nfaStates, nextStates);
                std::string nextState = stateSetToString(closure);
                if (stateSetToDFAState.find(closure) == stateSetToDFAState.end()) {
                    stateSetToDFAState[closure] = nextState;
                    //for (const auto& state : closure) {
                    //    std::cout << state << " ";
                    //}
                    unmarkedStates.push(closure);
                    // std::cout << nextState << std::endl;
                    
                }
                states[currentState][symbol] = nextState;

                // debug
                // std::cout << currentState << " --- " << symbol << " ---> " << nextState << std::endl;

                // Mark state as accepting if it contains any of the NFA's final states
                for (const State& finalState : finalStates) {
                    if (closure.find(finalState) != closure.end()) {
                        accepting_states.insert(nextState);
                        break;
                    }
                }
            }
        }
    }

    /**
     * @brief Initialize the partition with accepting and non-accepting states
     * Procedure:
     * 1. Add accepting states to the partition
     * 2. Add non-accepting states to the partition
     * 
     * @return std::vector<StateSet> 
     */

    std::vector<StateSet> initialize() {
        std::vector<StateSet> P = {accepting_states};
        StateSet non_accepting_states;
        for (const auto& state : states) {
            if (accepting_states.find(state.first) == accepting_states.end()) {
                non_accepting_states.insert(state.first);
            }
        }
        if (!non_accepting_states.empty()) { // Add non-accepting states only if they exist
            P.push_back(non_accepting_states);
        }

        return P;

    }

    /**
     * @brief Refine the partition based on the transitions
     * Procedure:
     * 1. Repeat until no more partitions can be refined:
     *  a. For each group in the current partition:
     *    i. Create a map of transitions to the next partition
     *  b. Create a new partition based on the map
     *  c. If the new partition has more than one group, update the partition
     * 2. Return the refined partition
     * 
     * 
     * @param P 
     * @return std::vector<StateSet> 
     */


    std::vector<StateSet> refine_partition(std::vector<StateSet>& P) {  

        //optimized

        bool partitions_changed;
        do {
            partitions_changed = false;
            std::vector<StateSet> new_P;

            std::unordered_map<State,int> s2p ;

            for(int i = 0 ; i < P.size() ; i++) {
                for (const auto& state : P[i]) {
                    s2p[state] = i;
                }
            }


            for (const auto& group : P) { // For each group in the current partition
                std::unordered_map<std::string, StateSet> partition_map; // Map of transitions to the next partition

                for (const auto& state : group) { // For each state in the group 
                    std::ostringstream ks ; // Create a key for the map
                    
                    for (const auto& [symbol, next_state] : states[state]) { // For each transition in the state 
                        ks << s2p[next_state] << "-" << symbol << ";"; // Add the transition to the key

                    }
                    partition_map[ks.str()].insert(state); // Add the state to the partition
                }
                // debug
                // for (const auto& [key, partition] : partition_map) {
                //     std::cout << key << " : ";
                //     for (const auto& state : partition) {
                //         std::cout << state << " ---hello--- ";
                //     }

                //     std::cout << std::endl;
                // 

                for (const auto& partition : partition_map) { // Create a new partition based on the map
                    new_P.push_back(partition.second); // Add the partition to the new partition
                }
                if (partition_map.size() > 1) { 
                    // If the new partition has more than one group Update the partition
                    partitions_changed = true; 
                }
            }
            P = new_P;
        } while (partitions_changed); 

        return P;
    }

    /**
     * @brief Create a New DFA object
     * Procedure:
     * 1. For each group in the partition:
     *  a. If the group is not empty, set the representative as the first state in the group
     *  b. For each state in the group, set the representative as the representative of the state
     * 2. For each state in the DFA:
     *  a. For each transition:
     *      i. Update the transition with the representative of the state
     * 3. Return the new DFA and representative
     * 
     * 
     * @param P 
     * @return std::pair<DFATransitionTable,std::unordered_map<State, State>> 
     */


    std::pair<DFATransitionTable,std::unordered_map<State, State>> createNewDFA(std::vector<StateSet>& P) {

        DFATransitionTable new_states;
        std::unordered_map<State, State> representative;

        for (const auto& group : P) { 
            if (!group.empty()) {
                State rep = *group.begin(); // If the group is not empty, set the representative as the first state in the group
                for (const State& state : group)  representative[state] = rep; 
            }
        }

        for (const auto& [state, transitions] : states) { 
            // For each state in the DFA 
            // For each transition
             // Update the transition
            for (const auto& [symbol,next_state] : transitions)
                new_states[representative[state]][symbol] = representative[next_state]; 
        }


        return {new_states, representative};
    }

    /**
     * @brief Update the DFA with the new states and representative
     *  Procedure:
     * 1. Update DFA states and accepting states
     * 2. For each accepting state:
     * . If the state is in the representative, add the state to the accepting states
     * 
     * @param new_states 
     * @param representative 
     * @return StateSet 
     */


    StateSet updateDFA(DFATransitionTable& new_states,std::unordered_map<State, State>& representative) {

        // Update DFA states and accepting states
        states = new_states;
        start_state = representative[start_state]; 
        StateSet new_accepting_states;
        for (const auto& state : accepting_states) { 
            // For each accepting state 
            // If the state is in the representative
            // Add the state to the accepting states
            if (representative.find(state) != representative.end())  new_accepting_states.insert(representative[state]); 

        }
        
        return new_accepting_states;
    }

    /**
     * @brief Minimize the DFA using Moore's algorithm
     * Procedure:
     * 1. Initialize partitions with accepting and non-accepting states P = A, Q - A 
     * 2. Repeat until no more partitions can be refined:
     *   a. For each group in the current partition, create a map of transitions to the next partition
     *   b. Create a new partition based on the map 
     * c. If the new partition has more than one group, update the partition
     * 3. Create a new DFA based on the refined partition
     * 4. Update DFA states and accepting states
     * 
     */

    
    void mooreMinimization() {
        // 1. Initialize partitions with accepting and non-accepting states
        std::vector<StateSet> P = initialize();
        // 2. Repeat until no more partitions can be refined
        
        P = refine_partition(P);

        // 3 . new dfa based on refined partition
        std::pair<DFATransitionTable,std::unordered_map<State, State>> new_dfa = createNewDFA(P);

        // 4. Update DFA states and accepting states
        accepting_states = updateDFA(new_dfa.first, new_dfa.second);
        
    }
};