#include "parser.hpp"

class ParseRegex {
private:
    std::vector<Token> tokenStream;
    int currToken;

    std::shared_ptr<AstNode> parse_R() {
        return parse_U();
    }

    std::shared_ptr<AstNode> parse_U() {
        std::shared_ptr<AstNode> ast = parse_C();
        while (isMatch(OR)) {
            std::shared_ptr<AstNode> left = ast;
            std::shared_ptr<AstNode> right = parse_C();
            ast = std::make_shared<OrAstNode>(left, right);
        }
        return ast;
    }

    std::shared_ptr<AstNode> parse_C() {
        std::shared_ptr<AstNode> ast = parse_K();
        while (currToken < tokenStream.size()) {
            int tokenType = tokenStream[currToken].index;
            if (tokenType == LITERAL || tokenType == OPEN_PAREN) {
                std::shared_ptr<AstNode> left = ast;
                std::shared_ptr<AstNode> right = parse_K();
                ast = std::make_shared<SeqAstNode>(left, right);
            } else {
                break;
            }
        }
        return ast;
    }

    std::shared_ptr<AstNode> parse_K() {
        std::shared_ptr<AstNode> ast = parse_S();
        if (isMatch(STAR)) {
            ast = std::make_shared<StarAstNode>(ast);
        } else if (isMatch(PLUS)) {
            ast = std::make_shared<PlusAstNode>(ast);
        }
        return ast;
    }

    std::shared_ptr<AstNode> parse_S() {
        if (isMatch(OPEN_PAREN)) {
            std::shared_ptr<AstNode> ast = parse_R();
            check(CLOSED_PAREN);
            return ast;
        } else if (isMatch(LITERAL)) {
            return std::make_shared<LiteralCharacterAstNode>(tokenStream[currToken - 1].val);
        } else {
            return nullptr;
        }
    }

    bool isMatch(int tokenType) {
        if (currToken >= tokenStream.size()) {
            return false;
        }
        if (tokenStream[currToken].index == tokenType) {
            currToken++;
            return true;
        }
        return false;
    }

    void check(int tokenType) {
        if (!isMatch(tokenType)) {
            throw std::runtime_error("Expected token");
        }
    }

public:
    ParseRegex(const std::vector<Token>& tokenStream) {
        this->tokenStream = tokenStream;
        currToken = 0;
    }

    std::shared_ptr<AstNode> parse() {
        std::shared_ptr<AstNode> ast = parse_R();
        if (currToken < tokenStream.size()) {
            throw std::runtime_error("Unchecked token");
        }
        return ast;
    }
};

