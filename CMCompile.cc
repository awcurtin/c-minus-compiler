/*
  Filename   : CMCompile.cc
  Author     : Andrew Curtin
  Course     : CSCI 435
  Assignment : Final Compiler - Driver
  Description: Driver for C- compiler, currently producing
               annotated AST for valid programs in a .ast
               output file.
*/

/************************************************************/
// System includes

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/************************************************************/
// Local includes

#include "Parser.h"
#include "SymbolTableVisitor.h"
#include "CodeGenerationVisitor.h"

/************************************************************/
// Using declarations

using std::cout;
using std::endl;
using std::string;
using std::ofstream;

/************************************************************/
// Function prototypes/global vars/type definitions

/************************************************************/

int 
main (int argc, char* argv[])
{
  ++argv;
  --argc;

  FILE* input;
  bool isStdin = false;
  string fileName;

  if (argc > 0)
  {
    input = fopen (argv[0], "r");
  }
  else
  {
    input = stdin;
    isStdin = true;
    fileName = "Default";
  }

  if (input == NULL)
  {
    cout << "Error: File not found.\n";
    return EXIT_FAILURE;
  }

  if (!isStdin)
  {
    fileName = string (argv[0]);
    fileName = fileName.substr (0, fileName.find_last_of ('.'));
    //fileName += ".ast";
  }
  
  Parser p (input);
  p.parse ();

  SymbolTableVisitor symVis;
  symVis.visit (p.getTree ());

  cout << "Program is valid!\nWriting AST to \"" + fileName + ".ast"  + "\"" << endl;

  PrintVisitor visitor;
  visitor.visit (p.getTree ());

  
  ofstream output (fileName + ".ast", ofstream::trunc);
  output << visitor.getTree ();
  output.close ();
  
  ofstream genOutput (fileName + ".s", ofstream::trunc);

  CodeGenerationVisitor cGenVis (genOutput, fileName);
  cGenVis.visit (p.getTree ());

  cout << "Writing executable to: " << fileName << endl;
  
  string gccCall = "gcc -m32 -g " + fileName + ".s" + " -o " + fileName;

  int result = system (gccCall.data ());

  if (result == -1)
  {
    cout << "Error in assembly. Compilation failed." << endl;
  }
  else
  {
    cout << "Compilation of " << fileName << " succesfull." << endl; 
  }
  
  
  return EXIT_SUCCESS;
}

/*********************************************************************/
