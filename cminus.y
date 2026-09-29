/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char * message);

%}

%token ENDFILE ERROR 
%token IF WHILE RETURN INT VOID
%token ID NUM 
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%right ELSE

%% /* Grammar for TINY */
program     : declaration_list
                 { savedTree = $1; }
            ;

declaration_list : declaration_list declaration 
                    {
                        YYSTYPE t = $1;
                        if(t != NULL)
                        {
                            TreeNode *p = t;
                            while (p->sibling != NULL)
                                p = p->sibling;
                            p->sibling = $2;
                            $$ = $1;
                        }
                        else 
                        {
                            $$ = $2;
                        }
                    } 
                 | declaration { $$= $1; }                  
            ;

declaration : var_declaration
              {
                $$ = $1;
              }
            | fun_declaration
              {
                $$ = $1;
              }
            ;

var_declaration : type_specifier ID SEMI 
                  {
                    $$ = newDeclNode(VarDeclK);
                    $$->child[0] = $1; 
                    $$->child[1] = newExpNode(IdK); 
                    $$->child[1]->attr.name = copyString(savedName);
                    $$->lineno = lineno;
                  }
                | type_specifier ID LBRACE NUM
                  {
                    /* capture the array size right after NUM is shifted,
                     * before further lookahead can overwrite tokenString */
                    $$ = newExpNode(ConstK);
                    $$->attr.val = atoi(tokenString);
                    $$->lineno = lineno;
                  }
                  RBRACE SEMI
                  {
                    $$ = newDeclNode(VarDeclK);
                    $$->child[0] = $1;
                    $$->child[1] = newExpNode(IdK);
                    $$->child[1]->attr.name = copyString(savedName);
                    $$->attr.val = $5->attr.val;
                    $$->lineno = lineno;
                  }
            ;

type_specifier : INT { $$ = newExpNode(TypeK); $$->attr.op = INT; }
               | VOID { $$ = newExpNode(TypeK); $$->attr.op = VOID; }
            ;

