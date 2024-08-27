This is a basic regular expression to DFA converter based on the coursework of theory of computation.Implemented ```*,+,Seq,OR``` as we weren't taught about ```(.,[],-,^,?)``` in that course. 
However now implementing a full scope regular expression solver for 
making a lexical analysis of a C program here in this repo : https://github.com/Superb-Man/Lexical_Analyzer
# Features
```
- It takes an input as expression and parse it.
- Then from the abstract syntax tree it makes NFA and from NFA.
- it is converted into DFA using the epsilon closure propery....
- Then moore minimization algorithm is used to minimize the DFA.
- An additional string check is used also(Whether the input belongs to the expression)
- worked with OR/UNION,STAR,SEQ/AND,PLUS and literals
```
# Screenshots 
```
- abstract syntax tree for a+b*(c+de)*f
```
![parse_tree](https://github.com/user-attachments/assets/9b9c334c-b232-47f7-aafc-0fedce4560c1)
```
- nfa and min dfa
```
![nfa](https://github.com/user-attachments/assets/7079e41f-bd83-4aea-96c7-7ccbe1e022ad)
![dfa_moore_minimizedTable](https://github.com/Superb-Man/TOC-Solver/assets/104999005/fa69ec28-4a37-4003-b11d-fdac50c6e1e9)
![dfa_moore_minimized](https://github.com/Superb-Man/TOC-Solver/assets/104999005/41afb480-46b1-4611-a332-8c00197e0016)

```
- CYK simulation for the CFG
- S -> A B | B C
- A -> B A | a
- B -> C C | b
- C -> A B | a
```
![cyk_table](https://github.com/Superb-Man/TOC-Solver/assets/104999005/7d0666ad-9aea-4959-b7e5-0fc6f4331df8)

# Will try to implement 
```
- CFG To CNF
- CFG to PDA
- CFG to LL(K)
- Left factoring and Left recursion elimination
- Computing first and follow sets
```
