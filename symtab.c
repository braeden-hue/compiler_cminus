/****************************************************/
/* File: symtab.c                                    */
/* Scope-aware symbol table implementation           */
/****************************************************/

#include "symtab.h"

/* the scope currently being populated / searched */
static Scope currentScope = NULL;

/* every scope ever created, in creation order, for printSymTab */
static Scope firstScope = NULL;
static Scope lastScope = NULL;

/* djb-style hash, folded into STSIZE buckets */
static int hash(char * key)
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % STSIZE;
    i++;
  }
  return temp;
}

Scope sc_create(char * name, Scope parent)
{ Scope s = (Scope) malloc(sizeof(struct ScopeRec));
  int i;
  if (s == NULL)
  { fprintf(stderr, "Out of memory creating scope %s\n", name);
    exit(1);
  }
  s->name = name;
  s->parent = parent;
  s->next = NULL;
  for (i = 0; i < STSIZE; i++) s->hashTable[i] = NULL;

  if (firstScope == NULL) firstScope = s;
  else lastScope->next = s;
  lastScope = s;

  return s;
}

Scope sc_current(void)
{ return currentScope; }

void sc_enter(Scope scope)
{ currentScope = scope; }

void sc_leave(void)
{ if (currentScope != NULL) currentScope = currentScope->parent; }

BucketList st_insert(Scope scope, char * name, ExpType type,
                      int isArray, int arraySize,
                      DeclKind declKind, int lineno, TreeNode * treeNode)
{ int h = hash(name);
  BucketList b = (BucketList) malloc(sizeof(struct BucketListRec));
  if (b == NULL)
  { fprintf(stderr, "Out of memory inserting symbol %s\n", name);
    exit(1);
  }
  b->name = name;
  b->type = type;
  b->isArray = isArray;
  b->arraySize = arraySize;
  b->declKind = declKind;
  b->treeNode = treeNode;
  b->lines = (LineList) malloc(sizeof(struct LineListRec));
  b->lines->lineno = lineno;
  b->lines->next = NULL;
  b->next = scope->hashTable[h];
  scope->hashTable[h] = b;
  return b;
}

BucketList st_lookup_local(Scope scope, char * name)
{ int h = hash(name);
  BucketList b = scope->hashTable[h];
  while ((b != NULL) && (strcmp(name, b->name) != 0))
    b = b->next;
  return b;
}

BucketList st_lookup(Scope scope, char * name)
{ while (scope != NULL)
  { BucketList b = st_lookup_local(scope, name);
    if (b != NULL) return b;
    scope = scope->parent;
  }
  return NULL;
}

void st_add_lineno(BucketList entry, int lineno)
{ LineList l = entry->lines;
  while (l->next != NULL) l = l->next;
  l->next = (LineList) malloc(sizeof(struct LineListRec));
  l->next->lineno = lineno;
  l->next->next = NULL;
}

static const char * declKindStr(DeclKind k)
{ switch (k)
  { case VarDeclK: return "var";
    case FunDeclK: return "fun";
    case ParamK:   return "param";
    default:       return "?";
  }
}

static const char * typeStr(ExpType t)
{ switch (t)
  { case Void:    return "void";
    case Integer: return "int";
    case Boolean: return "bool";
    default:      return "?";
  }
}

void printSymTab(FILE * listing)
{ Scope s;
  for (s = firstScope; s != NULL; s = s->next)
  { int i;
    fprintf(listing, "\n<< Scope: %s >>\n", s->name);
    fprintf(listing, "Name              Kind    Type   Array  Lines\n");
    fprintf(listing, "----------------  ------  -----  -----  ------------\n");
    for (i = 0; i < STSIZE; i++)
    { BucketList b = s->hashTable[i];
      while (b != NULL)
      { LineList l = b->lines;
        fprintf(listing, "%-18s%-8s%-7s%-7s", b->name,
                declKindStr(b->declKind), typeStr(b->type),
                b->isArray ? "yes" : "no");
        while (l != NULL)
        { fprintf(listing, "%d ", l->lineno);
          l = l->next;
        }
        fprintf(listing, "\n");
        b = b->next;
      }
    }
  }
}
