/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    this->symbolTable = new Hashtable<Decl*>;
    (decls=d)->SetParentAll(this);
}

Type* Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */
    for (int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Declare(this->symbolTable);
    }

    for (int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Check();
    }
    return Type::voidType;
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    this->symbolTable = new Hashtable<Decl*>;
    this->checked = false;
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
Type* StmtBlock::Check()
{
    if (this->checked)
        return Type::voidType;
    this->checked = true;
    for (int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Declare(this->symbolTable);
    }

    for (int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Check();
    }

    for (int i = 0; i < this->stmts->NumElements(); i++)
    {
        this->stmts->Nth(i)->Check();
    }
    return Type::voidType;
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    this->checked = false;
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}
Type* ConditionalStmt::Check()
{
    return Type::voidType;
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    this->checked = false;
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
Type* ForStmt::Check()
{
    if (this->checked)
        return Type::voidType;
    this->checked = true;
    this->init->Check();
    this->test->Check();
    this->step->Check();
    this->body->Check();
    return Type::voidType;
}

Type* LoopStmt::Check()
{
    return Type::voidType;
}

Type* BreakStmt::Check()
{
    return Type::voidType;
}

Type* WhileStmt::Check()
{
    if (this->checked)
        return Type::voidType;
    this->checked = true;
    this->test->Check();
    this->body->Check();
    return Type::voidType; // statements are void
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    this->checked = false;
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
Type* IfStmt::Check()
{
    this->test->Check();
    this->body->Check();

    if(this->elseBody)
        this->elseBody->Check();
    return Type::voidType;
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    this->checked = false;
    (expr=e)->SetParent(this);
}
Type* ReturnStmt::Check()
{
    if (this->checked)
        return Type::voidType;
    this->checked = true;
    this->expr->Check();
    return Type::voidType;
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    this->checked = false;
    (args=a)->SetParentAll(this);
}
Type* PrintStmt::Check()
{
    if (this->checked)
        return Type::voidType;
    this->checked = true;
    for (int i = 0; i < this->args->NumElements(); i++)
    {
        this->args->Nth(i)->Check();
    }
    return Type::voidType;
}
