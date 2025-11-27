
#include <iostream>
#include <cmath>
#include <vector>

#include <stdio.h>

#include "antlr4-runtime.h"

#include "SimpleIRLexer.h"
#include "SimpleIRParser.h"
#include "SimpleIRBaseListener.h"

using namespace antlr4;
using namespace std;

const int bytewidth = 8;
static const char *registers[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

const char *filename;
map<string, int> symtab;
int stackoffset = 0;

static string operand_to_string(antlr4::Token *token) {
  stringstream operand;
  if (SimpleIRParser::NAME == token->getType()) {
    operand << symtab[token->getText()] << "(%rbp)";
  } else if (SimpleIRParser::NUM == token->getType()) {
    operand << "$" << token->getText();
  } else {
    assert(false);
  }
  return operand.str();
}

class CodeGen : public SimpleIRBaseListener {
public:
  virtual void enterUnit(SimpleIRParser::UnitContext * ctx) override {

    // filename
  cout << "\t.file \"" << filename << "\"" << endl;
  cout << "\t.section .note.GNU-stack,\"\",@progbits" << endl;
  cout << "\t.text" << endl;
  }

  virtual void enterFunction(SimpleIRParser::FunctionContext * ctx) override {

    string functionName = ctx -> functionName -> getText();
    
    cout << "\t.globl " << functionName << endl;
    cout << "\t.type " << functionName << ", @function" << endl;
    cout << functionName << ":" << endl;

    cout << "\t# prologue, update stack pointer" << endl;
    cout << "\tpushq\t%rbp # save old base pointer" << endl;
    cout << "\tmovq\t%rsp, %rbp # set new base pointer" << endl;
    cout << "\tpushq\t%rbx # %rbx is callee-saved" << endl;
  }

  virtual void enterEnd(SimpleIRParser::EndContext * ctx) override {

    cout << "\t#epilogue" << endl;
    cout << "\tadd\t$" << stackoffset << ", %rsp" << endl;
    
    cout << "\tpop\t%rbx #restore %rbx" << endl;
    cout << "\tpop\t%rbp #restore old base pointer" << endl;
    cout << "\tret" << endl; 
  }

  virtual void enterLocalVariables(SimpleIRParser::LocalVariablesContext * ctx) override {
    auto variables = ctx->variables;
    // start from of an offset of 2x bytewidth from rbp, 1 to start after rbp
    // and another 1 to skip rbx which will have already been pushed
    int starting_offset = 2;
    for (int variable_i = 0; variable_i < variables.size(); variable_i++) {
      string variable = variables[variable_i]->getText();
      int offset = (starting_offset + variable_i) * bytewidth;
      symtab[variable] = -1 * offset;
    }
    
    // compute size of the stack space needed
    int stackspace = variables.size() * bytewidth;
    // ceiling to 8 bytes
    stackoffset = std::ceil(stackspace / 8) * 8;
    // align to 16 bytes, accounting for rbx being pushed by adding 8
    stackoffset += (stackoffset + 8) % 16;
    // emit the subtraction to allocate stack space
    cout << "\t# allocate stack space for locals" << endl;
    cout << "\tsub\t$" << stackoffset << ", %rsp" << endl;
  }

  virtual void enterParameters(SimpleIRParser::ParametersContext * ctx) override {

   const auto &formals = ctx->formals;
   int numParams = (int)formals.size();
   string formalName;
    
    for (int i = 0; i < numParams && i < 6;i++) {

      formalName = formals[i]->getText();
      int localOffset = symtab[formalName];
      
      cout << "\t# move register parameter " << formalName << " to local variable" << endl;
      cout << "\tmov\t" << registers[i] << ", " << localOffset << "(%rbp)" << endl;
    }
      for (int i = 6; i < numParams; i++){

	string formalName = formals[i]->getText();
	int localOffset = symtab[formalName];
	int sOffset = 16 + (i - 6 + 1) * 8;
	
	cout << "\t# move stack param " << formalName << " to local variable" << endl;
	cout << "\tmov\t" << sOffset << "(%rbp), %rax" << endl;
	cout << "\tmov\t%rax, " << localOffset << "(%rbp)" << endl;

      }
    }

  
  
  virtual void enterReturnStatement(SimpleIRParser::ReturnStatementContext * ctx) override {

    string operand = operand_to_string(ctx->operand);

    cout << "\t# set return value" << endl;
    cout << "\tmovq\t " << operand << ", %rax" << endl;
  }

virtual void enterStatement(SimpleIRParser::StatementContext * ctx) override {
  }

  virtual void enterAssign(SimpleIRParser::AssignContext * ctx) override {
    string operand = operand_to_string(ctx->operand);
    cout << "\t# assign " << ctx->operand->getText() << " to " << ctx->variable->getText() << endl;
    cout << "\tmov\t" << operand << ", %rax" << endl;
    cout << "\tmov\t%rax, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
}

 virtual void enterOperation(SimpleIRParser::OperationContext * ctx) override {
  }

  virtual void enterCall(SimpleIRParser::CallContext * ctx) override {

    auto &actuals = ctx->actuals;
    int numActuals = actuals.size();
    string functionName = ctx -> functionName -> getText();
    string actualVar = ctx -> variable -> getText();

    cout << "\t# " << actualVar << " := call " << functionName;
    
  
    for (auto *a : actuals){
      cout << " " << a->getText();
    }
      cout << endl;
    
    int stackArgs = (numActuals > 6) ? (numActuals - 6) : 0;
    int misAlign = (stackoffset + stackArgs * 8) % 16;
    int pad = (8 - misAlign + 16) % 16;
    

    if (pad){

      cout << "\t# aalign stack before call" << endl;
      cout << "\tsubq\t$8, %rsp" << endl;
    }

    for (int i = 0; i < numActuals && i < 6; i++){

      string actualName = actuals[i] -> getText();
      int localOffset = symtab[actualName];
      
      cout << "\t# move register argument " << actualName << " to " << registers[i] << endl;
      cout << "\tmovq\t" << localOffset << "(%rbp), " << registers[i] << endl;
    }

    for (int i = numActuals - 1; i >=6 ; i--){
      string actualName = actuals[i] -> getText();
      int localOffset = symtab[actualName];
      
      cout << "\t# push stack argument " << actualName << endl;
    cout << "\tmovq\t" << localOffset << "(%rbp), %rax" << endl;
    cout << "\tpushq\t%rax" << endl;

    }

    cout << "\tcall\t" << functionName << endl;

    int totalRest = pad + stackArgs * 8;
    if (totalRest >0){
      
      cout << "\t# restore stack after call" << endl;
      cout << "\taddq\t$" << totalRest << ", %rsp" << endl;
    }
    
    int resultOffset = symtab[actualVar];
      cout << "\t# save return value" << endl;
      cout << "\tmovq\t%rax, " << resultOffset << "(%rbp)" << endl;
    
  }

  virtual void enterLabel(SimpleIRParser::LabelContext * ctx) override {
    cout << ctx -> labelName -> getText() << ":" << endl;
  }

  virtual void enterGotoStatement(SimpleIRParser::GotoStatementContext * ctx) override {

    cout << "\tjmp\t" << ctx -> labelName -> getText() << endl;
  }

  virtual void enterIfGoto(SimpleIRParser::IfGotoContext * ctx) override {

    string operandLeft = operand_to_string(ctx -> operand1);
    string operandRight = operand_to_string(ctx -> operand2);

    cout << "\tmov\t" << operandLeft << ", %rax" << endl;
    cout << "\tmov\t" << operandRight << ", %rbx" << endl;

    cout << "\tcmp\t%rbx, %rax" << endl;
    switch (ctx -> operatorKind -> getType()){
      
    case SimpleIRParser:: EQ:
      
      cout << "\tje\t" << ctx -> labelName -> getText() << endl;
      break;

    case SimpleIRParser:: NEQ:
      
      cout << "\tjne\t" << ctx -> labelName -> getText() << endl;
      break;

    case SimpleIRParser:: LT:
      
      cout << "\tjl\t" << ctx -> labelName -> getText() << endl;
      break;

    case SimpleIRParser:: LTE:
      
      cout << "\tjle\t" << ctx -> labelName -> getText() << endl;
      break;

    case SimpleIRParser:: GT:
      
      cout << "\tjg\t" << ctx -> labelName -> getText() << endl;
      break;

    case SimpleIRParser:: GTE:
      
      cout << "\tjge\t" << ctx -> labelName -> getText() << endl;
      break;

    default:

      cerr << "\tInvalid/Unknown branch type\t" << ctx -> labelName -> getText() << endl;
      break;
    }
  }
  
  virtual void enterDereference(SimpleIRParser::DereferenceContext * ctx) override {
    string operand = operand_to_string(ctx->operand);
    string offset = to_string(symtab[ctx->variable->getText()]);
    
    cout << "\tmov " << operand << ", %rax" << endl;
    cout << "\tmov\t(%rax), %rbx" << endl;
    cout << "\tmov\t%rbx, " << offset << "(%rbp)" << endl;
  }
  
  virtual void enterReference(SimpleIRParser::ReferenceContext * ctx) override {
    int sourceOffset = symtab[ctx-> operand -> getText()];
    string offset = to_string(symtab[ctx-> variable -> getText()]);

    cout << "\tmovq\t%rbp, %rax" << endl;
    cout << "\tadd " << "$" << sourceOffset << ", %rax" << endl;
    cout << "\tmovq\t%rax, " << offset << "(%rbp)" << endl;
  }
  
  virtual void enterAssignDereference(SimpleIRParser::AssignDereferenceContext * ctx) override {
    string operand = operand_to_string(ctx-> operand);
    string pointerOffset = to_string(symtab[ctx-> variable -> getText()]);

    cout << "\tmov " << pointerOffset << "(%rbp), %rax" << endl;
    cout << "\tmov " << operand << ", %rbx" << endl;
    cout << "\tmov\t%rbx, (%rax)" << endl;
  }
};

int main(int argc, const char* argv[]) {
  ANTLRInputStream *input;

  if (argc > 1) {
    ifstream fstream;
    filename = argv[1];
    fstream.open(filename);
    ANTLRInputStream *finput = new ANTLRInputStream(fstream);
    input = finput;
  } else {
    filename = "stdin";
    input = new ANTLRInputStream(cin);
  }

  SimpleIRLexer lexer(input);
  CommonTokenStream tokens(&lexer);
  SimpleIRParser parser(&tokens);

  tree::ParseTree *tree = parser.unit();
  CodeGen listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return 0;
}

