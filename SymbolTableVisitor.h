/*
  Filename   : SymbolTableVisitor.h
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - Symbol Table Visitor header
  Description: Header file for Symbol Table Visitor class to
               walk AST and pair uses with declarations.
*/

#ifndef SYMBOL_VISITOR_H
#define SYMBOL_VISITOR_H

/************************************************************/
// System includes


/************************************************************/
// Local includes

#include "SymbolTable.h"

/************************************************************/
// Using declarations


/************************************************************/
// Function prototypes/global vars/type definitions

class SymbolTableVisitor : public IVisitor
{
public:
  
  SymbolTableVisitor ();
  ~SymbolTableVisitor ();

  virtual void visit (ProgramNode* node);
  
  
private:
  //member functions
  void error (DeclarationNode* decl);
  void error (ParameterNode* param);
  void error (VariableExpressionNode* exp);
  void error (CallExpressionNode* call);
  

  virtual void visit (DeclarationNode* node);
  virtual void visit (FunctionDeclarationNode* node);
  virtual void visit (VariableDeclarationNode* node);
  virtual void visit (ArrayDeclarationNode* node);
  virtual void visit (ParameterNode* node);

  virtual void visit (StatementNode* node);
  virtual void visit (CompoundStatementNode* node);
  virtual void visit (IfStatementNode* node);
  virtual void visit (WhileStatementNode* node);
  
  virtual void visit (ReturnStatementNode* node);
  virtual void visit (ExpressionStatementNode* node);

  virtual void visit (ExpressionNode* node);
  virtual void visit (AssignmentExpressionNode* node);
  virtual void visit (VariableExpressionNode* node);
  virtual void visit (SubscriptExpressionNode* node);
  virtual void visit (CallExpressionNode* node);
  virtual void visit (AdditiveExpressionNode* node);
  virtual void visit (MultiplicativeExpressionNode* node);
  virtual void visit (RelationalExpressionNode* node);
  virtual void visit (UnaryExpressionNode* node);
  virtual void visit (IntegerLiteralExpressionNode* node);


  //member variables
  SymbolTable            m_table;
  vector<ParameterNode*> m_funcParams;
  bool                   m_isFunc;
  bool                   m_errorFlag;
};

#endif
