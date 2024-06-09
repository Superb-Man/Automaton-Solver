#include "NFA.hpp"
class DFA {
public:
    using DFATransitionTable = std::unordered_map<State, std::unordered_map<std::string,State>>;

    DFA(const NFA& nfa) {
        convertFromNFA(nfa);
        mooreMinimization();
    }
    std::unordered_map<State, std::unordered_map<std::string,State>> dfaStruct() {
        std::vector<State> dfa_states;
        for (const auto& state_pair : this->states) {
            dfa_states.push_back(state_pair.first);
        }
        for (const auto& state : accepting_states) {
            dfa_states.push_back(state);
        }

        std::unordered_map<State ,int> state_map = StateToNumber(dfa_states);

        std::unordered_map<State, std::unordered_map<std::string,State>> dfa_dict;
        dfa_dict["start"][""] = std::to_string(state_map[start_state]);

        for (const auto& state_pair : states) {
            const auto& state_name = state_pair.first;
            const auto& transitions = state_pair.second;

            for (const auto& transition_pair : transitions) {
                std::string symbol = transition_pair.first.empty() ? "epsilon" : transition_pair.first;
                dfa_dict[std::to_string(state_map[state_name])][symbol] = std::to_string(state_map[transition_pair.second]);
            }
            dfa_dict[std::to_string(state_map[state_name])]["ending"] = accepting_states.count(state_name) ? "true" : "false";
        }

        for (const auto& state : accepting_states) {
            dfa_dict[std::to_string(state_map[state])]["ending"] = "true";
        }

    
        dfaTable(state_map);

        return dfa_dict;
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

    bool match(const std::string& input) const {
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

private:
    DFATransitionTable states;
    StateSet accepting_states;  
    State start_state;

    StateSet epsilonClosure(const TransitionTable& nfa, const State& state) const {
        StateSet closure;
        std::stack<State> stack;
        stack.push(state);

        while (!stack.empty()) {
            State current = stack.top();
            stack.pop();

            if (closure.find(current) == closure.end()) {
                closure.insert(current);
                auto it = nfa.find(current);
                if (it != nfa.end() && it->second.find("") != it->second.end()) {
                    for (const State& nextState : it->second.at("")) {
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

    // Convert NFA to DFA
    void convertFromNFA(const NFA& nfa) {
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

        while (!unmarkedStates.empty()) {
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

            for (const auto& [symbol, nextStates] : transitions) {
                StateSet closure = epsilonClosure(nfaStates, nextStates);
                std::string nextState = stateSetToString(closure);
                if (stateSetToDFAState.find(closure) == stateSetToDFAState.end()) {
                    stateSetToDFAState[closure] = nextState;
                    unmarkedStates.push(closure);
                }
                states[currentState][symbol] = nextState;

                // Mark state as accepting if it contains any of the NFA's final states
                for (const auto& finalState : finalStates) {
                    if (closure.find(finalState) != closure.end()) {
                        accepting_states.insert(nextState);
                        break;
                    }
                }
            }
        }
    }
    void mooreMinimization() {
        //Initialize partitions with accepting and non-accepting states
        std::vector<StateSet> P = {accepting_states};
        StateSet non_accepting_states;
        for (const auto& state : states) {
            if (accepting_states.find(state.first) == accepting_states.end()) {
                non_accepting_states.insert(state.first);
            }
        }
        if (!non_accepting_states.empty()) {
            P.push_back(non_accepting_states);
        }

        bool partitions_changed;
        do {
            partitions_changed = false;
            std::vector<StateSet> new_P;

            for (const auto& group : P) {
                std::unordered_map<std::string, StateSet> partition_map;

                for (const auto& state : group) {
                    std::string key;
                    for (const auto& [symbol, next_state] : states[state]) {
                        for (size_t i = 0; i < P.size(); ++i) {
                            if (P[i].find(next_state) != P[i].end()) {
                                key += std::to_string(i) + "-" + symbol + ";";
                                break;
                            }
                        }
                    }
                    partition_map[key].insert(state);
                }

                for (const auto& partition : partition_map) {
                    new_P.push_back(partition.second);
                }
                if (partition_map.size() > 1) {
                    partitions_changed = true;
                }
            }
            P = new_P;
        } while (partitions_changed);

        //new dfa based on refined partition
        DFATransitionTable new_states;
        std::unordered_map<State, State> representative;

        for (const auto& group : P) {
            if (!group.empty()) {
                auto rep = *group.begin();
                for (const auto& state : group) {
                    representative[state] = rep;
                }
            }
        }

        for (const auto& [state, transitions] : states) {
            for (const auto& [symbol, next_state] : transitions) {
                new_states[representative[state]][symbol] = representative[next_state];
            }
        }

        // Update DFA states and accepting states
        states = new_states;
        start_state = representative[start_state];
        StateSet new_accepting_states;
        for (const auto& state : accepting_states) {
            if (representative.find(state) != representative.end()) {
                new_accepting_states.insert(representative[state]);
            }
        }
        accepting_states = new_accepting_states;
    }

};