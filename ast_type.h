/* File: ast_type.h
 * ----------------
 * In our parse tree, Type nodes are used to represent and
 * store type information. The base Type class is used
 * for built-in types, the NamedType for classes and interfaces,
 * and the ArrayType for arrays of other types.  
 *
 * pp3: You will need to extend the Type classes to implement
 * the type system and rules for type equivalency and compatibility.
 */
 
#ifndef _H_ast_type
#define _H_ast_type

#include "ast.h"
#include "list.h"
#include <iostream>


class Type : public Node 
{

  public:
    char *typeName;
    static Type *intType, *doubleType, *boolType, *voidType,
                *nullType, *stringType, *errorType;

    Type(yyltype loc) : Node(loc) {}
    Type(const char *str);
    virtual void Check();
    virtual void PrintToStream(std::ostream& out) { out << typeName; }
    friend std::ostream& operator<<(std::ostream& out, Type *t) { t->PrintToStream(out); return out; }
    virtual bool IsEquivalentTo(Type *other) { return this == other; }
};

class NamedType : public Type 
{
    
  public:
    Identifier *id;
    NamedType(Identifier *i);
    void Check();
    void PrintToStream(std::ostream& out) { out << id; }
    bool IsEquivalentTo(Type *other)
    {
      NamedType* n = dynamic_cast<NamedType*>(other);
      if (n == NULL)
        return false;

      if (strcmp(n->id->name, this->id->name) == 0)
      {
        return true;
      }
      return false;
    }
};

class ArrayType : public Type 
{
  public:
    Type *elemType;
    ArrayType(yyltype loc, Type *elemType);
    void Check();
    void PrintToStream(std::ostream& out) { out << elemType << "[]"; }
    bool IsEquivalentTo(Type *other) 
    {
      ArrayType *o = dynamic_cast<ArrayType*>(other);
      if (o == NULL)
        return false;

      return this->elemType->IsEquivalentTo(o->elemType);
    }
};

 
#endif
