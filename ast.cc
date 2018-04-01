/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
    symbolTable = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
}

Decl *Node::FindDecl(const char *name)
{
    Decl *result = NULL;
    Hashtable<Decl*> *currentScope;
    Node *p = this;

    while (p != NULL)
    {
        currentScope = p->symbolTable;
        if (currentScope == NULL)
        {
            p = p->parent;
            continue;
        }
        result = currentScope->Lookup(name);
        if (result != NULL)
            return result;
        
        p = p->parent;
    }
    return NULL;
}

Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

