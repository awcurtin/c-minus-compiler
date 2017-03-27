/********************************************************************/
// C- AST Header File
// Design by CSCI 435: Compilers class
// Fall 2014
// Fall 2016

#ifndef C_MINUS_AST_H
#define C_MINUS_AST_H

/********************************************************************/
// System Includes

#include <string>
#include <vector>

/********************************************************************/
// Using Declarations

using std::string;
using std::vector;

/********************************************************************/
// Forward Class Declarations

// Visitor
struct IVisitor;

// Abstract Node
struct Node;

// Root Node
struct ProgramNode;

// Declaration Nodes
struct DeclarationNode;
struct FunctionDeclarationNode;
struct ParameterNode;
struct VariableDeclarationNode;
struct ArrayDeclarationNode;

// Statement Nodes
struct StatementNode;
struct CompoundStatementNode;
struct IfStatementNode;
struct WhileStatementNode;
struct ForStatementNode;
struct ReturnStatementNode;
struct ExpressionStatementNode;

// Expression Nodes
struct ExpressionNode;
struct AssignmentExpressionNode;
struct VariableExpressionNode;
struct SubscriptExpressionNode;
struct CallExpressionNode;
struct AdditiveExpressionNode;
struct MultiplicativeExpressionNode;
struct RelationalExpressionNode;
struct UnaryExpressionNode;
struct IntegerLiteralExpressionNode;

/********************************************************************/
//  Enum Classes

enum class AdditiveOperatorType
{
  PLUS, MINUS
};

enum class MultiplicativeOperatorType
{
  TIMES, DIVIDE
};

enum class RelationalOperatorType
{
  LT, LTE, GT, GTE, EQ, NEQ
};

enum class UnaryOperatorType
{
  INCREMENT, DECREMENT
};

enum class ValueType
{
  VOID, INT, ARRAY
};

/********************************************************************/
// Abstract Classes

class IVisitor
{
public:
  virtual void visit (ProgramNode* node) = 0;

  virtual void visit (DeclarationNode* node) = 0;
  virtual void visit (FunctionDeclarationNode* node) = 0;
  virtual void visit (VariableDeclarationNode* node) = 0;
  virtual void visit (ArrayDeclarationNode* node) = 0;
  virtual void visit (ParameterNode* node) = 0;

  virtual void visit (StatementNode* node) = 0;
  virtual void visit (CompoundStatementNode* node) = 0;
  virtual void visit (IfStatementNode* node) = 0;
  virtual void visit (WhileStatementNode* node) = 0;
  //virtual void visit (ForStatementNode* node) = 0;
  virtual void visit (ReturnStatementNode* node) = 0;
  virtual void visit (ExpressionStatementNode* node) = 0;

  virtual void visit (ExpressionNode* node) = 0;
  virtual void visit (AssignmentExpressionNode* node) = 0;
  virtual void visit (VariableExpressionNode* node) = 0;
  virtual void visit (SubscriptExpressionNode* node) = 0;
  virtual void visit (CallExpressionNode* node) = 0;
  virtual void visit (AdditiveExpressionNode* node) = 0;
  virtual void visit (MultiplicativeExpressionNode* node) = 0;
  virtual void visit (RelationalExpressionNode* node) = 0;
  virtual void visit (UnaryExpressionNode* node) = 0;
  virtual void visit (IntegerLiteralExpressionNode* node) = 0;
};

struct Node
{
  virtual ~Node ();

  virtual void
  accept (IVisitor* visitor) = 0;
};

struct ProgramNode : Node
{
  ProgramNode (vector<DeclarationNode*> declarations);

  ~ProgramNode ();

  void
  accept (IVisitor* visitor);

  vector<DeclarationNode*> declarations;
};

/********************************************************************/
// DECLARATION NODES

struct DeclarationNode : Node
{
  DeclarationNode (ValueType t, string id);
  
  ~DeclarationNode ();

  virtual void
  accept (IVisitor* visitor) = 0;

  ValueType valueType;
  string identifier;

  // Set when the symbol table is built
  // Used for code gen
  int nestLevel;

  //added location
  int row;
  int col;

  //hacking for final
  int isParam;
  int framePointerOffset;
  size_t index;
};

struct FunctionDeclarationNode : DeclarationNode
{
  FunctionDeclarationNode (ValueType t, string id,
    vector<ParameterNode*> params, CompoundStatementNode* csNode);

  ~FunctionDeclarationNode ();

  void
  accept (IVisitor* visitor);

  vector<ParameterNode*> parameters;
  CompoundStatementNode* functionBody;
};

struct VariableDeclarationNode : DeclarationNode
{
  VariableDeclarationNode (string id);

  ~VariableDeclarationNode ();

  void
  accept (IVisitor* visitor);

  // Used for code gen
  // Parameters and local variables are stored at addresses
  //   offset from the frame pointer (ebp)
  int framePointerOffset;
};

struct ArrayDeclarationNode : VariableDeclarationNode
{
  ArrayDeclarationNode (string id, size_t size);

  ~ArrayDeclarationNode ();

  void
  accept (IVisitor* visitor);

  size_t size;
};

