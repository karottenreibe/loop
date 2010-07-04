using namespace llvm;
struct CodeGenerator;

struct ExprAST {
    virtual ~ExprAST() {}
    virtual Value* codegen(CodeGenerator* generator) = 0;
};

// <number> := [0-9]+
struct NumberAST : public ExprAST {
    int value;
    NumberAST(int val) : value(val) {}
    virtual Value* codegen(CodeGenerator* generator);
};

// <identifier> := [a-z][a-z0-9]*
struct IdentifierAST : public ExprAST {
    std::string name;
    IdentifierAST(std::string nam) : name(nam) {}
    virtual Value* codegen(CodeGenerator* generator);
};

// <value> := <value> + <term> | <value> - <term>
struct ValueAST : public ExprAST {
    char op;
    ExprAST* lhs;
    ExprAST* rhs;
    ValueAST(ExprAST* l, char o, ExprAST* r) : op(o), lhs(l), rhs(r) {}
    virtual ~ValueAST() { delete lhs; delete rhs; }
    virtual Value* codegen(CodeGenerator* generator);
};

// <loop> := loop <value> do <expression> end
struct LoopAST : public ExprAST {
    ExprAST* argument;
    ExprAST* body;
    LoopAST(ExprAST* arg, ExprAST* b) : argument(arg), body(b) {}
    virtual ~LoopAST() { delete argument; delete body; }
    virtual Value* codegen(CodeGenerator* generator);
};

// <assignment> := <identifier> = <value>
struct AssignAST : public ExprAST {
    IdentifierAST* identifier;
    ExprAST* value;
    AssignAST(IdentifierAST* ident, ExprAST* val) : identifier(ident), value(val) {}
    virtual ~AssignAST() { delete value; }
    virtual Value* codegen(CodeGenerator* generator);
};

// <expression> := <expression> ; <expression>
struct SequenceAST : public ExprAST {
    ExprAST* lhs;
    ExprAST* rhs;
    SequenceAST(ExprAST* l, ExprAST* r) : lhs(l), rhs(r) {}
    virtual ~SequenceAST() { delete lhs; delete rhs; }
    virtual Value* codegen(CodeGenerator* generator);
};

struct TopLevelAST : public ExprAST {
    ExprAST* expression;
    TopLevelAST(ExprAST* exp) : expression(exp) {}
    virtual ~TopLevelAST() { delete expression; }
    virtual Function* codegen(CodeGenerator* generator);
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

    ExprAST* error(const char* expected) {
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
            return lhs;
        } else {
            lhs = error("expression");
            fprintf(stderr, "got: %i\n", token.type);
        }
        if (token.type == tok_sep) {
            eat();
            ExprAST* rhs = parseExpression();
            return new SequenceAST(lhs, rhs);
        } else {
            return lhs;
        }
    }

    // <assignment> := <identifier> = <value>
    ExprAST* parseAssignment() {
        IdentifierAST* ident = parseIdent();
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
        if (lhs == NULL) {
            lhs = parseTerm();
        }
        if (token.type != tok_plus && token.type != tok_minus) {
            return lhs;
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

    // <term> := <number> | <identifier> | <parens>
    ExprAST* parseTerm() {
        if (token.type == tok_par_open) {
            return parseParens();
        } else if (token.type == tok_number) {
            return parseNumber();
        } else if (token.type == tok_ident) {
            return (ExprAST*) parseIdent();
        } else {
            return error("term");
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
    IdentifierAST* parseIdent() {
        Token ident = eat();
        return new IdentifierAST(ident.cbuffer);
    }
};


