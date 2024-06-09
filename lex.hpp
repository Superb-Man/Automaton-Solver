#include<bits/stdc++.h>

#define OR 1
#define STAR 2
#define PLUS 3
#define OPEN_PAREN 4
#define CLOSED_PAREN 5
#define LITERAL 6

std::pair<int,std::string> getTokenType(char token) {
    if(token == '|') return {OR, "OR"} ;
    if(token == '*') return {STAR, "STAR"} ;
    if(token == '+') return {PLUS, "PLUS"} ;
    if(token == '(') return {OPEN_PAREN, "OPEN_PAREN"} ;
    if(token == ')') return {CLOSED_PAREN, "CLOSED_PAREN"} ;
    return {LITERAL, "LITERAL"} ;
}

char getTokenVal(int token) {
    if(token == OR) return '|' ;
    if(token == STAR) return '*' ;
    if(token == PLUS) return '+' ;
    if(token == OPEN_PAREN) return '(' ;
    if(token == CLOSED_PAREN) return ')' ;
    return token ;
}

struct Token {
    std::string name;
    char val;
    int index;
    std::string content;

    Token(std::string name, char val,int index,std::string content = "") {
        this->name = name ;
        this->val = val ;
        this->index = index;
        this->content = content;
    }
    //overload the  cout operator
    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << token.name << ", " << token.val << ")";
        return os;
    }
};

std::vector<Token> lexer(std::string regex) {
    std::vector<Token> tokenStream;
    for(int i = 0; i < regex.size(); i++) {
        auto it = getTokenType(regex[i]);
        Token token(it.second, regex[i],it.first);
        tokenStream.push_back(token);
    }
    return tokenStream;
}
