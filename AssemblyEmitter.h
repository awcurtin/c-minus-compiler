/********************************************************************/
// Assembly Emitter
//   Utility class to write assembly instructions to a file
//
// CSCI 435: Compilers class
// Fall 2014, 2016

#ifndef ASSEMBLY_EMITTER_H
#define ASSEMBLY_EMITTER_H

/********************************************************************/
// System includes

#include <fstream>
#include <string>

/********************************************************************/
// Using declarations

/********************************************************************/

class AssemblyEmitter
{
public:

  AssemblyEmitter (std::ofstream& outFile);
  ~AssemblyEmitter ();

  // Create a new label
  std::string
  createUniqueLabel ();

  // Generic emission methods
  void
  emitComment (const std::string& comment);

  void
  emitLabel (const std::string& label, const std::string& comment = "");

  void
  emitInstruction (const std::string& operation,
		   const std::string& operands = "",
		   const std::string& comment = "");

  void
  emitFunctionDeclaration (const std::string& name);

  // Emit a function prologue that allocates "localVarSpace" on the stack
  void
  emitPrologue (int localVarSpace);

  // Emit a function epilogue that restores the stack and base pointers,
  //   and returns
  void
  emitEpilogue ();

  // Concrete emission methods
  void
  emitSeparator ();

  void
  emitHeader (const std::string& name);
  
  void
  emitFooter ();
  
  void
  emitInputFunction ();

  void
  emitOutputFunction ();

private:

  std::ofstream& m_outFile;
  int            m_labelCounter;
};
  
#endif
