/*
  Filename   : Parser.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - C- Parser 
  Description: A predictive, recursive descent parser class for
               C-. Is LL(1) except for expression grammar
	       and look-ahead in factor.

*/

/************************************************************/
// System includes

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

/************************************************************/
// Local includes

#include "Lexer.h"
#include "Parser.h"

/************************************************************/
// Using declarations

using std::cout;
using std::endl;
using std::string;
using std::vector;

/************************************************************/
// Function prototypes/global vars/type definitions


/*********************************************************************/

Parser::Parser (FILE* sourceFile)
  : m_lex (Lexer (sourceFile)), list (), g_token (0)
{
  Token tok = m_lex.getToken ();  
  TokenLoc tokLoc = TokenLoc (tok, m_lex.getRow (), m_lex.getCol ());
  list.push_back (tokLoc);

  while (tokLoc.token.type != END_OF_FILE)
  {
    tok = m_lex.getToken ();
    int row = m_lex.getRow ();
    int col = m_lex.getCol ();
    
    list.push_back (tokLoc = TokenLoc (tok, row, col));
  }
}

/*********************************************************************/

void
Parser::parse ()
{
  m_tree = program (); 
}

/*********************************************************************/

ProgramNode*
Parser::getTree ()
{
  return m_tree;
}

/*********************************************************************/

void
Parser::match (TokenType type, string state, string lex) 
{
  if (list[g_token].token.type == type)
  {
    ++g_token;
  }
  else
  {
    error (state, lex);
  }
    
}

/*********************************************************************/

void
Parser::error (string state, string lex)
{
  cout << "Error while parsing \'" << state << "\' on line "
       << list[g_token].row << ", column " << list[g_token].col
       << endl << "Encountered \'" << list[g_token].token.lexeme
       << "\'" << endl << "  while expecting \'" << lex << "\'"
       << endl;

  exit (1);
}

/*********************************************************************/

ProgramNode*
Parser::program ()
{
  vector<DeclarationNode*> declist = declarationList ();
  ProgramNode* p = new ProgramNode (declist);
  return p;
}

/*********************************************************************/

vector<DeclarationNode*>
Parser::declarationList ()
{
  vector<DeclarationNode*> decList;
  decList.push_back (declaration ());
  
  while (list[g_token].token.type == INT
	 || list[g_token].token.type == VOID)
  {
    decList.push_back (declaration ());
  }
  
  if (list[g_token].token.type != END_OF_FILE)
  {
    error ("declaration list", "end of file");
  }

  return decList;
}

/*********************************************************************/

DeclarationNode*
Parser::declaration ()
{
  ValueType t = typeSpecifier ();
  string id = list[g_token].token.lexeme;

  int row = list[g_token].row;
  int col = list[g_token].col;

  match (ID, "declaration", "identifier");

  DeclarationNode* decl;
  
  if (list[g_token].token.type == LPAREN)
  {
    decl = functionDeclaration ();
  }
  else
  {
    decl = variableDeclaration ();
  }

  decl->valueType = t;
  decl->identifier = id;
  decl->row = row;
  decl->col = col;
  
  return decl;
}

/*********************************************************************/

ValueType
Parser::typeSpecifier ()
{
  ValueType t;
  
  if (list[g_token].token.type == INT)
  {
    match (INT, "type", "int");
    t = ValueType::INT;
  }
  else if (list[g_token].token.type == VOID)
  {
    match (VOID, "type", "void");
    t = ValueType::VOID;
  }
  else
  {
    error ("type", "int/void");
  }

  return t;
}

/*********************************************************************/

FunctionDeclarationNode*
Parser::functionDeclaration ()
{
  match (LPAREN, "function", "(");
  vector<ParameterNode*> params = parameters ();
  match (RPAREN, "function", ")");
  
  CompoundStatementNode* body = compoundStatement ();

  FunctionDeclarationNode* funcDec = new FunctionDeclarationNode (ValueType::INT,
								  "", params, body);
  return funcDec;
}

/*********************************************************************/

VariableDeclarationNode*
Parser::variableDeclaration ()
{
  VariableDeclarationNode* var;
  
  if (list[g_token].token.type == LBRACK)
  {
    match (LBRACK, "variable", "[");

    size_t num = list[g_token].token.number;
    match (NUM, "variable", "numerical");

    match (RBRACK, "variable", "]");

    var = new ArrayDeclarationNode ("", num);
  }
  else
  {
    var = new VariableDeclarationNode ("");
  }

  match (SEMI, "variable declaration", ";");

  return var;
}

