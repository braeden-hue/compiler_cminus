/****************************************************/
/* File: symtab.h                                    */
/* Scope-aware symbol table interface for C-Minus    */
/* Built on a chained hash table per scope, with a   */
/* parent link per scope for enclosing-scope lookup  */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

#define STSIZE 211  /* hash table size (prime); one per scope */
#define SHIFT 4      /* shift used by the hash function */

/* line numbers a symbol is referenced on (declaration line included) */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* one symbol table entry */
typedef struct BucketListRec
   { char * name;
     LineList lines;            /* lines this name is referenced on */
     ExpType type;               /* Void | Integer */
     int isArray;
     int arraySize;              /* valid when isArray; 0 otherwise */
     DeclKind declKind;         /* VarDeclK | FunDeclK | ParamK */
     TreeNode * treeNode;       /* the declaration node (params/return type for FunDeclK) */
     struct BucketListRec * next; /* separate chaining within a scope */
   } * BucketList;

/* one lexical scope: "global", or one function's params+locals */
typedef struct ScopeRec
   { char * name;
     struct ScopeRec * parent;  /* enclosing scope, for st_lookup */
     BucketList hashTable[STSIZE];
     struct ScopeRec * next;    /* links every scope ever created, for printSymTab */
   } * Scope;

/* creates a new scope named `name` whose enclosing scope is `parent`
 * (pass NULL for the global scope). Does not switch the current scope. */
Scope sc_create(char * name, Scope parent);

/* the scope semantic analysis is currently inserting/looking up in */
Scope sc_current(void);

/* switches the current scope to `scope` (e.g. on entering a function body) */
void sc_enter(Scope scope);

/* returns to the current scope's parent (e.g. on leaving a function body) */
void sc_leave(void);

/* registers a new declaration in `scope`. Does not check for redeclaration --
 * call st_lookup_local first and report an error if it already returns non-NULL. */
BucketList st_insert(Scope scope, char * name, ExpType type,
                      int isArray, int arraySize,
                      DeclKind declKind, int lineno, TreeNode * treeNode);

/* looks up `name` starting at `scope` and walking up through parents.
 * Returns NULL if not found in any enclosing scope. */
BucketList st_lookup(Scope scope, char * name);

/* looks up `name` in `scope` only (no parent walk) -- use for redeclaration checks */
BucketList st_lookup_local(Scope scope, char * name);

/* records that `entry` was referenced on `lineno` */
void st_add_lineno(BucketList entry, int lineno);

/* dumps every scope created via sc_create, in creation order */
void printSymTab(FILE * listing);

#endif
