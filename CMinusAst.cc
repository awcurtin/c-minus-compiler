/*
  Filename   : CMinusAst.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - Symbol Table - Ast implementation
  Description: Definition of Node class structure, visitor
               class structure, and PrintVisitor.
*/

/************************************************************/
// System includes

#include <iostream>

/************************************************************/
// Local includes

#include "CMinusAst.h"

/************************************************************/
// Using declarations

using std::cout;
using std::endl;
using std::to_string;

/************************************************************/
// Function prototypes/global vars/type definitions

//Node base class
Node::~Node ()
{}

/************************************************************/
//Program Node: root

ProgramNode::ProgramNode (vector<DeclarationNode*> declarations)
  : declarations (declarations)
{}

ProgramNode::~ProgramNode ()
{
  for (auto decl: declarations)
  {
    delete decl;
  }

  declarations.clear ();
}

void
ProgramNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

/************************************************************/
//Declaration Nodes

//DeclarationNode
DeclarationNode::DeclarationNode (ValueType t, string id)
  : valueType (t), identifier (id)
{}

DeclarationNode::~DeclarationNode ()
{}

//FunctionDeclarationNode
FunctionDeclarationNode::FunctionDeclarationNode
(ValueType t, string id,
 vector<ParameterNode*> params,
 CompoundStatementNode* body)
  : DeclarationNode (t, id),
    parameters (params),
    functionBody (body)
{}

FunctionDeclarationNode::~FunctionDeclarationNode ()
{
  delete functionBody;

  for (auto param: parameters)
  {
    delete param;
  }

  parameters.clear ();
}

void
FunctionDeclarationNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//VariableDeclarationNode 
VariableDeclarationNode::VariableDeclarationNode (string id)
  : DeclarationNode (ValueType::INT, id)
{}

VariableDeclarationNode::~VariableDeclarationNode ()
{}

void
VariableDeclarationNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//ArrayDeclarationNode
ArrayDeclarationNode::ArrayDeclarationNode (string id, size_t size)
  : VariableDeclarationNode (id), size (size)
{}

ArrayDeclarationNode::~ArrayDeclarationNode ()
{}

void
ArrayDeclarationNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//ParameterNode
ParameterNode::ParameterNode (string id, bool array)
  : DeclarationNode (ValueType::INT, id),
    isArray (array)
{}

ParameterNode::~ParameterNode ()
{}

void
ParameterNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

/************************************************************/
//Statement Nodes

//Statement Base class
StatementNode::~StatementNode ()
{}

void
StatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//CompoundStatementNode
CompoundStatementNode::CompoundStatementNode
(vector<VariableDeclarationNode*> decls,
 vector<StatementNode*> stmts)
  : localDeclarations (decls), statements (stmts)
{}

CompoundStatementNode::~CompoundStatementNode ()
{
  for (auto decl: localDeclarations)
  {
    delete decl;
  }
  localDeclarations.clear ();

  for (auto stmt: statements)
  {
    delete stmt;
  }
  statements.clear ();
}

void
CompoundStatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//IfStatementNode
IfStatementNode::IfStatementNode (ExpressionNode* expr,
				  StatementNode* thenStmt,
				  StatementNode* elseStmt)
  : conditionalExpression (expr), thenStatement (thenStmt),
    elseStatement (elseStmt)
{}

IfStatementNode::~IfStatementNode ()
{
  delete conditionalExpression;
  delete thenStatement;
  delete elseStatement;
}

void
IfStatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//WhileStatementNode
WhileStatementNode::WhileStatementNode (ExpressionNode* expr,
					StatementNode* stmt)
  :conditionalExpression (expr), body (stmt)
{}

WhileStatementNode::~WhileStatementNode ()
{
  delete conditionalExpression;
  delete body;
}

void
WhileStatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//ReturnStatementNode
ReturnStatementNode::ReturnStatementNode (ExpressionNode* expr)
  : expression (expr)
{}

ReturnStatementNode::~ReturnStatementNode ()
{
  delete expression;
}

void
ReturnStatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//ExpressionStatementNode
ExpressionStatementNode::ExpressionStatementNode (ExpressionNode* expr)
  : expression (expr)
{}

ExpressionStatementNode::~ExpressionStatementNode ()
{
  delete expression;
}

void
ExpressionStatementNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

/************************************************************/
//Expression Nodes

//ExpressionNode base class
ExpressionNode::~ExpressionNode ()
{}

void
ExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//AssignmentExpressionNode
AssignmentExpressionNode::AssignmentExpressionNode
(VariableExpressionNode* var, ExpressionNode* expr)
  : variable (var), expression (expr)
{}

AssignmentExpressionNode::~AssignmentExpressionNode ()
{
  delete variable;
  delete expression;
}

void
AssignmentExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//VariableExpressionNode
VariableExpressionNode::VariableExpressionNode (string id)
  : identifier (id)
{}

VariableExpressionNode::~VariableExpressionNode ()
{}

void
VariableExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//SubscriptExpressionNode
SubscriptExpressionNode::SubscriptExpressionNode (string id,
						  ExpressionNode* index)
  : VariableExpressionNode (id), index (index)
{}

