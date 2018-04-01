/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "errors.h"
#include <iostream>
using namespace std;
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    this->checked = false;
    (id=n)->SetParent(this); 
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    this->checked = false;
    (type=t)->SetParent(this);
}
void VarDecl::Declare(Hashtable<Decl*> *symbolTable)
{
    this->type->Check();
    Decl* previousDeclare = symbolTable->Lookup(this->id->name);
    if (previousDeclare != NULL)
    {
        ReportError::DeclConflict(this, previousDeclare);
    }
    else
    {
        symbolTable->Enter(this->id->name, this);
    }
}
void VarDecl::Check()
{
    // Do nothing
}


ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    this->checked = false;
    extends = ex;
    this->symbolTable = new Hashtable<Decl*>;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
}
void ClassDecl::Declare(Hashtable<Decl*> *symbolTable)
{
    Decl *previousDeclare = symbolTable->Lookup(this->id->name);
    if (previousDeclare != NULL)
    {
        ReportError::DeclConflict(this, previousDeclare);
    }
    else
    {
        symbolTable->Enter(this->id->name, this);
    }
}
void ClassDecl::Check()
{
    if (this->checked)
        return;
    this->checked = true;

    for (int i = 0; i < this->members->NumElements(); i++)
    {
        this->members->Nth(i)->Declare(this->symbolTable);
    }

    for (int i = 0; i < this->members->NumElements(); i++)
    {
        this->members->Nth(i)->Check();
    }

    // Super class member checks
    if (this->extends != NULL)
    {
        NamedType *classType = this->extends;
        ClassDecl *classDecl = dynamic_cast<ClassDecl*>(this->FindDecl(classType->id->name));

        if (classDecl == NULL)
        {
            ReportError::IdentifierNotDeclared(classType->id, LookingForClass);
        }
        else
        {
            classDecl->Check();
            Decl* val;
            Iterator<Decl*> iter = classDecl->symbolTable->GetIterator();
            while ((val=iter.GetNextValue()) != NULL)
            {
                FnDecl* localImpl = dynamic_cast<FnDecl *>(this->symbolTable->Lookup(val->id->name));
                FnDecl* superImpl = dynamic_cast<FnDecl*>(val);
                if (localImpl == NULL || superImpl == NULL)
                {
                    if (this->symbolTable->Lookup(val->id->name) != NULL)
                    {
                        ReportError::DeclConflict(this->symbolTable->Lookup(val->id->name), val);
                    }
                }
                else if (FnDecl::Compare(superImpl, localImpl) == false)
                {
                    ReportError::OverrideMismatch(localImpl);
                }
                this->symbolTable->Enter(val->id->name, val);
            }
        }
    }

    // Interface member checks
    for (int i = 0; i < this->implements->NumElements(); i++)
    {
        NamedType *interfaceType = this->implements->Nth(i);

        InterfaceDecl *interfaceDecl = dynamic_cast<InterfaceDecl*>(this->FindDecl(interfaceType->id->name));
        if (interfaceDecl == NULL)
        {
            ReportError::IdentifierNotDeclared(interfaceType->id, LookingForInterface);
            continue;
        }
        bool missingTerms = false;
        interfaceDecl->Check();

        Decl *val;
        Iterator<Decl*> iter = interfaceDecl->symbolTable->GetIterator();
        while ((val=iter.GetNextValue()) != NULL)
        {
            FnDecl* classImpl = dynamic_cast<FnDecl *>(this->symbolTable->Lookup(val->id->name));
            FnDecl* intefImpl = dynamic_cast<FnDecl*>(val);
            if (classImpl == NULL || interfaceDecl == NULL)
            {
                missingTerms = true;
                continue;
            }

            if (FnDecl::Compare(classImpl, intefImpl) == false)
            {
                ReportError::OverrideMismatch(classImpl);
            }
        }
        if (missingTerms)
        {
            ReportError::InterfaceNotImplemented(this, this->implements->Nth(i));
        }
    }
}

InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    this->checked = false;
    this->symbolTable = new Hashtable<Decl*>;
    (members=m)->SetParentAll(this);
}
void InterfaceDecl::Declare(Hashtable<Decl*> *symbolTable)
{
    Decl *previousDeclare = symbolTable->Lookup(this->id->name);
    if (previousDeclare != NULL)
    {
        ReportError::DeclConflict(this, previousDeclare);
    }
    else
    {
        symbolTable->Enter(this->id->name, this);
    }
}
void InterfaceDecl::Check()
{
    if (this->checked)
        return;
    this->checked = true;

    for (int i = 0; i < this->members->NumElements(); i++)
    {
        this->members->Nth(i)->Declare(this->symbolTable);
    }

    for (int i = 0; i < this->members->NumElements(); i++)
    {
        this->members->Nth(i)->Check();
    }
}
	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    this->symbolTable = new Hashtable<Decl*>;
    this->checked = false;
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}
void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}
void FnDecl::Declare(Hashtable<Decl*> *symbolTable)
{
    this->returnType->Check();
    Decl* previousDeclare = symbolTable->Lookup(this->id->name);
    if (previousDeclare != NULL)
    {
        ReportError::DeclConflict(this, previousDeclare);
    }
    else
    {
        symbolTable->Enter(this->id->name, this);
        for (int i = 0; i < this->formals->NumElements(); i++)
        {
            this->formals->Nth(i)->Declare(this->symbolTable);
        }
    }
}
void FnDecl::Check()
{
    if (this->checked)
        return;
    this->checked = true;
    if (this->body != NULL)
    {
        this->body->Check();
    }
}
bool FnDecl::Compare(FnDecl* a, FnDecl* b)
{
    if (! a->returnType->IsEquivalentTo(b->returnType) )
    {
        return false;
    }
    if (a->formals->NumElements() != b->formals->NumElements())
    {
        return false;
    }
    for (int i = 0; i < a->formals->NumElements(); i++)
    {
        if (! a->formals->Nth(i)->type->IsEquivalentTo(b->formals->Nth(i)->type) )
        {
            return false;
        }
    }
    return true;
}
