#install graphviz if not installed
if ! command -v dot &> /dev/null
then
    echo "Graphviz could not be found, installing..."
    sudo apt-get install graphviz
fi

g++ NFA-DFA-Regex/draw.cpp -o dfa_draw
g++ CFG-PDA-CYK-CNF/main.cpp -o cfg_draw
g++ NFA-DFA-Regex/test.cpp -o regex_test