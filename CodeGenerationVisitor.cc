/*
  Filename   : CodeGenerationVisitor.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Final Compiler - CodeGen
  Description: CodeGenerationVisitor Implementation file
*/

/************************************************************/
// System includes

//debug
#include <iostream>

/************************************************************/
// Local includes

#include "CodeGenerationVisitor.h"

/************************************************************/
// Using declarations

//debug
using std::cout;
using std::endl;

/************************************************************/
// Function prototypes/global vars/type definitions

CodeGenerationVisitor::CodeGenerationVisitor (std::ofstream& outfile,
					      const std::string& name)
  : m_emitter (outfile), m_isGlobal (true), m_isFunc (false) , m_offset (0)
{
  m_emitter.emitHeader (name);
  cout << "Completed construction" << endl;
}

CodeGenerationVisitor::~CodeGenerationVisitor ()
{
  cout << "Activated dtor" << endl;
}

bool
CodeGenerationVisitor::isParam (DeclarationNode* parent)
{
  cout << "entering isParam" << endl;
  return (parent->isParam == 1);
  cout << "leaving isParam" << endl;
}

//Program Node
void
CodeGenerationVisitor::visit (ProgramNode* node)
{
  cout << "entering ProgramNode*" << endl;
  for (auto decl : node->declarations)
  {
    decl->accept (this);
  }
  m_emitter.emitFooter ();
  cout << "exiting ProgramNode*" << endl;
}

//Declaration Nodes
void
CodeGenerationVisitor::visit (DeclarationNode* node)
{
  cout << "Entering DeclNode*" << endl;
  node->accept (this);
  cout << "Exiting DeclNode*" << endl;
}

void
CodeGenerationVisitor::visit (FunctionDeclarationNode* node)
{
  cout << "Entering FuncDeclNode*" << endl;
  m_isGlobal = false;

  //load up function header and prologue
  m_emitter.emitFunctionDeclaration (node->identifier);

  //function flag set for compound statement
  m_isFunc = true;
  
  //compound statement
  node->functionBody->accept (this);

  m_isFunc = false;

  m_offset = 0;

  cout << "Exiting FuncDeclNode*" << endl;
}


void
CodeGenerationVisitor::visit (VariableDeclarationNode* node)
{
  cout << "Entering VarDeclNode*" << endl;
  if (m_isGlobal)
  {
    string id = node->identifier;
    m_emitter.emitComment ("-> allocating global var: " + id);
    m_emitter.emitInstruction (".comm", id + ", 4", "bss space for global");
    m_emitter.emitComment ("<- vardecl");
  }
  else
  {
    //decrement m_offset, give to node's offset member
    m_offset += 4;
    node->framePointerOffset = m_offset;
  }

  cout << "Exiting VarDeclNode*" << endl;
}

void
CodeGenerationVisitor::visit (ArrayDeclarationNode* node)
{
  cout << "Entering ArrayDeclNode*" << endl;
  if (m_isGlobal)
  {
    string id = node->identifier;
    m_emitter.emitComment ("-> allocating global array: " + id);

    string numBytes = std::to_string (4 * node->size);
    
    m_emitter.emitInstruction (".comm", id + ", " + numBytes, "bss space for global");
    m_emitter.emitComment ("<- arraydecl");
  }
  else
  {
    //increment m_offset, give to node's offset member
    m_offset += 4;
    node->framePointerOffset = m_offset;

    m_offset += 4 * (node->size - 1);
  }

  cout << "Exiting ArrayDeclNode*" << endl;
}

void
CodeGenerationVisitor::visit (ParameterNode* node)
{
  cout << "Visited ParmeterNode*" << endl;
}

//Statement Nodes
void
CodeGenerationVisitor::visit (StatementNode* node)
{
  cout << "Entering StatementNode*" << endl;
  node->accept (this);
  cout << "Exiting StatementNode*" << endl;
}

