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