SubscriptExpressionNode::~SubscriptExpressionNode ()
{
  delete index;
}

void
SubscriptExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//CallExpressionNode
CallExpressionNode::CallExpressionNode (string id,
					vector<ExpressionNode*> args)
  : identifier (id), arguments (args)
{}

CallExpressionNode::~CallExpressionNode ()
{
  for (auto arg : arguments)
  {
    delete arg;
  }
  arguments.clear ();
}

void
CallExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//AdditiveExpressionNode
AdditiveExpressionNode::AdditiveExpressionNode (AdditiveOperatorType addop,
						ExpressionNode* lhs,
						ExpressionNode* rhs)
  : addOperator (addop), left (lhs), right (rhs)
{}

AdditiveExpressionNode::~AdditiveExpressionNode ()
{
  delete left;
  delete right;
}

void
AdditiveExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//MultiplicativeExpressionNode
MultiplicativeExpressionNode::MultiplicativeExpressionNode
(MultiplicativeOperatorType mulop, ExpressionNode* lhs, ExpressionNode* rhs)
  : multOperator (mulop), left (lhs), right (rhs)
{}

MultiplicativeExpressionNode::~MultiplicativeExpressionNode ()
{
  delete left;
  delete right;
}

void
MultiplicativeExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//RelationalExpressionNode
RelationalExpressionNode::RelationalExpressionNode
(RelationalOperatorType relop, ExpressionNode* lhs, ExpressionNode* rhs)
  : relationalOperator (relop), left (lhs), right (rhs)
{}

RelationalExpressionNode::~RelationalExpressionNode ()
{
  delete left;
  delete right;
}

void
RelationalExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//UnaryExpressionNode
UnaryExpressionNode::UnaryExpressionNode (UnaryOperatorType unaryOp,
					  VariableExpressionNode* var)
  : unaryOperator (unaryOp), variable (var)
{}

UnaryExpressionNode::~UnaryExpressionNode ()
{
  delete variable;
}

void
UnaryExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

//IntegerLiteralExpressionNode
IntegerLiteralExpressionNode::IntegerLiteralExpressionNode (int value)
  : value (value)
{}

IntegerLiteralExpressionNode::~IntegerLiteralExpressionNode ()
{}

void
IntegerLiteralExpressionNode::accept (IVisitor* visitor)
{
  visitor->visit (this);
}

/************************************************************/
//PrintVisitor

PrintVisitor::PrintVisitor (string start, int ind)
  : tree (start), indentL (ind)
{}

PrintVisitor::~PrintVisitor ()
{}

string
PrintVisitor::getTree () const
{
  return tree;
}

void
PrintVisitor::indent ()
{
  for (int i = 0; i < indentL; ++i)
  {
    tree += "  ";
  }
}

void
PrintVisitor::visit (ProgramNode* node)
{
  tree += "Program Node\n";
  ++indentL;
  
  for (auto decl : node->declarations)
  {
    decl->accept (this);
  }
}

void
PrintVisitor::visit (DeclarationNode* node)
{
  node->accept (this);
}

void
PrintVisitor::visit (FunctionDeclarationNode* node)
{
  indent ();
  tree += "Function: " + node->identifier + ": ";

  if (node->valueType == ValueType::VOID)
  {
    tree += "Void type\n";
  }
  else
  {
    tree += "Int type\n";
  }

  ++indentL;

  for (auto param : node->parameters)
  {
    param->accept (this);
  }

  if (node->functionBody != NULL)
  {
    node->functionBody->accept (this);
  }

  --indentL;
}

void
PrintVisitor::visit (VariableDeclarationNode* node)
{
  indent ();
  ValueType t = node->valueType;
  string type = "Int type\n";
  if (t == ValueType::VOID)
  {
    type = "Void type\n";
  }
  
  
  tree += "VariableDeclaration: " + node->identifier + ": " + type; 
}

void
PrintVisitor::visit (ArrayDeclarationNode* node)
{
  indent ();
  ValueType t = node->valueType;
  string type = "Int type\n";
  if (t == ValueType::VOID)
  {
    type = "Void type\n";
  }
  
  string arrSize = to_string (node->size);
  tree += "Variable: " + node->identifier + "[" + arrSize + "]: " + type;
}

void
PrintVisitor::visit (ParameterNode* node)
{
  indent ();
  tree += "Paramater: " + node->identifier;

  if (node->isArray)
  {
    tree += "[]";
  }
  ValueType t = node->valueType;
  string type = "Int";

  if (t == ValueType::VOID)
  {
    type = "Void";
  }
  
  tree += ": " + type + " type\n";
}

void
PrintVisitor::visit (StatementNode* node)
{
  node->accept (this);
}

void
PrintVisitor::visit (CompoundStatementNode* node)
{
  indent ();
  tree += "CompoundStatement\n";

  ++indentL;

  for (auto decl : node->localDeclarations)
  {
    decl->accept (this);
  }

  for (auto stmt : node->statements)
  {
    stmt->accept (this);
  }

  --indentL;
}

