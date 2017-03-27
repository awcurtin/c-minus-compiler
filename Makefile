########################################################
# Andrew Curtin : Makefile for CMParser
########################################################

########################################################
# Variable definitions
########################################################
# C++ compiler
#CXX := clang++
CXX := g++

# Include directories, prefaced with "-I"
INCDIRS  := 

# C++ compiler flags
# Use the first for debugging, the second for release
CXXFLAGS := -g -Wall -std=c++14 $(INCDIRS) 
#CXXFLAGS := -O3 -Wall -std=c++14 $(INCDIRS) 

# Linker. For C++ should be $(CXX).
LINK := $(CXX)

# Linker flags. Usually none.
LDFLAGS := 

# Library paths, prefaced with "-L". Usually none.
LDPATHS := 

# Libraries used, prefaced with "-l".
LDLIBS :=
#LDLIBS := -lpthread -lboost_mpi -lboost_serialization

GENERATED := 

#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

CMCompile : Parser.o Lexer.o CMinusAst.o SymbolTable.o SymbolTableVisitor.o AssemblyEmitter.o CodeGenerationVisitor.o CMCompile.cc

CodeGenerationVisitor.o : CodeGenerationVisitor.h CodeGenerationVisitor.cc

AssemblyEmitter.o : AssemblyEmitter.h AssemblyEmitter.cc

SymbolTable.o : SymbolTable.h SymbolTable.cc

SymbolTableVisitor.o : SymbolTableVisitor.h SymbolTableVisitor.cc

Parser.o : Parser.cc Parser.h

Lexer.o : Lexer.cc Lexer.h

CMinusAst.o : CMinusAst.cc CMinusAst.h

#############################################################

.PHONY : clean
clean :
	$(RM) a.out core
	$(RM) *.o *.d *~
	$(RM) $(GENERATED)

#############################################################