fun_declaration : type_specifier ID
                  {
                    /* capture the function name right after ID is shifted,
                     * before params/body scanning overwrites savedName */
                    $$ = newExpNode(IdK);
                    $$->attr.name = copyString(savedName);
                    $$->lineno = lineno;
                  }
                  LPAREN params RPAREN compound_stmt
                  {
                    $$ = newDeclNode(FunDeclK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->child[2] = $5;
                    $$->child[3] = $7;
                    $$->lineno = lineno;
                  }
            ;

params      : param_list 
                {
                    $$ = $1;
                }
            | VOID { $$ = newExpNode(TypeK); $$->attr.op = VOID; }
            ;

param_list  : param_list COMMA param  
                {
                    YYSTYPE t = $1;
                    if(t != NULL)
                    {
                        TreeNode *p = t;
                        while (p->sibling != NULL)
                            p = p->sibling;
                        p->sibling = $3;
                        $$ = $1;
                    }
                    else 
                    {
                        $$ = $3;
                    }
                }
            | param { $$ = $1 ;}
            ;

param       : type_specifier ID 
                {
                    $$ = newDeclNode(ParamK);
                    $$->child[0] = $1; 
                    $$->child[1] = newExpNode(IdK);
                    $$->child[1]->attr.name = copyString(savedName);
                    $$->lineno = lineno;
                }
            | type_specifier ID LBRACE RBRACE
                {
                    $$ = newDeclNode(ParamK);
                    $$->child[0] = $1; 
                    $$->child[1] = newExpNode(IdK);
                    $$->child[1]->attr.name = copyString(savedName);
                    $$->isArray = TRUE; 
                    $$->lineno = lineno;
                }
            ;

compound_stmt : LCURLY local_declaration statement_list RCURLY
                {
                    $$ = newStmtNode(CompK);
                    $$->child[0] = $2; 
                    $$->child[1] = $3; 
                    $$->lineno = lineno;
                }
            ;

local_declaration : local_declaration  var_declaration 
                      {
                          YYSTYPE t = $1;
                          if(t != NULL)
                          {
                              TreeNode *p = t;
                              while (p->sibling != NULL)
                                  p = p->sibling;
                              p->sibling = $2;
                              $$ = $1;
                          }
                          else 
                          {
                              $$ = $2;
                          }
                      }
                  | /* empty */ { $$ = NULL; }
            ;

statement_list : statement_list statement 
                  {
                      YYSTYPE t = $1;
                      if(t != NULL)
                      {
                          TreeNode *p = t;
                          while (p->sibling != NULL)
                              p = p->sibling;
                          p->sibling = $2;
                          $$ = $1;
                      }
                      else 
                      {
                          $$ = $2;
                      }
                  }
               | /* empty */ { $$ = NULL; }
            ;

statement
    : else_stmt { $$ = $1; }  
    | onlyif_stmt    { $$ = $1; }
    ;

else_stmt
    : expression_stmt { $$ = $1; }
    | compound_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    | IF LPAREN expression RPAREN else_stmt ELSE else_stmt
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
            $$->lineno = lineno;
        }
    | WHILE LPAREN expression RPAREN else_stmt
        {
            $$ = newStmtNode(WhileK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->lineno = lineno;
        }
    ;

onlyif_stmt
    : IF LPAREN expression RPAREN statement
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = NULL;
            $$->lineno = lineno;
        }
    | IF LPAREN expression RPAREN else_stmt ELSE onlyif_stmt
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
            $$->lineno = lineno;
        }
    | WHILE LPAREN expression RPAREN onlyif_stmt
        {
            $$ = newStmtNode(WhileK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->lineno = lineno;
        }
    ;

expression_stmt : expression SEMI
                  {
                    $$ = newStmtNode(ExponentK);
                    $$->child[0] = $1;
                    $$->lineno = lineno;
                  }
                | SEMI { $$ = newStmtNode(ExponentK); $$->child[0] = NULL; $$->lineno = lineno; }
            ;

return_stmt : RETURN SEMI 
              {
                $$ = newStmtNode(ReturnK);
                $$->child[0] = NULL;
                $$->lineno = lineno;
              }
            | RETURN expression SEMI 
                {
                    $$ = newStmtNode(ReturnK);
                    $$->child[0] = $2;
                    $$->lineno = lineno;
                }
            ;

expression  : var ASSIGN expression 
                {
                    $$ = newExpNode(AssignK);
                    $$->child[0] = $1; 
                    $$->child[1] = $3; 
                    $$->lineno = lineno;
                }
            | simple_expression
                {
                    $$ = $1;
                }
            ;

var         : ID 
                {
                    $$ = newExpNode(IdK);
                    $$->attr.name=copyString(savedName);
                    $$->lineno = lineno;
                }
            | ID
                {
                    /* capture the array's base name before the index
                     * expression's own identifiers overwrite savedName */
                    $$ = newExpNode(IdK);
                    $$->attr.name = copyString(savedName);
                    $$->lineno = lineno;
                }
              LBRACE expression RBRACE
                {
                    $$ = newExpNode(ArrayIdK);
                    $$->attr.name = $2->attr.name;
                    $$->child[0] = $4;
                    $$->lineno = lineno;
                }
            ;

simple_expression : additive_expression relop additive_expression 
                      {
                        $$ = newExpNode(OpK);
                        $$->attr.op = $2->attr.op; 
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->lineno = lineno;
                      }
                  | additive_expression
                      {
                        $$ = $1;
                      }
            ;

relop       : LE 
                {
                  $$ = newExpNode(OpK); $$->attr.op = LE; $$->lineno = lineno;
                }
            | LT
                {
                    $$ = newExpNode(OpK); $$->attr.op = LT; $$->lineno = lineno;
                }
            | GT { $$ = newExpNode(OpK); $$->attr.op = GT; $$->lineno = lineno; }
            | GE { $$ = newExpNode(OpK); $$->attr.op = GE; $$->lineno = lineno; }
            | EQ { $$ = newExpNode(OpK); $$->attr.op = EQ; $$->lineno = lineno; }
            | NE { $$ = newExpNode(OpK); $$->attr.op = NE; $$->lineno = lineno; }
            ;

additive_expression : additive_expression addop term 
                        {
                            $$ = newExpNode(OpK);
                            $$->attr.op = $2->attr.op;
                            $$->child[0] = $1;
                            $$->child[1] = $3;
                            $$->lineno = lineno;
                        }
                    | term
                        {
                            $$ = $1;
                        } 
                    ;

addop       : PLUS { $$ = newExpNode(OpK); $$->attr.op = PLUS; $$->lineno = lineno; }
            | MINUS { $$ = newExpNode(OpK); $$->attr.op = MINUS; $$->lineno = lineno; }
            ;

term        : term mulop factor
                {
                    $$ = newExpNode(OpK);
                    $$->attr.op = $2->attr.op; 
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->lineno = lineno;
                }
            | factor { $$ = $1; }
            ;

mulop       : TIMES { $$ = newExpNode(OpK); $$->attr.op = TIMES; $$->lineno = lineno; }
            | OVER { $$ = newExpNode(OpK); $$->attr.op = OVER; $$->lineno = lineno; }
            ;

factor      : LPAREN expression RPAREN
                {
                    $$ = $2;
                }
            | var { $$ = $1; }
            | call { $$ = $1; }
            | NUM {
                $$ = newExpNode(ConstK);
                $$->attr.val = atoi(tokenString);
                $$->lineno = lineno;
            }
            ;

call        : ID
              {
                /* capture the callee name before the argument
                 * expressions' own identifiers overwrite savedName */
                $$ = newExpNode(IdK);
                $$->attr.name = copyString(savedName);
                $$->lineno = lineno;
              }
              LPAREN args RPAREN
              {
                $$ = newExpNode(CallK);
                $$->attr.name = $2->attr.name;
                $$->child[0] = $4;
                $$->lineno = lineno;
              }
            ;
            
args        : arg_list { $$ = $1; }
            | /* empty */ { $$ = NULL; }
            ;

arg_list    : arg_list COMMA expression
                {
                    YYSTYPE t = $1;
                    if(t != NULL)
                    {
                        TreeNode *p = t;
                        while (p->sibling != NULL)
                            p = p->sibling;
                        p->sibling = $3;
                        $$ = $1;
                    }
                    else 
                    {
                        $$ = $3;
                    }
                }
            | expression 
                {
                    $$ = $1;
                }
            ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