struct ParameterNode : DeclarationNode
{
  ParameterNode (string id, bool array);
  ~ParameterNode ();

  void
  accept (IVisitor* visitor);

  bool   isArray;
  size_t index;
};

/********************************************************************/
// STATEMENT NODES

struct StatementNode : Node
{
  ~StatementNode ();

  void
  accept (IVisitor* visitor);
};

struct CompoundStatementNode : StatementNode
{
  CompoundStatementNode (vector<VariableDeclarationNode*> decls,
			 vector<StatementNode*> stmts);
  ~CompoundStatementNode ();

  void
  accept (IVisitor* visitor);

  vector<VariableDeclarationNode*> localDeclarations;
  vector<StatementNode*> statements;
};

struct IfStatementNode : StatementNode
{
  IfStatementNode (ExpressionNode* expr,
                   StatementNode* thenStmt,
                   StatementNode* elseStmt = nullptr);

  ~IfStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* conditionalExpression;
  StatementNode* thenStatement;
  StatementNode* elseStatement;
};

struct WhileStatementNode : StatementNode
{
  WhileStatementNode (ExpressionNode* expr, StatementNode* stmt);

  ~WhileStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* conditionalExpression;
  StatementNode* body;
};

struct ForStatementNode : StatementNode
{
  ForStatementNode (ExpressionNode* e1,
                    ExpressionNode* e2,
                    ExpressionNode* e3,
                    StatementNode* s);

  ~ForStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* initializer;
  ExpressionNode* condition;
  ExpressionNode* updater;
  StatementNode*  body;
};

struct ReturnStatementNode : StatementNode
{
  ReturnStatementNode (ExpressionNode* expr = nullptr);

  ~ReturnStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* expression;
};

struct ExpressionStatementNode : StatementNode
{
  ExpressionStatementNode (ExpressionNode* expr = nullptr);
  ~ExpressionStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* expression;
};

/********************************************************************/
// EXPRESSION NODES

struct ExpressionNode : Node
{
  ~ExpressionNode ();

  void
  accept (IVisitor* visitor);

  int row;
  int col;

  DeclarationNode* parentDecl;
};

struct AssignmentExpressionNode : ExpressionNode
{
  AssignmentExpressionNode (VariableExpressionNode* v, ExpressionNode* e);

  ~AssignmentExpressionNode ();

  void
  accept (IVisitor* visitor);

  VariableExpressionNode* variable;
  ExpressionNode* expression;
};

struct VariableExpressionNode : ExpressionNode
{
  VariableExpressionNode (string id);
  ~VariableExpressionNode ();

  void
  accept (IVisitor* visitor);

  DeclarationNode* parentDecl;
  string identifier;
};

struct SubscriptExpressionNode : VariableExpressionNode
{
  SubscriptExpressionNode (string id, ExpressionNode* index);
  ~SubscriptExpressionNode ();

  void
  accept (IVisitor* visitor);

  DeclarationNode* parentDecl;
  ExpressionNode* index;
};

struct CallExpressionNode : ExpressionNode
{
  CallExpressionNode (string id, vector<ExpressionNode*> args);
  ~CallExpressionNode ();

  void
  accept (IVisitor* visitor);

  DeclarationNode* parentDecl;
  string identifier;
  vector<ExpressionNode*> arguments;
};

struct AdditiveExpressionNode : ExpressionNode
{
  AdditiveExpressionNode (AdditiveOperatorType addop,
			  ExpressionNode* l,
			  ExpressionNode* r);
  ~AdditiveExpressionNode ();

  void
  accept (IVisitor* visitor);

  AdditiveOperatorType addOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};

struct MultiplicativeExpressionNode : ExpressionNode
{
  MultiplicativeExpressionNode (MultiplicativeOperatorType multop,
				ExpressionNode* l,
				ExpressionNode* r);

  ~MultiplicativeExpressionNode ();

  void
  accept (IVisitor* visitor);

  MultiplicativeOperatorType multOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};

struct RelationalExpressionNode : ExpressionNode
{
  RelationalExpressionNode (RelationalOperatorType relop,
			    ExpressionNode* l,
			    ExpressionNode* r);
  ~RelationalExpressionNode ();

  void
  accept (IVisitor* visitor);

  RelationalOperatorType relationalOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};


struct UnaryExpressionNode : ExpressionNode
{
  UnaryExpressionNode (UnaryOperatorType unop,
		       VariableExpressionNode* v);
  ~UnaryExpressionNode ();

  void
  accept (IVisitor* visitor);

  UnaryOperatorType unaryOperator;
  VariableExpressionNode* variable;
};

struct IntegerLiteralExpressionNode : ExpressionNode
{
  IntegerLiteralExpressionNode (int v);
  ~IntegerLiteralExpressionNode ();

  void
  accept (IVisitor* visitor);

  int value;
};

/********************************************************************/
//PrintVisitor

class PrintVisitor : public IVisitor
{
public:

  PrintVisitor (string start = "", int ind = 0);
  ~PrintVisitor ();

  string getTree () const;
  
  virtual void visit (ProgramNode* node);
  
private:

  void indent ();

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
  
  string tree;
  int    indentL;
};

#endif
