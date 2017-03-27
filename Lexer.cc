/*

Filename   : Lexer.cc
Author     : Andrew Curtin
Course     : CSCI 435
Assignment : Lab 11 - OO Lexer - Class Body
Description: Definitions for Lexer class functions
             Unchanged from Lab 7

*/

/************************************************************/
// System includes

#include <iostream>
#include <cstdio>
#include <string>
#include <ctype.h>

/************************************************************/
// Local includes

#include "Lexer.h"

/************************************************************/
// Using declarations

using std::cout;
using std::string;
using std::stoi;
using std::left;

/************************************************************/
// Function prototypes/global vars/type definitions
#define NUMKW 6

static Token keywords[NUMKW] = { {IF, "if"}, {ELSE, "else"},
				 {INT, "int"}, {VOID, "void"},
		                 {RETURN, "return"},
				 {WHILE, "while"} };

/************************************************************/
// Member function definitions

Lexer::Lexer (FILE* sourceFile)
  : m_sourceFile (sourceFile), m_nextChar (0),
    m_numRow (1), m_numCol (0)
{}

/************************************************************/

Token
Lexer::getToken ()
{
  string lexeme = "";
  getChar ();

  checkWhiteSpace ();
  processComment ();

  //process IDs and keywords
  if (isalpha (m_nextChar))
  {
    lexeme += m_nextChar;

    getChar ();

    while (isalpha (m_nextChar))
    {
      lexeme += m_nextChar;
      getChar ();
    }

    ungetChar ();

    //check for keywords
    for (int i = 0; i < NUMKW; ++i)
    {
      if (lexeme == keywords[i].lexeme)
      {
	return keywords[i];
      }
    }

    return Token (ID, lexeme);
  }

  //process NUM
  if (isdigit (m_nextChar))
  {
    lexeme += m_nextChar;

    getChar ();

    while (isdigit (m_nextChar))
    {
      lexeme += m_nextChar;
      getChar ();
    }

    ungetChar ();

    return Token (NUM, lexeme, stoi (lexeme));
  }

  //process operators, punctuators, eof, error
  switch (m_nextChar)
  {
    case '+':
      //check for increment
      getChar ();
      if (m_nextChar == '+')
      {
	return Token (INCREMENT, "++");
      }
      else
      {
	ungetChar ();
	return Token (PLUS, "+");
      }
      

    case '-':
      //check for decrement
      getChar ();
      if (m_nextChar == '-')
      {
	return Token (DECREMENT, "--");
      }
      else
      {
	ungetChar ();
	return Token (MINUS, "-");
      }
      

    case '*':

      return Token (TIMES, "*");

    case '/':

      return Token (DIVIDE, "/");

    case '<':
      //check LTE
      getChar ();
      if (m_nextChar == '=')
      {
	return Token (LTE, "<=");
      }
      else
      {
	ungetChar ();
	return Token (LT, "<");
      }

    case '>':
      //check GTE
      getChar ();

      if (m_nextChar == '=')
      {
	return Token (GTE, ">=");
      }
      else
      {
	ungetChar ();
	return Token (GT, ">");
      }


    case '=':
      //check EQ
      getChar ();

      if (m_nextChar == '=')
      {
	return Token (EQ, "==");
      }
      else
      {
	ungetChar ();
	return Token (ASSIGN, "=");
      }

    case '!':
      //check NEQ
      getChar ();

      if (m_nextChar == '=')
      {
	return Token (NEQ, "!=");
      }
      else
      {
	ungetChar ();
	return Token (ERROR, "!");
      }

    case ';':

      return Token (SEMI, ";");

    case ',':

      return Token (COMMA, ",");

    case '(':

      return Token (LPAREN, "(");

    case ')':

      return Token (RPAREN, ")");

    case '[':

      return Token (LBRACK, "[");

    case ']':

      return Token (RBRACK, "]");

    case '{':

      return Token (LBRACE, "{");

    case '}':

      return Token (RBRACE, "}");

    case EOF:

      return Token (END_OF_FILE, "EOF");

    default:

      lexeme += m_nextChar;
      return Token (ERROR, lexeme);
    }

}

/************************************************************/

void
Lexer::getChar ()
{
  m_nextChar = fgetc (m_sourceFile);
  ++m_numCol;
}

/************************************************************/

void
Lexer::ungetChar ()
{
  ungetc (m_nextChar, m_sourceFile);
  --m_numCol;
}

/************************************************************/

void
Lexer::checkWhiteSpace ()
{
  while (m_nextChar == ' '
	 || m_nextChar == '\t'
	 || m_nextChar == '\n')
  {
    switch (m_nextChar)
    {
      case ' ':
	break;

      case '\t':
	m_numCol += 7;
	break;

      case '\n':
	++m_numRow;
	m_numCol = 0;
	break;
    }

    getChar ();
  }
}

/************************************************************/

void
Lexer::processComment ()
{
  if (m_nextChar != '/')
  {
    return;
  }
  else
  {
    getChar ();
    if (m_nextChar != '*')
    {
      ungetChar ();
      m_nextChar = '/';
      return;
    }
    else
    {
      getChar ();
      while (m_nextChar != EOF)
      {
	checkWhiteSpace ();
	if (m_nextChar == '*')
	{
	  getChar();
	  if (m_nextChar == '/')
	  {
	    getChar ();
	    break;
	  }
	}
	else
	{
	  getChar ();
	}
      }

      checkWhiteSpace ();
      processComment ();
    }
  }
}

/************************************************************/

const int
Lexer::getRow ()
{
  return m_numRow;
}

/************************************************************/

const int
Lexer::getCol ()
{
  return m_numCol;
}

/************************************************************/
