/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
char * savedName = NULL;
int savedLineNo = 0;

%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"return"        {return RETURN;}
"int"           {return INT;}
"void"          {return VOID;}
"="             {return ASSIGN;}
"=="            {return EQ;}
"!="            {return NE;}
"<"             {return LT;}
"<="            {return LE;}
">"             {return GT;}
">="            {return GE;} 
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"/*"            { 
                  char c;
                  while ( (c = input()) != EOF ) {
                      if (c == '*') {
                          if ( (c = input()) == '/' ) {
                              break; 
                          } else {
                              unput(c); 
                          }
                      }
                      if (c == '\n') lineno++; 
                  }
                  if (c == EOF) {
                      fprintf(stderr, "Syntax error: unterminated comment at line %d\n", lineno);
                      return ERROR;
                  }
                }
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACE;}
"]"             {return RBRACE;}
";"             {return SEMI;}
","             {return COMMA;}


{number}        {return NUM;}
{identifier}    {
                  if (savedName!=NULL)
                  {
                    free(savedName);
                    savedName = NULL;
                  }
                  savedName = copyString(yytext);
                  return ID;
                }
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"{"             { 

                  return LCURLY;
                }
"}"             {return RCURLY;}
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  
  strncpy(tokenString,yytext,MAXTOKENLEN);
  fprintf(listing,"%d %s\n",currentToken, tokenString);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

