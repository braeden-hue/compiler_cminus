#include "globals.h"
#include "symtab.h"

FILE * listing;
int lineno = 0;

int main(void)
{ Scope global, funcScope;
  BucketList b;

  listing = stdout;

  /* global scope: int x;  int add(int a, int b) { ... } */
  global = sc_create("global", NULL);
  sc_enter(global);
  st_insert(global, "x", Integer, FALSE, 0, VarDeclK, 1, NULL);
  st_insert(global, "add", Integer, FALSE, 0, FunDeclK, 3, NULL);

  /* redeclaration check: "x" again in the same (global) scope */
  b = st_lookup_local(global, "x");
  printf("redeclare check for 'x' in global: %s\n", b ? "FOUND (would be error)" : "not found");

  /* function scope for add(int a, int b) */
  funcScope = sc_create("add", global);
  sc_enter(funcScope);
  st_insert(funcScope, "a", Integer, FALSE, 0, ParamK, 3, NULL);
  st_insert(funcScope, "b", Integer, FALSE, 0, ParamK, 3, NULL);

  /* inside add's body: reference to "a" (local) and "x" (must fall through to global) */
  b = st_lookup(sc_current(), "a");
  printf("lookup 'a' inside add(): %s\n", b ? "found in function scope" : "NOT FOUND (bug)");
  if (b) st_add_lineno(b, 4);

  b = st_lookup(sc_current(), "x");
  printf("lookup 'x' inside add() (should fall through to global): %s\n",
         b ? "found via parent scope" : "NOT FOUND (bug)");
  if (b) st_add_lineno(b, 4);

  b = st_lookup(sc_current(), "nope");
  printf("lookup undeclared 'nope': %s\n", b ? "FOUND (bug)" : "not found (correct)");

  sc_leave(); /* back to global */
  printf("current scope after leaving add(): %s\n", sc_current()->name);

  printSymTab(listing);

  return 0;
}
