/*
  Filename   : SymbolTableVisitor.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - Symbol Table Visitor implementation
  Description: Implementation of Symbol Table Visitor class
               to pair uses and declarations in a C- AST.
*/

/************************************************************/
// System includes
#include <iostream>

/************************************************************/
// Local includes

#include "SymbolTableVisitor.h"

/************************************************************/
// Using declarations

using std::cout;
using std::endl;

/************************************************************/
// Function prototypes/global vars/type definitions

SymbolTableVisitor::SymbolTableVisitor ()
  : m_table (), m_funcParams (), m_isFunc (false),
    m_errorFlag (false)
{
  //adding input and output
  ValueType type = ValueType::INT;
  string id = "input";
  vector<ParameterNode*> parameters;
  CompoundStatementNode* body = nullptr;
  FunctionDeclarationNode* input = new FunctionDeclarationNode (type, id, parameters, body);
  input->nestLevel = 0;
  
  //add int input (void) { ... } at global scale
  m_table.insert (input);

  id = "output";
  ParameterNode* param = new ParameterNode ("x", false);
  param->valueType = type;
  parameters.push_back (param);
  type = ValueType::VOID;

  FunctionDeclarationNode* output = new FunctionDeclarationNode (type, id, parameters, body);
  output->nestLevel = 0;

  //add void output (int x) { ... } at global scale
  m_table.insert (output);
}

SymbolTableVisitor::~SymbolTableVisitor ()
{}

//Error messages
void
SymbolTableVisitor::error (DeclarationNode* decl)
{
  DeclarationNode* declared = m_table.lookup (decl->identifier);
  int decRow = declared->row;
  int decCol = declared->col;
  
  cout << "Error: multiple declaration:" << endl
       << "\"" << decl->identifier << "\" at line "
       << decl->row << ", column " << decl->col
       << " previously declared at line " << decRow
       << ", column " << decCol << endl << endl; 

  m_errorFlag = true;
}

void
SymbolTableVisitor::error (ParameterNode* param)
{
  DeclarationNode* declared = m_table.lookup (param->identifier);
  int decRow = declared->row;
  int decCol = declared->col;
  
  cout << "Error: duplicate parameter declaration:" << endl
       << "\"" << param->identifier << "\" at line "
       << param->row << ", column " << param->col
       << " previously declared at line " << decRow
       << ", column " << decCol << endl << endl;

  m_errorFlag = true;
}

void
SymbolTableVisitor::error (VariableExpressionNode* exp)
{ 
  cout << "Error: undeclared variable:" << endl
       << "\"" << exp->identifier << "\" at line "
       << exp->row << ", column " << exp->col
       << " not previously declared." << endl
       << endl;

  m_errorFlag = true;
}

void
SymbolTableVisitor::error (CallExpressionNode* call)
{ 
  cout << "Error: undeclared function call:" << endl
       << "\"" << call->identifier << "\" at line "
       << call->row << ", column " << call->col
       << " not previously declared." << endl
       << endl;

  m_errorFlag = true;
}


/*********************************************/
//Program Node
void
SymbolTableVisitor::visit (ProgramNode* node)
{
  for (auto decl : node->declarations)
  {
    decl->accept (this);
  }

  if (m_errorFlag)
  {
    cout << "Unsuccessful exit. Terminating compilation." << endl;
    exit (1);
  }
}

//Declaration Nodes
void
SymbolTableVisitor::visit (DeclarationNode* node)
{
  node->accept (this);
}

void
SymbolTableVisitor::visit (FunctionDeclarationNode* node)
{
  bool inserted = m_table.insert (node);
  if (!inserted)
  {
    error (node);
  }

  m_funcParams = node->parameters;
  m_isFunc = true;

  node->functionBody->accept (this);

  m_funcParams.clear ();
  m_isFunc = false;
  
}

void
SymbolTableVisitor::visit (VariableDeclarationNode* node)
{
  DeclarationNode* found = m_table.lookup (node->identifier);
  if (found != nullptr)
  {
    error (node);
  }

  m_table.insert (node);
}

void
SymbolTableVisitor::visit (ArrayDeclarationNode* node)
{
  DeclarationNode* found = m_table.lookup (node->identifier);
  if (found != nullptr)
  {
    error (node);
  }

  m_table.insert (node);
}


void
SymbolTableVisitor::visit (ParameterNode* node)
{  
}

//Statement Nodes
void
SymbolTableVisitor::visit (StatementNode* node)
{
  node->accept (this);
}

void
SymbolTableVisitor::visit (CompoundStatementNode* node)
{
  m_table.enterScope ();

  //add parameters if function definition
  if (m_isFunc)
  {
    for (auto param : m_funcParams)
    {
      bool inserted = m_table.insert (param);
      if (!inserted)
      {
	//duplicate parameter error
	error (param);
      }
    }
    m_isFunc = false;
  }
  
  for (auto decl : node->localDeclarations)
  {
    decl->accept (this);
  }

  for (auto stmt : node->statements)
  {
    stmt->accept (this);
  }
  
  m_table.exitScope ();
}

void
SymbolTableVisitor::visit (IfStatementNode* node)
{
  node->conditionalExpression->accept (this);
  node->thenStatement->accept (this);

  if (node->elseStatement != nullptr)
  {
    node->elseStatement->accept (this);
  }
}

void
SymbolTableVisitor::visit (WhileStatementNode* node)
{
  node->conditionalExpression->accept (this);
  node->body->accept (this);
}

void
SymbolTableVisitor::visit (ReturnStatementNode* node)
{
  if (node->expression != nullptr)
  {
    node->expression->accept (this);
  }
}

void
SymbolTableVisitor::visit (ExpressionStatementNode* node)
{
  if (node->expression != nullptr)
  {
    node->expression->accept (this);
  }
}

//Expression Nodes
void
SymbolTableVisitor::visit (ExpressionNode* node)
{
  node->accept (this);
}

void
SymbolTableVisitor::visit (AssignmentExpressionNode* node)
{
  node->variable->accept (this);
  node->expression->accept (this);
}

void
SymbolTableVisitor::visit (VariableExpressionNode* node)
{
  DeclarationNode* found = m_table.lookup (node->identifier);
  if (found == nullptr)
  {
    //undeclared variable error
    error (node);
  }

  node->parentDecl = found;
}

void
SymbolTableVisitor::visit (SubscriptExpressionNode* node)
{
  DeclarationNode* found = m_table.lookup (node->identifier);
  if (found == nullptr)
  {
    //undeclared variable error
    error (node);
  }
  
  node->parentDecl = found;

  node->index->accept (this);
}

void
SymbolTableVisitor::visit (CallExpressionNode* node)
{
  DeclarationNode* found = m_table.lookup (node->identifier);
  if (found == nullptr)
  {
    //undeclared function error
    error (node);
  }

  node->parentDecl = found;

  for (auto arg : node->arguments)
  {
    arg->accept (this);
  }
}

void
SymbolTableVisitor::visit (AdditiveExpressionNode* node)
{
  node->left->accept (this);
  node->right->accept (this);
}

void
SymbolTableVisitor::visit (MultiplicativeExpressionNode* node)
{
  node->left->accept (this);
  node->right->accept (this);
}

void
SymbolTableVisitor::visit (RelationalExpressionNode* node)
{
  node->left->accept (this);
  node->right->accept (this);
}

void
SymbolTableVisitor::visit (UnaryExpressionNode* node)
{}

void
SymbolTableVisitor::visit (IntegerLiteralExpressionNode* node)
{}
