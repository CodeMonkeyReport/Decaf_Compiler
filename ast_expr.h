/* File: ast_expr.h
 * ----------------
 * The Expr class and its subclasses are used to represent
 * expressions in the parse tree.  For each expression in the
 * language (add, call, New, etc.) there is a corresponding
 * node class for that construct. 
 *
 * pp3: You will need to extend the Expr classes to implement 
 * semantic analysis for rules pertaining to expressions.
 */


#ifndef _H_ast_expr
#define _H_ast_expr

#include "ast.h"
#include "ast_stmt.h"
#include "list.h"

class NamedType; // for new
class Type; // for NewArray


class Expr : public Stmt 
{
  public:
    Expr(yyltype loc) : Stmt(loc) {}
    Expr() : Stmt() {}
    virtual Type* Check();
};

/* This node type is used for those places where an expression is optional.
 * We could use a NULL pointer, but then it adds a lot of checking for
 * NULL. By using a valid, but no-op, node, we save that trouble */
class EmptyExpr : public Expr
{
  public:
};

class IntConstant : public Expr 
{
  protected:
    int value;
  
  public:
    Type* Check();
    IntConstant(yyltype loc, int val);
};

class DoubleConstant : public Expr 
{
  protected:
    double value;
    
  public:
    Type* Check();
    DoubleConstant(yyltype loc, double val);
};

class BoolConstant : public Expr 
{
  protected:
    bool value;
    
  public:
    Type* Check();
    BoolConstant(yyltype loc, bool val);
};

class StringConstant : public Expr 
{ 
  protected:
    char *value;
    
  public:
    Type* Check();
    StringConstant(yyltype loc, const char *val);
};

class NullConstant: public Expr 
{
  public: 
    Type* Check();
    NullConstant(yyltype loc) : Expr(loc) {}
};

class Operator : public Node 
{  
  public:
    char tokenString[4];
    Operator(yyltype loc, const char *tok);
    friend std::ostream& operator<<(std::ostream& out, Operator *o) { return out << o->tokenString; }
 };
 
class CompoundExpr : public Expr
{
  protected:
    Operator *op;
    Expr *left, *right; // left will be NULL if unary
    
  public:
    CompoundExpr(Expr *lhs, Operator *op, Expr *rhs); // for binary
    CompoundExpr(Operator *op, Expr *rhs);             // for unary
    Type* Check();
};

class ArithmeticExpr : public CompoundExpr 
{
  public:
    ArithmeticExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    ArithmeticExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    Type* Check();
};

class RelationalExpr : public CompoundExpr 
{
  public:
    Type* Check();
    RelationalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
};

class EqualityExpr : public CompoundExpr 
{
  public:
    Type* Check();
    EqualityExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "EqualityExpr"; }
};

class LogicalExpr : public CompoundExpr 
{
  public:
    Type* Check();
    LogicalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    LogicalExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    const char *GetPrintNameForNode() { return "LogicalExpr"; }
};

class AssignExpr : public CompoundExpr 
{
  public:
    Type* Check();
    AssignExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "AssignExpr"; }
};

class LValue : public Expr 
{
  public:
    Type* Check();
    LValue(yyltype loc) : Expr(loc) {}
};

class This : public Expr 
{
  public:
    This(yyltype loc) : Expr(loc) {}
    Type* Check();
};

class ArrayAccess : public LValue 
{
  protected:
    Expr *base, *subscript;
    
  public:
    Type* Check();
    ArrayAccess(yyltype loc, Expr *base, Expr *subscript);
};

/* Note that field access is used both for qualified names
 * base.field and just field without qualification. We don't
 * know for sure whether there is an implicit "this." in
 * front until later on, so we use one node type for either
 * and sort it out later. */
class FieldAccess : public LValue 
{
  protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    
  public:
    FieldAccess(Expr *base, Identifier *field); //ok to pass NULL base
    Type* Check();
};

/* Like field access, call is used both for qualified base.field()
 * and unqualified field().  We won't figure out until later
 * whether we need implicit "this." so we use one node type for either
 * and sort it out later. */
class Call : public Expr 
{
  protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    List<Expr*> *actuals;
    
  public:
    Call(yyltype loc, Expr *base, Identifier *field, List<Expr*> *args);
    Type* Check();
};

class NewExpr : public Expr
{
  protected:
    NamedType *cType;
    
  public:
    Type* Check();
    NewExpr(yyltype loc, NamedType *clsType);
};

class NewArrayExpr : public Expr
{
  protected:
    Expr *size;
    Type *elemType;
    
  public:
    Type* Check();
    NewArrayExpr(yyltype loc, Expr *sizeExpr, Type *elemType);
};

class ReadIntegerExpr : public Expr
{
  public:
    Type* Check();
    ReadIntegerExpr(yyltype loc) : Expr(loc) {}
};

class ReadLineExpr : public Expr
{
  public:
    Type* Check();
    ReadLineExpr(yyltype loc) : Expr (loc) {}
};

    
#endif
