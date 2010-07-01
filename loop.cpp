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

using namespace llvm;

// ===== lexer ================================================================

enum TokenType {
    tok_eof = -1,
    tok_loop = -2,
    tok_do = -3,
    tok_end = -4,
    tok_number = -5,
    tok_plus = -6,
    tok_minus = -7,
    tok_par_open = -8,
    tok_par_closed = -9,
    tok_assign = -10,
    tok_ident = -11,
    tok_sep = -12,
    tok_invalid = -99,
};

struct Token {
    int type;
    int ibuffer;
    std::string cbuffer;
    Token() {
        this->type = 0;
        this->ibuffer = 0;
        this->cbuffer = "";
    }
};

struct Lexer {
    char last_char;

    Lexer() {
        this->last_char = ' ';
    }

    Token next_token() {
        Token token;
        while (true) {
            // identifier := [a-z][a-z0-9]*
            if (isalpha(last_char) || (token.type == tok_ident && isdigit(last_char))) {
                token.type = tok_ident;
                token.cbuffer += last_char;
            // number := [0-9]+
            } else if (isdigit(last_char)) {
                token.type = tok_number;
                token.ibuffer = (token.ibuffer * 10) + last_char - 48;
            // +
            } else if (last_char == '+') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_plus;
                last_char = getchar();
                break;
            // -
            } else if (last_char == '-') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_minus;
                last_char = getchar();
                break;
            // =
            } else if (last_char == '=') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_assign;
                last_char = getchar();
                break;
            // ;
            } else if (last_char == ';') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_sep;
                last_char = getchar();
                break;
            // space
            } else if (isspace(last_char)) {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
            // EOF
            } else if (last_char == EOF) {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_eof;
                break;
            // invalid
            } else {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_invalid;
                token.cbuffer += last_char;
                last_char = getchar();
                break;
            }
            last_char = getchar();
        }
        return token;
    }
};


// ===== parser ===============================================================

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

// <parens> := (<value>)
struct ParensAST : public ExprAST {
    ExprAST body;
    ParensAST(ExprAST b) : body(b) {}
};

// <assignment> := <identifier> = <value>
struct AssignAST : public ExprAST {
    IdentifierAST identifier;
    ExprAST value;
    AssignAST(IdentifierAST ident, ExprAST val) : identifier(ident), value(val) {}
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

