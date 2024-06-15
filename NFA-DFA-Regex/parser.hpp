#include"lex.hpp"
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual std::string getLabel() const = 0;
};

class OrAstNode : public AstNode {

public:
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;

    OrAstNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right) {
        this->left = left;
        this->right = right;
    }
    std::string getLabel() const override {
        return "|";
    }

};

class SeqAstNode : public AstNode {

public:
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;

    SeqAstNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right){
        this->left = left;
        this->right = right;
    }
    std::string getLabel() const override {
        return ".";
    }


};

class StarAstNode : public AstNode {

public:
    std::shared_ptr<AstNode> left;

    explicit StarAstNode(std::shared_ptr<AstNode> left) {
        this->left = left;
    }
    std::string getLabel() const override {
        return "*";
    }


};

class PlusAstNode : public AstNode {
public:
    std::shared_ptr<AstNode> left ;

    explicit PlusAstNode(std::shared_ptr<AstNode> left) {
        this->left = left ;
    }
    std::string getLabel() const override {
        return "+";
    }


};

class LiteralCharacterAstNode : public AstNode {
public:
    char ch;

    explicit LiteralCharacterAstNode(char ch) {
        this->ch = ch;
    }
    std::string getLabel() const override {
        return std::string(1, ch);
    }
};
