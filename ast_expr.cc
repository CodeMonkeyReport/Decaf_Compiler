/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "errors.h"
#include <string.h>


Type* Expr::Check()
{
    return NULL;
}


IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
Type* IntConstant::Check()
{
    return Type::intType;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
Type* DoubleConstant::Check()
{
    return Type::doubleType;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
Type* BoolConstant::Check()
{
    return Type::boolType;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}
Type* StringConstant::Check()
{
    return Type::stringType;
}

Type* NullConstant::Check()
{
    return Type::nullType;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}
Type* CompoundExpr::Check()
{
    return NULL; // TODO
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

Type* EqualityExpr::Check()
{
    Type* lhsType = this->left->Check();
    Type* rhsType = this->right->Check();
    
    if (lhsType->IsEquivalentTo(Type::errorType) || rhsType->IsEquivalentTo(Type::errorType))
    {
        return Type::errorType;
    }
    if (!lhsType->IsEquivalentTo(rhsType))
    {
        ReportError::IncompatibleOperands(this->op, lhsType, rhsType);
        return Type::errorType;
    }
    return Type::boolType; // TODO
}

Type* LogicalExpr::Check()
{
    return Type::boolType;
}

Type* ArithmeticExpr::Check()
{
    if (this->left == NULL) // Unary operation
    {
        Type* opType = this->right->Check();
        if (opType->IsEquivalentTo(Type::doubleType))
        {
            return Type::doubleType;
        }
        else if (opType->IsEquivalentTo(Type::intType))
        {
            return Type::intType;
        }
        else
        {
            ReportError::IncompatibleOperand(this->op, opType);
            return Type::intType;
        }

    }
    else // Binary operation
    {
        Type* lhsType = this->left->Check();
        Type* rhsType = this->right->Check();

        if (lhsType->IsEquivalentTo(Type::errorType) || rhsType->IsEquivalentTo(Type::errorType))
        {
            return Type::errorType;
        }
        if (!lhsType->IsEquivalentTo(rhsType))
        {
            ReportError::IncompatibleOperands(this->op, lhsType, rhsType);
            return Type::errorType;
        }
        return lhsType;
    }
}

Type* RelationalExpr::Check()
{
    return Type::boolType; // TODO
}

Type* AssignExpr::Check()
{
    Type* lhsType = this->left->Check();
    Type* rhsType = this->right->Check();

    if (lhsType->IsEquivalentTo(Type::doubleType) && rhsType->IsEquivalentTo(Type::intType))
    {
        return Type::doubleType;
    }

    if (lhsType->IsEquivalentTo(Type::errorType) || rhsType->IsEquivalentTo(Type::errorType))
    {
        return Type::errorType;
    }

    if (!lhsType->IsEquivalentTo(rhsType))
    {
        ReportError::IncompatibleOperands(this->op, lhsType, rhsType);
        return Type::errorType;
    }
    return lhsType;
}

ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
Type* ArrayAccess::Check()
{
    return Type::voidType; // TODO
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}
Type* FieldAccess::Check()
{
    // If the base is not null then there was a '.' access
    if (this->base != NULL)
    {
        Type* baseType = this->base->Check();
        return baseType;
    }
    else
    {
        Decl* source = this->FindDecl(this->field->name);
        VarDecl* varSource = dynamic_cast<VarDecl*>(source);
        if (varSource == NULL)
            return Type::voidType;
        return varSource->type;
    }
}

Type* LValue::Check()
{
    return Type::voidType;
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}
Type* Call::Check()
{
    Decl* foundDeclare;
    FnDecl* foundFunctionDeclare;
    if (this->base != NULL)
    {
        // Problem is we're not getting the class correclty here
        Type* baseType = this->base->Check();
        if (baseType == NULL)
            return NULL;

        Decl* baseDecl = this->FindDecl(baseType->typeName);
        ClassDecl* baseClassDecl = dynamic_cast<ClassDecl*>(baseDecl);
        if (baseClassDecl == NULL)
            return NULL;
        foundDeclare = baseClassDecl->symbolTable->Lookup(this->field->name);
        foundFunctionDeclare = dynamic_cast<FnDecl*>(foundDeclare);

        // If there is a field not a function call
        if (foundDeclare != NULL && foundFunctionDeclare == NULL)
        {
            ReportError::InaccessibleField(this->field, baseType);
            return NULL;
        }
        // There is no member with that name in the class
        else if(foundDeclare == NULL && foundFunctionDeclare == NULL)
        {
            ReportError::FieldNotFoundInBase(this->field, baseType);
            return NULL;
        }
    }
    else
    {
        foundDeclare = this->FindDecl(this->field->name);
        foundFunctionDeclare = dynamic_cast<FnDecl*>(foundDeclare);
        if (foundFunctionDeclare == NULL)
        {
            ReportError::IdentifierNotDeclared(this->field, LookingForFunction);
            return NULL;
        }
    }
    return foundFunctionDeclare->returnType;
}

Type* This::Check()
{
    Node* p;
    ClassDecl* check;
    bool foundClassDeclare = false;

    for (p = this->parent; p != NULL; p = p->parent)
    {
        check = dynamic_cast<ClassDecl*>(p);
        if (check != NULL)
        {
            foundClassDeclare = true;
            break;
        }
    }
    if (foundClassDeclare == false)
    {
        ReportError::ThisOutsideClassScope(this);
    }
    return new NamedType(check->id);
}

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}
Type* NewExpr::Check()
{
    return Type::voidType; // TODO
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}
Type* NewArrayExpr::Check()
{
    return Type::voidType; // TODO
}

Type* ReadLineExpr::Check()
{
    return Type::stringType;
}

Type* ReadIntegerExpr::Check()
{
    return Type::intType;
}