void
CodeGenerationVisitor::visit (CompoundStatementNode* node)
{
  cout << "Entering CompoundStatementNode*" << endl;
  if (m_isFunc)
  {
    m_isFunc = false;

    for (auto decl : node->localDeclarations)
    {
      decl->accept (this);
    }

    m_emitter.emitPrologue (m_offset);

    for (auto stmt : node->statements)
    {
      stmt->accept (this);
    }

    m_emitter.emitInstruction ("addl", "$" + std::to_string (m_offset) + ", %esp",
			       "deallocate local var space");
    m_offset = 0;

    m_emitter.emitEpilogue ();
    m_emitter.emitComment ("<- fundecl");
    
  }
  else
  {
    int startOffset = m_offset;

    //accumulate var space
    for (auto decl : node->localDeclarations)
    {
      decl->accept (this);
    }

    int localOffset = m_offset - startOffset;

    //allocate local space
    m_emitter.emitComment ("-> compound statement: non-function");
    m_emitter.emitInstruction ("subl", "$" + std::to_string (localOffset) + ", %esp",
			       "allocate local var space");
    
    for (auto stmt : node->statements)
    {
      stmt->accept (this);
    }

    //exiting compound statement
    m_offset -= localOffset;
    m_emitter.emitInstruction ("addl", "$" + std::to_string (localOffset) + ", %esp",
			       "deallocate local var space");
    
    m_emitter.emitComment ("<- compound statement");
  }

  cout << "Exiting CompoundStatementNode*" << endl;
 
}

void
CodeGenerationVisitor::visit (IfStatementNode* node)
{
  cout << "Entering IfStatementNode*" << endl;
  m_emitter.emitComment ("-> if statement");

  node->conditionalExpression->accept (this);
  m_emitter.emitInstruction ("popl", "%eax", "val into %eax");
  
  if (node->elseStatement == nullptr)
  {
    string endLabel = m_emitter.createUniqueLabel ();
    m_emitter.emitInstruction ("test", "%eax, %eax", "checking expression eval");
    m_emitter.emitInstruction ("je", endLabel, "jump to end");

    m_emitter.emitComment ("true branch");
    node->thenStatement->accept (this);

    m_emitter.emitLabel (endLabel, "end label");
  }
  else
  {
    string elseLabel = m_emitter.createUniqueLabel ();
    string endLabel = m_emitter.createUniqueLabel ();
    m_emitter.emitInstruction ("test", "%eax, %eax", "checking expression eval");
    m_emitter.emitInstruction ("je", elseLabel, "jump to else");
    
    m_emitter.emitComment ("true branch");
    node->thenStatement->accept (this);
    m_emitter.emitInstruction ("jmp", endLabel, "jump to end");

    m_emitter.emitLabel (elseLabel, "else label");
    node->elseStatement->accept (this);

    m_emitter.emitLabel (endLabel, "end label");
  }

  m_emitter.emitComment ("<- if statement");

  cout << "Exiting IfStatementNode*" << endl;
}

void
CodeGenerationVisitor::visit (WhileStatementNode* node)
{
  cout << "Entering WhileStatementNode*" << endl;
  m_emitter.emitComment ("-> while statement");
  
  string expLabel = m_emitter.createUniqueLabel ();
  string endLabel = m_emitter.createUniqueLabel ();
  m_emitter.emitLabel (expLabel, "exp label");
  node->conditionalExpression->accept (this);

  m_emitter.emitInstruction ("test", "%eax, %eax", "checking expression eval");
  m_emitter.emitInstruction ("je", endLabel, "jump to end");

  node->body->accept (this);
  m_emitter.emitInstruction ("jmp", expLabel, "jump to test");

  m_emitter.emitLabel (endLabel, "post-loop");
  m_emitter.emitComment ("<- while statement");

  cout << "Exiting WhileStatementNode*" << endl;
}

void
CodeGenerationVisitor::visit (ReturnStatementNode* node)
{
  cout << "Entering ReturnStatementNode*" << endl;
  m_emitter.emitComment ("-> return statement");

  if (node->expression != nullptr)
  {
    node->expression->accept (this);
    m_emitter.emitInstruction ("popl", "%eax", "return val in %eax");
  }

  m_emitter.emitEpilogue ();
  
  m_emitter.emitComment ("<- return statement");

  cout << "Exiting ReturnStatementNode*" << endl;
}

void
CodeGenerationVisitor::visit (ExpressionStatementNode* node)
{
  cout << "Entering ExpressionStatementNode*" << endl;
  if (node->expression != nullptr)
  {
    node->expression->accept (this);
  }
  cout << "Exiting ExpressionStatementNode*" << endl;
}

