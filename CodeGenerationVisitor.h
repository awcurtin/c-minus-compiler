/*
  Filename   : CodeGenerationVisitor.h
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Final Compiler
  Description: CodeGenerationVisitor header file
*/

#ifndef CODE_GEN_VISITOR_H
#define CODE_GEN_VISITOR_H

/************************************************************/
// System includes


/************************************************************/
// Local includes

#include "CMinusAst.h"
#include "AssemblyEmitter.h"

/************************************************************/
// Using declarations


/************************************************************/
// Function prototypes/global vars/type definitions

class CodeGenerationVisitor : public IVisitor
{
public:
  CodeGenerationVisitor (std::ofstream& outfile, const string& name);
  ~CodeGenerationVisitor ();

  virtual void visit (ProgramNode* node);

 
  
private:
  
  bool isParam (DeclarationNode* parent);
  
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

  
  AssemblyEmitter m_emitter;
  bool            m_isGlobal;
  bool            m_isFunc;
  int             m_offset;
};


#endif