/*********************************************************************/

vector<ParameterNode*>
Parser::parameters ()
{
  vector<ParameterNode*> params;
  
  if (list[g_token].token.type == VOID)
  {
    match (VOID, "function parameters", "void");
  }
  else
  {
    paramList (params);
  }

  return params;
}

/*********************************************************************/

CompoundStatementNode*
Parser::compoundStatement ()
{
  match (LBRACE, "compound statement", "{");
  vector<VariableDeclarationNode*> localDecs = localDeclarations ();
  vector<StatementNode*> stmtList = statementList ();
  match (RBRACE, "compound statement", "}");

  CompoundStatementNode* cStmt = new CompoundStatementNode (localDecs, stmtList);
  return cStmt;
}

/*********************************************************************/

void
Parser::paramList (vector<ParameterNode*> &params)
{
  params.push_back (param ());

  while (list[g_token].token.type == COMMA)
  {
    match (COMMA, "parameter list", ",");
    params.push_back (param ());
  }
}

/*********************************************************************/

vector<VariableDeclarationNode*>
Parser::localDeclarations ()
{
  vector<VariableDeclarationNode*> localDecls;
  
  //stay in local-decs while seeing type signifiers
  while (list[g_token].token.type == INT
	 || list[g_token].token.type == VOID)
  {
    ValueType t = typeSpecifier ();
    string id = list[g_token].token.lexeme;
    int row = list[g_token].row;
    int col = list[g_token].col;
    
    match (ID, "declaration", "identifier");

    VariableDeclarationNode* varDecl = variableDeclaration ();
    varDecl->identifier = id;
    varDecl->valueType = t;
    varDecl->row = row;
    varDecl->col = col;
    
    localDecls.push_back (varDecl);
  }

  return localDecls;
}

/*********************************************************************/

vector<StatementNode*>
Parser::statementList ()
{
  vector<StatementNode*> stmtList;
  
  //stay in statement list until '}' seen
  while (list[g_token].token.type != RBRACE)
  {
    stmtList.push_back (statement ());
  }

  return stmtList;
}

/*********************************************************************/

ParameterNode*
Parser::param ()
{
  ValueType t = typeSpecifier ();
  string id = list[g_token].token.lexeme;
  int row = list[g_token].row;
  int col = list[g_token].col;

  match (ID, "parameter", "identifier");

  ParameterNode* p = new ParameterNode (id, false);
  p->valueType = t;
  
  if (list[g_token].token.type == LBRACK)
  {
    match (LBRACK, "parameter", "[");
    match (RBRACK, "parameter", "]");
    p->isArray = true;
  }

  p->row = row;
  p->col = col;
  
  return p;
}

/*********************************************************************/

StatementNode*
Parser::statement ()
{
  StatementNode* stmt;
  
  if (list[g_token].token.type == LBRACE)
  {
    stmt = compoundStatement ();
  }
  else if (list[g_token].token.type == IF)
  {
    stmt = selectionStatement ();
  }
  else if (list[g_token].token.type == WHILE)
  {
    stmt = iterationStatement ();
  }
  else if (list[g_token].token.type == RETURN)
  {
    stmt = returnStatement ();
  }
  else
  {
    stmt = expressionStatement ();
  }

  return stmt;
}

/*********************************************************************/

IfStatementNode*
Parser::selectionStatement ()
{
  match (IF, "if statement", "if");
  match (LPAREN, "if statement", "(");
  ExpressionNode* expr = expression ();
  match (RPAREN, "if statement", ")");
  StatementNode* thenStmt = statement ();

  IfStatementNode* ifStmt = new IfStatementNode (expr, thenStmt);
  
  if (list[g_token].token.type == ELSE)
  {
    match (ELSE, "if statement", "else");
    ifStmt->elseStatement = statement ();
  }

  return ifStmt;
}

/*********************************************************************/

WhileStatementNode*
Parser::iterationStatement ()
{
  match (WHILE, "while", "while");
  match (LPAREN, "while", "(");
  ExpressionNode* conditional = expression ();
  match (RPAREN, "while", ")");
  StatementNode* body = statement ();

  WhileStatementNode* whileStmt = new WhileStatementNode (conditional, body);

  return whileStmt;
}

/*********************************************************************/

ReturnStatementNode*
Parser::returnStatement ()
{
  match (RETURN, "return", "return");
  ReturnStatementNode* retStmt = new ReturnStatementNode ();
  
  if (list[g_token].token.type != SEMI)
  {
    retStmt->expression = expression ();
  }

  match (SEMI, "return", ";");

  return retStmt;
}

