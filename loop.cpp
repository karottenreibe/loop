#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "lexer.cpp"
#include "parser.cpp"
#include "codegen.cpp"

using namespace llvm;

int main(int argc, char ** argv) {
    InitializeNativeTarget();
    LLVMContext &context = getGlobalContext();

    // Make the module, which holds all the code.
    Module* module = new Module("LOOP program", context);

    // Create the JIT.
    std::string error_message;
    ExecutionEngine* execution_engine = EngineBuilder(module).setErrorStr(&error_message).create();
    if (!execution_engine) {
        fprintf(stderr, "Fatal: Could not create ExecutionEngine: %s\n", error_message.c_str());
        delete module;
        return 1;
    }

    FunctionPassManager fpm(module);

    // Set up the optimizer pipeline.  Start with registering info about how the
    // target lays out data structures.
    fpm.add(new TargetData(*execution_engine->getTargetData()));
    // Promote allocas to registers.
    fpm.add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm.add(createInstructionCombiningPass());
    // Reassociate expressions.
    fpm.add(createReassociatePass());
    // Eliminate Common SubExpressions.
    fpm.add(createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    fpm.add(createCFGSimplificationPass());

    fpm.doInitialization();

    // Parse all input.
    Parser parser;
    CodeGenerator generator(module, &fpm);
    TopLevelAST* toplevel = parser.parseToplevel();
    if (toplevel != NULL) {
        Value* toplevel_value = toplevel->codegen(&generator);

        // print header
        std::ifstream stream("header.s");
        std::istreambuf_iterator<char> buffer(stream);
        std::string header(buffer, std::istreambuf_iterator<char>());
        std::cout << header << std::endl;

        // Print out all of the generated code.
        raw_stdout_ostream ostream;
        module->print(ostream, NULL);

        delete toplevel;
        delete execution_engine;
        return 0;
    } else {
        delete toplevel;
        delete execution_engine;
        return 2;
    }
}

