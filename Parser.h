/*
  Filename   : Parser.h
  Author     : Andrew W. Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - C- Parser AST - Header 
  Description: Header file for recursive descent C- parser
               class for building C- AST.
*/   

/************************************************************/
// Macro guard to prevent multiple inclusions

#ifndef PARSER_H         
#define PARSER_H

/************************************************************/
// System includes

#include <vector>
#include <cstdlib>
#include <string>

/************************************************************/
// Local includes

#include "Lexer.h"
#include "CMinusAst.h"

/************************************************************/
// Using declarations

using std::vector;
using std::string;

/************************************************************/
// Function prototypes/global vars/type definitions

struct TokenLoc
{
  TokenLoc (Token pToken = Token (), int pRow = 0, int pCol = 0)
  : token (pToken), row (pRow), col (pCol)
  {}

  Token token;
  int   row;
  int   col;

};

//Typedefs
typedef vector<TokenLoc> TokenList;

/************************************************************/
//Parser class

class Parser
{

public:

  Parser (FILE* sourceFile);
  void parse ();
  ProgramNode* getTree ();
  
private:
  
  void
  match (TokenType type, string state,
	 string lex);

  void
  error (string state, string lex);

  ProgramNode*
  program ();

  vector<DeclarationNode*>
  declarationList ();

  DeclarationNode*
  declaration ();

  ValueType
  typeSpecifier ();

  FunctionDeclarationNode*
  functionDeclaration ();

  VariableDeclarationNode*
  variableDeclaration ();

  vector<ParameterNode*>
  parameters ();

  CompoundStatementNode*
  compoundStatement ();

  void
  paramList (vector<ParameterNode*> &params);

  vector<VariableDeclarationNode*>
  localDeclarations ();

  vector<StatementNode*>
  statementList ();

  ParameterNode*
  param ();

  StatementNode*
  statement ();

  IfStatementNode*
  selectionStatement ();

  WhileStatementNode*
  iterationStatement ();

  ReturnStatementNode*
  returnStatement ();

  ExpressionStatementNode*
  expressionStatement ();

  ExpressionNode*
  expression ();

  VariableExpressionNode*
  variable ();

  ExpressionNode*
  simpleExpression ();

  RelationalOperatorType
  relop ();

  ExpressionNode*
  additiveExpression ();

  AdditiveOperatorType
  addop ();

  ExpressionNode*
  term ();

  MultiplicativeOperatorType
  mulop ();

  ExpressionNode*
  factor ();

  CallExpressionNode*
  call ();

  vector<ExpressionNode*>
  arguments ();

  void
  argumentList (vector<ExpressionNode*> &args);

private:

  Lexer          m_lex;
  TokenList       list;
  size_t       g_token;
  ProgramNode*  m_tree;

};
/************************************************************/

#endif

/************************************************************/