void
PrintVisitor::visit (IfStatementNode* node)
{
  indent ();
  tree += "If\n";

  ++indentL;
  node->conditionalExpression->accept (this);
  node->thenStatement->accept (this);

  if (node->elseStatement != nullptr)
  {
    node->elseStatement->accept (this);
  }

  --indentL;
}

void
PrintVisitor::visit (WhileStatementNode* node)
{
  indent ();
  tree += "While\n";

  ++indentL;

  node->conditionalExpression->accept (this);
  node->body->accept (this);

  --indentL;
}

void
PrintVisitor::visit (ReturnStatementNode* node)
{
  indent ();
  tree += "Return\n";

  if (node->expression != nullptr)
  {
    ++indentL;
    node->expression->accept (this);
    --indentL;
  }  
}

void
PrintVisitor::visit (ExpressionStatementNode* node)
{
  indent ();

  tree += "ExpressionStatement\n";
  ++indentL;

  if (node->expression != nullptr)
  {
    node->expression->accept (this);
  }
  --indentL;
}

void
PrintVisitor::visit (ExpressionNode* node)
{
  node->accept (this);
}

void
PrintVisitor::visit (AssignmentExpressionNode* node)
{
  indent ();
  tree += "Assignment\n";

  ++indentL;
  node->variable->accept (this);
  node->expression->accept (this);
  --indentL;
}

void
PrintVisitor::visit (VariableExpressionNode* node)
{
  indent ();

  string type = "";
  
  switch (node->parentDecl->valueType)
  {
    case ValueType::INT:
      type = "Int";
      break;
    default:
      type = "Void";
  }
  
  tree += "Variable: " + node->identifier + ": " + type + " type\n";
}

void
PrintVisitor::visit (SubscriptExpressionNode* node)
{
  indent ();
  
  string type = "";
  
  switch (node->parentDecl->valueType)
  {
    case ValueType::INT:
      type = "Int";
      break;
    default:
      type = "Void";
  }
  
  tree += "Subscript: " + node->identifier + ": " + type + " type\n";

  ++indentL;
  indent ();
  tree += "Index:\n";
  ++indentL;
  
  node->index->accept (this);
  --indentL;

  --indentL;
}

void
PrintVisitor::visit (CallExpressionNode* node)
{
  indent ();

  string type = "";
  
  switch (node->parentDecl->valueType)
  {
    case ValueType::INT:
      type = "Int";
      break;
    default:
      type = "Void";
  }
  
  tree += "FunctionCall: " + node->identifier + ": " + type + " type\n";

  ++indentL;
  indent ();
  tree += "Arguments:\n";

  ++indentL;
  for (auto arg : node->arguments)
  {
    arg->accept (this);
  }
  --indentL;

  --indentL;
}

void
PrintVisitor::visit (AdditiveExpressionNode* node)
{
  indent ();

  tree += "AdditiveExpression: ";
  
  if (node->addOperator == AdditiveOperatorType::PLUS)
  {
    tree += "+\n";
  }
  else
  {
    tree += "-\n";
  }
  ++indentL;

  indent ();
  tree += "Left:\n";
  ++indentL;

  node->left->accept (this);

  --indentL;

  indent ();
  tree += "Right:\n";
  ++indentL;

  node->right->accept (this);
  --indentL;

  --indentL;
}

void
PrintVisitor::visit (MultiplicativeExpressionNode* node)
{
  indent ();

  tree += "MultiplicativeExpression: ";
  
  if (node->multOperator == MultiplicativeOperatorType::TIMES)
  {
    tree += "*\n";
  }
  else
  {
    tree += "/\n";
  }
  ++indentL;

  indent ();
  tree += "Left:\n";
  ++indentL;

  node->left->accept (this);

  --indentL;

  indent ();
  tree += "Right:\n";
  ++indentL;

  node->right->accept (this);
  --indentL;

  --indentL;

}

void
PrintVisitor::visit (RelationalExpressionNode* node)
{
  indent ();

  tree += "RelationalExpression: ";
  
  if (node->relationalOperator == RelationalOperatorType::LT)
  {
    tree += "<\n";
  }
  else if (node->relationalOperator == RelationalOperatorType::LTE)
  {
    tree += "<=\n";
  }
  else if (node->relationalOperator == RelationalOperatorType::GT)
  {
    tree += ">\n";
  }
  else if (node->relationalOperator == RelationalOperatorType::GTE)
  {
    tree += ">=\n";
  }
  else if (node->relationalOperator == RelationalOperatorType::EQ)
  {
    tree += "==\n";
  }
  else
  {
    tree += "!=\n";
  }
  ++indentL;

  indent ();
  tree += "Left:\n";
  ++indentL;

  node->left->accept (this);

  --indentL;

  indent ();
  tree += "Right:\n";
  ++indentL;

  node->right->accept (this);
  --indentL;

  --indentL;

}

void
PrintVisitor::visit (UnaryExpressionNode* node)
{
}

void
PrintVisitor::visit (IntegerLiteralExpressionNode* node)
{
  indent ();

  tree += "Integer: " + to_string (node->value) + "\n";
}
