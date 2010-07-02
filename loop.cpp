#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <iostream>
#include "lexer.cpp"
#include "parser.cpp"

using namespace llvm;


// ===== main =================================================================

int main(int argc, char ** argv) {
    Parser parser;
    while (parser.token.type != tok_eof) {
        ExprAST* expression = parser.parseExpression();
        if (expression == NULL) {
            parser.eat();
        } else {
            fprintf(stderr, "success!\n");
        }
    }
}