/*********************************************************************/

ExpressionStatementNode*
Parser::expressionStatement ()
{
  ExpressionStatementNode* expStmt = new ExpressionStatementNode ();
  
  if (list[g_token].token.type != SEMI)
  {
    expStmt->expression = expression ();
  }

  match (SEMI, "expression stmt", ";");

  return expStmt;
}

/*********************************************************************/

ExpressionNode*
Parser::expression ()
{
  ExpressionNode* exp;
  
  if (list[g_token].token.type != ID)
  {
    exp = simpleExpression ();
  }
  else
  {
    //next token == ID

    //setup: save current token, restore at end
    size_t currentToken = g_token;

    //look-ahead #2
    ++g_token;

    //encountered EOF before expression terminated
    if (list[g_token].token.type == END_OF_FILE)
    {
      error ("expression", "; / ) / ] / ,");
    }

    //checking for ASSIGN and LBRACK
    if (list[g_token].token.type == ASSIGN)
    {
      //found ASSIGN, reset g_token
      g_token = currentToken;

      //production --> var = expression
      VariableExpressionNode* var = variable ();
      match (ASSIGN, "expression", "=");
      ExpressionNode* rightSide = expression ();

      exp = new AssignmentExpressionNode (var, rightSide);
    }
    else if (list[g_token].token.type == LBRACK)
    {
      //tracking brackets for match
      int countBrack = 1;

      //look ahead #3
      //g_token is now past '['
      ++g_token;

      //walk through token stream, look for matching brackets
      while (list[g_token].token.type != END_OF_FILE && countBrack > 0)
      {
	if (list[g_token].token.type == RBRACK)
	{
	  --countBrack;
	}

	if (list[g_token].token.type == LBRACK)
	{
	  ++countBrack;
	}

	++g_token;

	//these tokens shouldn't be in expression
	//abort early if found
	if (list[g_token].token.type == SEMI
	    || list[g_token].token.type == LBRACE
	    || list[g_token].token.type == RBRACE)
	{
	  break;
	}
	
      }

      //successfully matched brackets
      //g_token is one past ']'
      if (countBrack == 0)
      {
	
	if (list[g_token].token.type == ASSIGN)
	{
	  //restore token pointer
	  g_token = currentToken;

	  //production --> var = expression
	  VariableExpressionNode* var = variable ();
	  match (ASSIGN, "expression", "=");
	  ExpressionNode* rightSide = expression ();

	  exp = new AssignmentExpressionNode (var, rightSide);
	}
	else
	{
	  //restore token pointer
	  g_token = currentToken;

	  //production --> simple-exp
	  exp = simpleExpression ();
	}
	
      }
      else
      {
	//should cover all cases differing from the ones above
	g_token = currentToken;

	//production --> simple-exp
	exp = simpleExpression ();
      }
      
    }
    else
    {
      //ID followed by anything but ASSIGN or []
      //restore token pointer
      g_token = currentToken;

      //production --> simple-exp
      exp = simpleExpression ();
    }
    
  }

  return exp;
}

/*********************************************************************/

VariableExpressionNode*
Parser::variable ()
{
  VariableExpressionNode* var;
  
  string id = list[g_token].token.lexeme;
  int row = list[g_token].row;
  int col = list[g_token].col;
  
  match (ID, "variable", "id");

  if (list[g_token].token.type == LBRACK)
  {
    match (LBRACK, "variable", "[");
    ExpressionNode* index = expression ();
    match (RBRACK, "variable", "]");

    var = new SubscriptExpressionNode (id, index);
    var->row = row;
    var->col = col;
    return var;
  }

  var = new VariableExpressionNode (id);
  var->row = row;
  var->col = col;
  
  return var;
}

/*********************************************************************/
 
