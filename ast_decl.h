/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"
#include "hashtable.h"

class Type;
class NamedType;
class Identifier;
class Stmt;

class Decl : public Node
{
  protected:
    bool checked;

  public:
    Identifier *id;
    Decl(Identifier *name);
    virtual void Declare(Hashtable<Decl*> *symbolTable) = 0;
    virtual Type* Check() = 0;
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{    
  public:
    Type *type;
    void Declare(Hashtable<Decl*> *symbolTable);
    Type* Check();
    VarDecl(Identifier *name, Type *type);
};

class ClassDecl : public Decl 
{

  public:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    void Declare(Hashtable<Decl*> *symbolTable);
    Type* Check();
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    void Declare(Hashtable<Decl*> *symbolTable);
    Type* Check();
    InterfaceDecl(Identifier *name, List<Decl*> *members);
};

class FnDecl : public Decl 
{
    
  public:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    static bool Compare(FnDecl *a, FnDecl* b);
    void Declare(Hashtable<Decl*> *symbolTable);
    Type* Check();
    void SetFunctionBody(Stmt *b);
};

#endif
