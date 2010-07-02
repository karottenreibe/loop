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

// <expression> := <expression> ; <expression> | <assignment> | <loop>
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
struct TermAST : public ExprAST {
    BinaryOperator op;
    ExprAST lhs;
    ExprAST rhs;
    TermAST(ExprAST l, BinaryOperator o, ExprAST r) : op(o), lhs(l), rhs(r) {}
};

// <loop> := loop <value> do <expression> end
struct LoopAST : public ExprAST {
    ExprAST argument;
    ExprAST body;
    LoopAST(ExprAST arg, ExprAST b) : argument(arg), body(b) {}
};

// <assignment> := <identifier> = <value>
struct AssignAST : public ExprAST {
    IdentifierAST identifier;
    ExprAST value;
    AssignAST(IdentifierAST ident, ExprAST val) : identifier(ident), value(val) {}
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
        return buf
    }

    ExprAST* error(const char * expected) {
        fprintf(stderr, "error: unexpected token. expected `%s'\n", expected);
        return NULL;
    }

    // <expression> := <expression> ; <expression> | <assignment> | <loop>

    // <assignment> := <identifier> = <value>
    ExprAST* parseAssignment() {
        IdentifierAST ident = parseIdent();
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

    // <value> := <number> | <term> | <parens>
    ExprAST* parseValue() {
        // <parens>
        if (token.type == tok_par_open) {
            return parseParens();
        } else if (token.type == tok_number) {
            return parseNumber();
        } else {
            return parseTerm();
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

    // <term> := <value> + <value> | <value> - <value>

    // <loop> := loop <value> do <expression> end

    // <number> := [0-9]+
    ExprAST* parseNumber() {
        Token number = eat();
        return new NumberAST(indent.ibuffer);
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

