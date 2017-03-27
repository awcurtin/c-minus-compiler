/********************************************************************/
// Assembly Emitter
//   Utility class to write assembly instructions to a file
//
// CSCI 435: Compilers class
// Fall 2014, 2016

/********************************************************************/
// System includes

#include <iostream>
#include <iomanip>

/********************************************************************/
// Local includes

#include "AssemblyEmitter.h"

/********************************************************************/
// Using declarations

using std::setw;
using std::setfill;
using std::endl;
using std::string;

using uint = unsigned int;

/********************************************************************/
// Constants for code formatting

const uint INDENT_WIDTH = 10;
const uint INSTRUCTION_WIDTH = 10;
const uint OPERANDS_WIDTH = 22;
const uint COMMENT_INDENT_WIDTH = INDENT_WIDTH
                                + INSTRUCTION_WIDTH
                                + OPERANDS_WIDTH;
const uint FULL_WIDTH = 80;

/********************************************************************/

AssemblyEmitter::AssemblyEmitter (std::ofstream &strm)
  : m_outFile (strm), m_labelCounter (0)
{
  m_outFile << std::left;
}

AssemblyEmitter::~AssemblyEmitter ()
{}

// ...

/********************************************************************/
// Utilities

string
AssemblyEmitter::createUniqueLabel ()
{
  return ".L" + std::to_string (m_labelCounter++);
}

/********************************************************************/
// Generic Emission methods

void
AssemblyEmitter::emitComment (const string& comment)
{
  m_outFile << setw (COMMENT_INDENT_WIDTH) << " "
	    << "# " << comment << endl;
}

/********************************************************************/

void
AssemblyEmitter::emitLabel (const string& label, const string& comment)
{
  m_outFile << setw (COMMENT_INDENT_WIDTH) << label + ":"
	  << "# " << comment << endl;
}

/********************************************************************/

void
AssemblyEmitter::emitInstruction (const string& operation,
				  const string& operands,
				  const string& comment)
{
  m_outFile << setw (INDENT_WIDTH) << " "
	    << setw (10) << operation << setw (22) << operands
	    << "# " << comment << endl;
}

void
AssemblyEmitter::emitFunctionDeclaration (const std::string& name)
{
  //should be good, check
  emitSeparator ();
  emitComment ("processing function: " + name);
  emitInstruction (".text");
  m_outFile << (".globl " + name + "\n" + name + ":\n");
}

void
AssemblyEmitter::emitPrologue (int localVarSpace)
{
  //function prologue, plus allocate local var space
  emitInstruction ("pushl", "%ebp", "Function prologue");
  emitInstruction ("movl", "%esp, %ebp");

  //not sure about this?
  emitComment ("-> compound statement");
  emitInstruction ("subl", "$" + std::to_string (localVarSpace) +
		   ", %esp", "allocate local var space");
}

void
AssemblyEmitter::emitEpilogue ()
{
  emitComment ("<- compound statement");
  //can use "leave," but trying not to
  emitInstruction ("movl", "%ebp, %esp", "leaving: reset stack pointer");
  emitInstruction ("popl", "%ebp", "reset base pointer");
  emitInstruction ("ret", "", "return to caller");
  emitComment ("<- fundecl");
}
// ...

/********************************************************************/
// Concrete emission methods

void
AssemblyEmitter::emitSeparator ()
{
  m_outFile << setfill ('=')
	    << setw (FULL_WIDTH) << "#" << endl
	    << setfill (' ');
}

void
AssemblyEmitter::emitHeader (const string& name)
{
  emitSeparator ();
  emitComment ("C-Minus Compilation to IA-32 Code");
  emitComment ("File: " + name + ".s");
  emitSeparator ();
}

void
AssemblyEmitter::emitFooter ()
{
  //Conversion Strings
  emitSeparator ();
  emitComment ("Conversion strings");
  emitInstruction (".section", "  .rodata");
  m_outFile << ".inStr:   .string   \"%d\"\n.outStr:  .string   \"%d\\n\"\n";

  emitInputFunction ();
  emitOutputFunction ();

  emitSeparator ();
  emitComment ("YOU LOSE! GOOD DAY, SIR!");
}

void
AssemblyEmitter::emitInputFunction ()
{
  //FINISH LATER
  emitSeparator ();
  emitComment ("Code for input routine");
  emitFunctionDeclaration ("input");
  emitPrologue (4);

  emitInstruction ("pushl", "%esp", "push slot's address");
  emitInstruction ("pushl", "$.inStr", "push conversion spec");
  emitInstruction ("call", "scanf", "read an integer");
  emitInstruction ("movl", "8(%esp), %eax", "move int to %eax");

  emitEpilogue ();
  
  /*
  emitInstruction (".text");

  m_outFile << ".globl input\ninput:\n";

  emitInstruction ("pushl", "");
  */
}

void
AssemblyEmitter::emitOutputFunction ()
{
  //FINISH LATER
  emitSeparator ();
  emitComment ("Code for output routine");
  emitFunctionDeclaration ("output");
  emitPrologue (0);

  emitInstruction ("pushl", "8(%ebp)", "get int to print");
  emitInstruction ("pushl", "$.outStr", "push conversion spec");
  emitInstruction ("call", "printf", "invoke printf");
  emitEpilogue ();
}
// ...
