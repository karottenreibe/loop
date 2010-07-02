#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "lexer.cpp"

using namespace llvm;

struct ExprAST {
    virtual ~ExprAST() {}
};

// <number> := [0-9]+
struct NumberAST : public ExprAST {
    double value;
    NumberAST(double val) : value(val) {}
};

// <identifier> := [a-z][a-z0-9]*
struct IdentifierAST : public ExprAST {
    std::string name;
    IdentifierAST(std::string nam) : name(nam) {}
};

// <term> := <value> + <value> | <value> - <value>
struct ValueAST : public ExprAST {
    char op;
    ExprAST* lhs;
    ExprAST* rhs;
    ValueAST(ExprAST* l, char o, ExprAST* r) : op(o), lhs(l), rhs(r) {}
    virtual ~ValueAST() { delete lhs; delete rhs; }
};

// <loop> := loop <value> do <expression> end
struct LoopAST : public ExprAST {
    ExprAST* argument;
    ExprAST* body;
    LoopAST(ExprAST* arg, ExprAST* b) : argument(arg), body(b) {}
    virtual ~LoopAST() { delete argument; delete body; }
};

// <assignment> := <identifier> = <value>
struct AssignAST : public ExprAST {
    ExprAST* identifier;
    ExprAST* value;
    AssignAST(ExprAST* ident, ExprAST* val) : identifier(ident), value(val) {}
    virtual ~AssignAST() { delete value; }
};

// <expression> := <expression> ; <expression>
struct SequenceAST : public ExprAST {
    ExprAST* lhs;
    ExprAST* rhs;
    SequenceAST(ExprAST* l, ExprAST* r) : lhs(l), rhs(r) {}
    virtual ~SequenceAST() { delete lhs; delete rhs; }
};

struct Parser {
    Token token;
    Lexer lexer;

    Parser() {
        eat();
    }

    Token eat() {
        Token buf = token;
        token = lexer.next_token();
        return buf;
    }

    ExprAST* error(const char * expected) {
        fprintf(stderr, "error: unexpected token. expected `%s'\n", expected);
        return NULL;
    }

    // <expression> := <expression> ; <expression> | <assignment> | <loop>
    ExprAST* parseExpression() {
        ExprAST* lhs;
        if (token.type == tok_loop) {
            lhs = parseLoop();
        } else if (token.type == tok_ident) {
            lhs = parseAssignment();
        } else if (token.type == tok_eof) {
        } else {
            lhs = error("expression");
        }
        if (token.type == tok_sep) {
            ExprAST* rhs = parseExpression();
            return new SequenceAST(lhs, rhs);
        } else {
            return lhs;
        }
    }

    // <assignment> := <identifier> = <value>
    ExprAST* parseAssignment() {
        ExprAST* ident = parseIdent();
        if (ident == NULL) {
            return NULL;
        } else {
            if (token.type != tok_assign) {
                return error("=");
            }
            eat();
            ExprAST* value = parseValue();
            if (value == NULL) {
                return NULL;
            } else {
                return new AssignAST(ident, value);
            }
        }
    }

    // <value> := <value> + <term> | <value> - <term> | <term>
    ExprAST* parseValue(ExprAST* lhs = NULL) {
        // <term>
        if (lhs == NULL && token.type != tok_number) {
            return parseTerm();
        // <value> +- <term>
        } else {
            if (lhs == NULL) {
                lhs = parseTerm();
            }
            if (token.type != tok_plus && token.type != tok_minus) {
                return error("operator");
            } else {
                char op;
                if (token.type == tok_plus) {
                    op = '+';
                } else {
                    op = '-';
                }
                eat();
                ExprAST* rhs = parseTerm();
                ValueAST* value = new ValueAST(lhs, op, rhs);
                // upwards recursion
                if (token.type == tok_plus || token.type == tok_minus) {
                    return parseValue(value);
                } else {
                    return value;
                }
            }
        }
    }

    // <parens> := (<value>)
    ExprAST* parseParens() {
        eat();
        ExprAST* body = parseValue();
        if (body == NULL) {
            return NULL;
        } else {
            if (token.type != tok_par_closed) {
                return error(")");
            }
            eat();
            return body;
        }
    }

    // <term> := <number> | <parens>
    ExprAST* parseTerm() {
        if (token.type == tok_par_open) {
            return parseParens();
        } else {
            return parseNumber();
        }
    }

    // <loop> := loop <value> do <expression> end
    ExprAST* parseLoop() {
        eat();
        ExprAST* value = parseValue();
        if (value == NULL) {
            return NULL;
        } else {
            if (token.type != tok_do) {
                return error("do");
            } else {
                eat();
                ExprAST* expression = parseExpression();
                if (expression == NULL) {
                    return NULL;
                } else {
                    if (token.type != tok_end) {
                        return error("end");
                    } else {
                        eat();
                        return new LoopAST(value, expression);
                    }
                }
            }
        }
    }

    // <number> := [0-9]+
    ExprAST* parseNumber() {
        Token number = eat();
        return new NumberAST(number.ibuffer);
    }

    // <identifier> := [a-z][a-z0-9]*
    ExprAST* parseIdent() {
        Token ident = eat();
        return new IdentifierAST(ident.cbuffer);
    }
};


// ===== main =================================================================

int main(int argc, char ** argv) {
    std::cout << "Reading..." << std::endl;
    Lexer lexer;
    while (1) {
        Token token = lexer.next_token();
        std::cout << "Token of type " << token.type << std::endl;
        std::cout << "  cbuffer: " << token.cbuffer << std::endl;
        std::cout << "  ibuffer: " << token.ibuffer << std::endl;
        if (token.type == tok_eof) {
            break;
        }
    }
}

