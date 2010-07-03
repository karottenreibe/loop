#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <map>
#include "lexer.cpp"
#include "parser.cpp"

using namespace llvm;

struct CodeGenerator {
    Module* module;
    IRBuilder<> builder(getGlobalContext());
    std::map<std::string, Value*> identifiers;

    CodeGenerator() {
        this->module = new Module("module", getGlobalContext());
    }

    ~CodeGenerator() {
        delete module;
    }

    Value* error(const char* msg) {
        fprintf(stderr, msg);
        return NULL;
    }
};

Value* NumberAST::codegen(CodeGenerator* generator) {
    return ConstantInt::get(getGlobalContext(), APInt(this->value));
}

Value* IdentifierAST::codegen(CodeGenerator* generator) {
    Value* value = generator->identifiers[this->name];
    if (value == NULL) {
        std::string message = "reference to undefined variable ";
        message += this->name;
        return error(message.c_str());
    } else {
        return value;
    }
}

Value* ValueAST::codegen(CodeGenerator* generator) {
    Value* lhs_val = this->lhs.codegen(generator);
    Value* rhs_val = this->rhs.codegen(generator);
    if (lhs_val == NULL || rhs_val == NULL) {
        return NULL;
    }
    switch (this->op) {
    case '+':
        return generator->builder.CreateAdd(lhs_val, rhs_val);
    case '-':
        IRBuilder<> builder = generator->builder;
        // calculate exact result (might be negative)
        Value* exact = builder.CreateSub(lhs_val, rhs_val);
        // create condition
        Value* condition = Builder.CreateICmpSLT(exact,
            ConstantInt::get(getGlobalContext(), APInt(0.0)), "ifcond");
        // create if/then/else blocks
        Function* fun = builder.GetInsertBlock()->getParent();
        BasicBlock* then_block = BasicBlock::Create(getGlobalContext(), "then", fun);
        BasicBlock* else_block = BasicBlock::Create(getGlobalContext(), "else");
        BasicBlock* merge_block = BasicBlock::Create(getGlobalContext(), "ifmerge");
        // create conditional branch
        builder.CreateCondBr(condition, then_block, else_block);
        // fill in then block, i.e. normalize to 0
        builder.SetInsertPoint(then_block);
        Value* then_value = ConstantInt::get(getGlobalContext(), APInt(0.0));
        builder.CreateBr(merge_block);
        // fill in else block, i.e. return exact result
        then_block = builder.GetInsertBlock();
        fun->getBasicBlockList().push_back(else_block);
        builder.SetInsertPoint(else_block);
        builder.CreateBr(merge_block);
        else_block = builder.GetInsertBlock();
        // fill in merge block
        fun->getBasicBlockList().push_back(merge_block);
        builder.SetInsertPoint(merge_block);
        PHINode* phi = builder.CreatePHI(Type::getInt32PtrTy(getGlobalContext()), "iftmp");
        phi->addIncoming(then_value, then_block)
        phi->addIncoming(exact, else_block)
        return phi;
    default:
        std::string message = "unknown operator: ";
        message += this->op;
        return error(message.c_str());
    }
}

Value* LoopAST::codegen(CodeGenerator* generator) {
    IRBuilder<> builder = generator->builder;
    // generate start value
    Value* start_counter = this->argument->codegen(generator);
    // get the blocks
    Function* fun = builder.GetInsertBlock()->getParent();
    BasicBlock* header_block = builder.GetInsertBlock();
    BasicBlock* condition_block = BasicBlock::Create(getGlobalContext(), "loopcondition");
    BasicBlock* body_block = BasicBlock::Create(getGlobalContext(), "loopbody");
    BasicBlock* after_block = BasicBlock::Create(getGlobalContext(), "afterloop");
    // fill header with branch to loop
    builder.CreateBr(loop_block);
    // add phi to loop block and add incoming for header
    fun->getBasicBlockList().push_back(condition_block);
    builder.SetInsertPoint(loop_block);
    PHINode* phi = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), "_loopvar");
    phi->addIncoming(start_counter, header_block);
    Value* old_value = generator->identifiers["_loopvar"];
    generator->identifiers["_loopvar"] = phi;
    // add end condition
    Value* condition = builder.CreateICmpEQ(phi, ConstantInt::get(getGlobalContext(), APInt(0)), "loopcond");
    builder.CreateCondBr(condition, after_block, body_block);
    // fill loop with body
    fun->getBasicBlockList().push_back(body_block);
    builder.SetInsertPoint(body_block);
    if (body->codegen(generator) == NULL) {
        return NULL;
    } else {
        // step down
        Value* next_counter = builder.CreateSub(phi, ConstantInt::get(getGlobalContext(), APInt(1)));
        builder.CreateBr(loop_block);
        phi->addIncoming(next_counter, body_block);
        builder.CreateBr(condition_block);
        // create afterblock
        fun->getBasicBlockList().push_back(after_block);
        builder.SetInsertPoint(after_block);
        // restore ident table
        generator->identifiers["_loopvar"] = old_value;
        return Constant::getNullValue(Type::getInt32Ty(getGlobalContext()));
    }
}

Value* AssignAST::codegen(CodeGenerator* generator) {
}

Value* SequenceAST::codegen(CodeGenerator* generator) {
}

Function* TopLevelAST::codegen(CodeGenerator* generator) {
    std::vector<const Type*> arguments(0);
    const Type* ret = Type::getInt32Ty(getGlobalContext()); //TODO
    FunctionType* fun_type = FunctionType::get(ret, args, false);
    Function* fun = Function::Create(fun_type, Function::ExternalLinkage, "", generator->module);
    BasicBlock* entry = BasicBlock::Create(getGlobalContext(), "entry", fun);
    generator->builder.SetInsertPoint(entry);
    Value* body = this->expression.codegen(generator);
    if (body == NULL) {
        fun->eraseFromParent();
        return NULL;
    } else {
        generator->builder.CreateRet(body);
        verifyFunction(fun);
        return fun;
    }
}


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

