/* 

Filename   : Lexer.h
Author     : Andrew W. Curtin
Course     : CSCI 435
Assignment : Lab 11 - OO Lexer - Header
Description: Header file for Lexer object - includes 
             token type defs, Token object definition,
             and Lexer function declarations.
	     Unchanged from Lab 7.
*/

/************************************************************/
// Macro guard to prevent multiple inclusions

#ifndef LEXER_H
#define LEXER_H

/************************************************************/
// System includes

#include <string>

/************************************************************/
// Local includes

/************************************************************/
// Using declarations

/************************************************************/
// Function prototypes/global vars/type definitions 

enum TokenType
{
  // Special tokens
  END_OF_FILE, ERROR,

  // Keywords
  IF, ELSE, INT, VOID, RETURN, WHILE,

  // Operators
  PLUS, MINUS, TIMES, DIVIDE, LT, LTE, GT, GTE, EQ, NEQ, ASSIGN,

    INCREMENT, DECREMENT,

  // Punctuators
  SEMI, COMMA, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE,

  // Identifier and integer literals
  ID, NUM
};

/***********************************************************************/

struct Token
{
  Token (TokenType pType = END_OF_FILE,
	 std::string pLexeme = "",
	 int pNumber = 0)
      : type (pType), lexeme (pLexeme), number (pNumber)
  {  }

  TokenType   type;
  std::string lexeme;
  int         number;
};

/***********************************************************************/

class Lexer
{
public:
  Lexer (FILE* sourceFile);

  Token
  getToken ();

  const int
  getRow ();

  const int
  getCol ();

private:
  void
  getChar ();

  void
  ungetChar ();

  void
  checkWhiteSpace ();

  void
  processComment ();

private:
  FILE* m_sourceFile;
  int   m_nextChar;
  int   m_numRow;
  int   m_numCol;
  
};

/***********************************************************************/

#endif