//Expression Nodes
void
CodeGenerationVisitor::visit (ExpressionNode* node)
{
  cout << "Entering ExpressionNode*" << endl;
  node->accept (this);
  cout << "Exiting ExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (AssignmentExpressionNode* node)
{
  cout << "Entering AssignmentExpressionNode*" << endl;
  m_emitter.emitComment ("-> assignment exp");

  node->expression->accept (this);
  m_emitter.emitInstruction ("popl", "%eax", "rhs into %eax");
  node->variable->accept (this);
  m_emitter.emitInstruction ("popl", "%ecx", "lhs into %eax");

  m_emitter.emitInstruction ("movl", "%eax, (%ecx)", "assign to lhs");
  
  m_emitter.emitComment ("<- assignment exp");
  cout << "Exiting AssignmentExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (VariableExpressionNode* node)
{
  cout << "Entering VariableExpressionNode*" << endl;
  string name = node->identifier;
  m_emitter.emitComment ("-> variable Expression: " + name);
  
  if (isParam (node->parentDecl))
  {
    string offset = std::to_string (8 + node->parentDecl->index * 4);
    m_emitter.emitInstruction ("leal", offset + "(%ebp), %eax", "load address of " + name);
  }
  else
  {
    string offset = std::to_string (node->parentDecl->framePointerOffset * -1);
    m_emitter.emitInstruction ("leal", offset + "(%ebp), %eax", "load address of " + name);
  }

  m_emitter.emitInstruction ("pushl", "%eax", "save address of " + name + " to stack");

  m_emitter.emitComment ("<- variable Expression");
  cout << "Exiting VariableExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (SubscriptExpressionNode* node)
{
  cout << "Entering SubscriptExpressionNode*" << endl;
  string name = node->identifier;
  m_emitter.emitComment ("-> subscript Expression");
  node->index->accept (this);

  m_emitter.emitInstruction ("popl", "%ebx", "grab subscript off stack");
  if (isParam (node->parentDecl))
  {
    string offset = std::to_string (8 + node->parentDecl->index * 4);
    m_emitter.emitInstruction ("leal", offset + "(%ebp), %eax", "load address of " + name);
  }
  else
  {
    string offset = std::to_string (node->parentDecl->framePointerOffset * -1);
    m_emitter.emitInstruction ("leal", offset + "(%ebp), %eax", "load address of " + name);
  }
  
  m_emitter.emitInstruction ("movl", "(%eax, %ebx, 4), %eax", "access array");
  m_emitter.emitInstruction ("pushl", "%eax", "store %eax");
  
  m_emitter.emitComment ("<- subscript Expression");
  cout << "Exiting SubscriptExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (CallExpressionNode* node)
{
  cout << "Entering CallExpressionNode*" << endl;
  m_emitter.emitComment ("-> function call");
  
  string name = node->identifier;

  for (auto arg : node->arguments)
  {
    arg->accept (this);
  }


  if (name.compare ("output") == 0)
  {
    m_emitter.emitInstruction ("call", "output", "output call");
  }
  else if (name.compare ("input") == 0)
  {
    m_emitter.emitInstruction ("call", "input", "input call");
    m_emitter.emitInstruction ("pushl", "%eax", "save %eax");
  }
  else
  {
    m_emitter.emitInstruction ("call", node->identifier, "function call");
  }

  m_emitter.emitInstruction ("pushl", "%eax");
  m_emitter.emitComment ("<- function call");
  cout << "Exiting CallExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (AdditiveExpressionNode* node)
{
  cout << "Entering AdditiveExpressionNode*" << endl;
  if (node->addOperator == AdditiveOperatorType::PLUS)
  {
    m_emitter.emitComment ("-> add op");
    node->left->accept (this);
    m_emitter.emitInstruction ("popl", "%ecx", "pop lhs");
    node->right->accept (this);
    m_emitter.emitInstruction ("popl", "%eax", "pop rhs");
    m_emitter.emitInstruction ("addl", "%ecx, %eax", "add");
    m_emitter.emitInstruction ("pushl", "%eax", "store value");
    
    m_emitter.emitComment ("<- add op");
  }
  else
  {
    //MINUS
    m_emitter.emitComment ("-> sub op");
    node->left->accept (this);
    m_emitter.emitInstruction ("popl", "%eax", "pop lhs");
    node->right->accept (this);
    m_emitter.emitInstruction ("popl", "%ecx", "pop rhs");
    m_emitter.emitInstruction ("subl", "%ecx, %eax", "subtract");
    m_emitter.emitInstruction ("pushl", "%eax", "store value");
    
    m_emitter.emitComment ("<- sub op");
  }

  cout << "Exiting AdditiveExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (MultiplicativeExpressionNode* node)
{
  cout << "Entering MultiplicativeExpressionNode*" << endl;
  MultiplicativeOperatorType op = node-> multOperator;

  if (op == MultiplicativeOperatorType::TIMES)
  {
    m_emitter.emitComment ("-> mul op");
    node->left->accept (this);
    m_emitter.emitInstruction ("popl", "%ecx", "pop lhs");
    node->right->accept (this);
    m_emitter.emitInstruction ("popl", "%eax", "pop rhs");
    m_emitter.emitInstruction ("imul", "%ecx, %eax", "multiply");
    m_emitter.emitInstruction ("pushl", "%eax", "store value");
    
    m_emitter.emitComment ("<- mul op");
  }
  else
  {
    //DIVIDE
    m_emitter.emitComment ("-> div op");
    node->left->accept (this);
    m_emitter.emitInstruction ("popl", "%eax", "pop lhs");
    m_emitter.emitInstruction ("cltd", "", "sign extend %edx:%eax");
    node->right->accept (this);
    m_emitter.emitInstruction ("popl", "%ecx", "pop rhs");
    m_emitter.emitInstruction ("idiv", "%ecx", "divide");
    m_emitter.emitInstruction ("pushl", "%eax", "store value");
    
    m_emitter.emitComment ("<- div op");
  }

  cout << "Exiting MultiplicativeExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (RelationalExpressionNode* node)
{
  cout << "Entering RelationalExpressionNode*" << endl;
  m_emitter.emitComment ("<-- rel exp");
  
  node->left->accept (this);
  m_emitter.emitInstruction ("popl", "%eax", "pop lhs");
  node->right->accept (this);
  m_emitter.emitInstruction ("popl", "%ecx", "pop rhs");
  string falseLabel;
  string endLabel;
  
  switch (node->relationalOperator)
  {    
    case RelationalOperatorType::LT:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs < rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("jle", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;

    case RelationalOperatorType::LTE:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs <= rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("jl", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;

    case RelationalOperatorType::GT:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs > rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("jge", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;
    case RelationalOperatorType::GTE:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs >= rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("jg", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;

    case RelationalOperatorType::NEQ:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs != rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("je", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;

    case RelationalOperatorType::EQ:
      m_emitter.emitInstruction ("cmp", "%eax, %ecx", "lhs == rhs");
      falseLabel = m_emitter.createUniqueLabel ();
      endLabel = m_emitter.createUniqueLabel ();
      m_emitter.emitInstruction ("jne", falseLabel , "false branch");
      m_emitter.emitInstruction ("movl", "$1, %eax", "true");
      m_emitter.emitInstruction ("jmp", endLabel, "jmp to end");
      m_emitter.emitLabel (falseLabel);
      m_emitter.emitInstruction ("movl", "$0, %eax", "false");
      m_emitter.emitLabel (endLabel);
      break;
  }
  m_emitter.emitInstruction ("pushl", "%eax", "store %eax");
  m_emitter.emitComment ("<-- rel exp");

  cout << "Exiting RelationalExpressionNode*" << endl;
}

void
CodeGenerationVisitor::visit (UnaryExpressionNode* node)
{
}

void
CodeGenerationVisitor::visit (IntegerLiteralExpressionNode* node)
{
  cout << "Entering IntegerLiteralExpressionNode*" << endl;
  m_emitter.emitComment ("-> int exp");
  string num = std::to_string (node->value);
  m_emitter.emitInstruction ("movl", "$" + num + ", %eax", "constant int expression");
  m_emitter.emitInstruction ("pushl", "%eax", "save %eax");
  m_emitter.emitComment ("<- int exp");
  cout << "Exiting IntegerLiteralExpressionNode*" << endl;
}
