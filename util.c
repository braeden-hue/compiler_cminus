/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF: fprintf(listing, "reserved word: %s\n", tokenString); break;
    case WHILE: fprintf(listing, "reserved word: %s\n", tokenString); break;
    case ELSE: fprintf(listing, "reserved word: %s\n", tokenString); break;
    case RETURN: fprintf(listing, "reserved word: %s\n", tokenString); break;
    case INT: fprintf(listing, "reserved word: %s\n", tokenString); break;
    case VOID: fprintf(listing, "reserved word: %s\n", tokenString); break;

    case ASSIGN: fprintf(listing,"=\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case NE: fprintf(listing, "!=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case LE: fprintf(listing, "<=\n"); break;
    case GT: fprintf(listing, ">\n"); break;
    case GE: fprintf(listing, ">=\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LBRACE: fprintf(listing, "[\n"); break;
    case RBRACE: fprintf(listing, "]\n"); break;
    case LCURLY: fprintf(listing, "{\n"); break;
    case RCURLY: fprintf(listing, "}\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;

    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function newDeclNode creates a new declaration 
 * node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind kind) // Added for C-Minus
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK; // Declaration node kind
    t->kind.decl = kind;
    t->lineno = lineno;
    t->type = Void; // Default type, will be set during type checking
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = (char*)malloc(n);
  if (t==NULL)
    {fprintf(listing,"Out of memory error at line %d\n",lineno); exit(1);}
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0; // Changed to int, removed static from inner scope for global use

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  if (tree==NULL)
  {
    fprintf(listing,"SDFSDF\n");
  }
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case WhileK: // Added
          fprintf(listing,"While\n");
          break;
        case ReturnK: // Added
          fprintf(listing,"Return\n");
          break;
        case ExponentK: // Expression statement
          fprintf(listing,"Expression Statement\n");
          break;
        case CompK: // Compound statement
          fprintf(listing,"Compound Statement\n");
          break;
        default:
          fprintf(listing,"Unknown StmtNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Id: %s\n",tree->attr.name);
          break;
        case ArrayIdK: 
          fprintf(listing,"Array Id: %s\n",tree->attr.name);
          break;
        case AssignK: 
          fprintf(listing,"Assign\n");
          break;
        case CallK: 
          fprintf(listing,"Call: %s\n",tree->attr.name);
          break;
        case TypeK: 
            fprintf(listing,"Type: %s\n", (tree->attr.op == INT) ? "int" : "void");
            break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DeclK) // Added for declaration nodes
    { switch (tree->kind.decl) {
        case VarDeclK:
          fprintf(listing,"Variable Declaration");
          if (tree->attr.val > 0) fprintf(listing, " [size: %d]", tree->attr.val); // Array size
          fprintf(listing,"\n");
          break;
        case FunDeclK:
          fprintf(listing,"Function Declaration: %s\n", tree->child[1]->attr.name); // Function name
          break;
        case ParamK:
          fprintf(listing,"Parameter Declaration");
          if (tree->isArray) fprintf(listing, "[]");
          fprintf(listing, "\n");
          break;
        default:
          fprintf(listing,"Unknown DeclNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}