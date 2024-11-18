/*********************************************************************
 Name: Grant Doty NetID: td1131
 Course: CSE 4714 Assignment: Part 1
 Programming Environment: VirtualBox ubuntu 20.04, vs code
 Purpose of File: Contains a subset of the rules for the language TIPS/
                  lexical analysis.
*********************************************************************/
%option yylineno
%option noyywrap
%{
#include "lexer.h"

%}

%%
 /* Keywords */
"BEGIN"             { return TOK_BEGIN; }
"BREAK"             { return TOK_BREAK; }
"CONTINUE"          { return TOK_CONTINUE; }
"DOWNTO"            { return TOK_DOWNTO; }
"ELSE"              { return TOK_ELSE; }
"END"               { return TOK_END; }
"FOR"               { return TOK_FOR; }
"IF"                { return TOK_IF; }
"LET"               { return TOK_LET; }
"PROGRAM"           { return TOK_PROGRAM; }
"READ"              { return TOK_READ; }
"THEN"              { return TOK_THEN; }
"TO"                { return TOK_TO; }
"VAR"               { return TOK_VAR; }
"WHILE"             { return TOK_WHILE; }
"WRITE"             { return TOK_WRITE; }

 /* Datatype */
"INTEGER"           { return TOK_INTEGER; }
"REAL"              { return TOK_REAL; }

 /* Punctuation */
";"                 { return TOK_SEMICOLON; }
":"                 { return TOK_COLON; }
"("                 { return TOK_OPENPAREN; }
")"                 { return TOK_CLOSEPAREN; }
"{"                 { return TOK_OPENBRACE; }
"}"                 { return TOK_CLOSEBRACE; }

 /* Operator */
"+"                 { return TOK_PLUS; }
"-"                 { return TOK_MINUS; }
"*"                 { return TOK_MULTIPLY; }
"/"                 { return TOK_DIVIDE; }
":="                { return TOK_ASSIGN; }
"="                 { return TOK_EQUALTO; }
"<"                 { return TOK_LESSTHAN; }
">"                 { return TOK_GREATERTHAN; }
"<>"                { return TOK_NOTEQUALTO; }
"MOD"               { return TOK_MOD; }
"NOT"               { return TOK_NOT; }
"OR"                { return TOK_OR; }
"AND"               { return TOK_AND; }

 /* Abstractions */
[A-Z][A-Z0-9]*                                  {    
                                                if(strlen(yytext) < 9)
                                                    return TOK_IDENT; 
                                                else 
                                                    return TOK_UNKNOWN;
                                                }
[0-9][0-9]*                                 { return TOK_INTLIT; }
[0-9]+"."[0-9][0-9]*                        { return TOK_FLOATLIT; }
"\'"[ A-Za-z0-9,./?!:@#()$%^&*_+=\"\t\r]*"\'"     {                      
                                                if(strlen(yytext) < 81)
                                                    return TOK_STRINGLIT; 
                                                else
                                                    return TOK_UNKNOWN;
                                                }
 /* ""  { return TOK_EOF; } */

 /* Eat Whitespace */
[ \t\r\n]+      /* aop */

 /* Found an unknown character */

.                   { return TOK_UNKNOWN; }

 /* Recognize end of file */

<<EOF>>             { return TOK_EOF; }