ExpressionNode*
Parser::simpleExpression ()
{
  ExpressionNode* simpExp = additiveExpression ();

  if (list[g_token].token.type == LT
      || list[g_token].token.type == LTE
      || list[g_token].token.type == GT
      || list[g_token].token.type == GTE
      || list[g_token].token.type == EQ
      || list[g_token].token.type == NEQ)
  {
    RelationalOperatorType relop;
    
    switch (list[g_token].token.type)
    {
      case LT:
	relop = RelationalOperatorType::LT;
	break;
      case LTE:
	relop = RelationalOperatorType::LTE;
	break;
      case GT:
	relop = RelationalOperatorType::GT;
	break;
      case GTE:
	relop = RelationalOperatorType::GTE;
	break;
      case EQ:
	relop = RelationalOperatorType::EQ;
	break;
      case NEQ:
	relop = RelationalOperatorType::NEQ;
	break;
      default:
	error ("simple expression", "<, <=, >, >=, ==, or !=");
    }
    
    match (list[g_token].token.type, "boolean exp", "boolean op");
    ExpressionNode* rhs = additiveExpression ();

    simpExp = new RelationalExpressionNode (relop, simpExp, rhs);
  }

  return simpExp;
}

/*********************************************************************/
 
ExpressionNode*
Parser::additiveExpression ()
{
  ExpressionNode* exp = term ();

  while (list[g_token].token.type == PLUS
	 || list[g_token].token.type == MINUS)
  {
    AdditiveOperatorType op = addop ();
    ExpressionNode* rhs = term ();
    exp = new AdditiveExpressionNode (op, exp, rhs);
  }

  return exp;
}

/*********************************************************************/
 
AdditiveOperatorType
Parser::addop ()
{
  AdditiveOperatorType op;
  
  if (list[g_token].token.type == PLUS)
  {
    match (PLUS, "add", "+");
    op = AdditiveOperatorType::PLUS;
  }
  else if (list[g_token].token.type == MINUS)
  {
    match (MINUS, "subtract", "-");
    op = AdditiveOperatorType::MINUS;
  }
  else
  {
    error ("additive operator type", "+ or -");
  }

  return op;
}

/*********************************************************************/
 
ExpressionNode*
Parser::term ()
{
  ExpressionNode* exp = factor ();

  while (list[g_token].token.type == TIMES
	 || list[g_token].token.type == DIVIDE)
  {
    MultiplicativeOperatorType op = mulop ();
    ExpressionNode* rhs = factor ();
    exp = new MultiplicativeExpressionNode (op, exp, rhs);
  }

  return exp;
}

/*********************************************************************/
 
MultiplicativeOperatorType
Parser::mulop ()
{
  MultiplicativeOperatorType op;
  
  if (list[g_token].token.type == TIMES)
  {
    match (TIMES, "mul op", "*");
    op = MultiplicativeOperatorType::TIMES;
  }
  else if (list[g_token].token.type == DIVIDE)
  {
    match (DIVIDE, "mul op", "/");
    op = MultiplicativeOperatorType::DIVIDE;
  }
  else
  {
    error ("multiplicative operator", "* or /");
  }

  return op;
}

/*********************************************************************/
 
ExpressionNode*
Parser::factor ()
{
  ExpressionNode* exp;
  
  if (list[g_token].token.type == NUM)
  {
    exp = new IntegerLiteralExpressionNode (list[g_token].token.number);
    match (NUM, "factor", "numerical");
  }
  else if (list[g_token].token.type == LPAREN)
  {
    match (LPAREN, "factor", "(");
    exp = expression ();
    match (RPAREN, "factor", ")");
  }
  else if (list[g_token].token.type == ID)
  {
    //ad hoc look-ahead past ID
    if (((g_token + 1) < list.size ())
	&& list[g_token + 1].token.type == LPAREN)
    {
      exp = call ();
    }
    else
    {
      exp = variable ();
    }
  }
  else
  {
    error ("factor", "NUM/ID/(");
  }

  return exp;
}

/*********************************************************************/
 
CallExpressionNode*
Parser::call ()
{
  string id = list[g_token].token.lexeme;
  int row = list[g_token].row;
  int col = list[g_token].col;
  
  match (ID, "function call", "id");
  
  match (LPAREN, "function call", "(");
  vector<ExpressionNode*> args = arguments ();
  match (RPAREN, "function call", ")");

  CallExpressionNode* callExp = new CallExpressionNode (id, args);
  callExp->row = row;
  callExp->col = col;
  
  return callExp;
}

/*********************************************************************/
 
vector<ExpressionNode*>
Parser::arguments ()
{
  vector<ExpressionNode*> args;
  
  if (list[g_token].token.type != RPAREN)
  {
    argumentList (args);
  }

  return args;
}

/*********************************************************************/
 
void
Parser::argumentList (vector<ExpressionNode*> &args)
{
  args.push_back (expression ());

  while (list[g_token].token.type == COMMA)
  {
    match (COMMA, "argument list", ",");
    args.push_back (expression ());
  }
}

/*********************************************************************/
