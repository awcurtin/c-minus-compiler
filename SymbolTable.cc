/*
  Filename   : SymbolTable.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Lab 11 - SymbolTable implementation
  Description: Implementation of Symbol Table class to
               track scope of functions, parameters,
               and variables in C- programs for 
               annotated AST generation.
*/

/************************************************************/
// System includes
#include <iostream>

/************************************************************/
// Local includes

#include "SymbolTable.h"

/************************************************************/
// Using declarations

using std::cout;
using std::endl;

/************************************************************/
// Function prototypes/global vars/type definitions

SymbolTable::SymbolTable ()
  : m_nestLevel (0), m_table ()
{
  m_table.push_back (std::make_unique<ScopeTable> ());
}

SymbolTable::~SymbolTable ()
{
  m_table.clear ();
}

int
SymbolTable::getNestLevel () const
{
  return m_nestLevel;
}


void
SymbolTable::enterScope ()
{
  m_table.push_back (std::make_unique<ScopeTable> ());
  ++m_nestLevel;
}


void
SymbolTable::exitScope ()
{
  m_table.pop_back ();
  --m_nestLevel;
}


bool
SymbolTable::insert (DeclarationNode* declarationPtr)
{
  //How's that for obfuscation?
  bool inserted = m_table[m_nestLevel]->insert ({declarationPtr->identifier, declarationPtr}).second;

  if (inserted)
  {
    declarationPtr->nestLevel = m_nestLevel;
  }

  return inserted;
}


DeclarationNode*
SymbolTable::lookup (std::string name)
{
  for (auto i = m_table.rbegin (); i != m_table.rend (); ++i)
  {
    auto searchIter = (*i)->find (name);
    if (searchIter != (*i)->end ())
    {
      return searchIter->second;
    }
  }
  
  return nullptr;
}
