/**************************************************************************
Grant Doty td1131
Purpose: header file of driver.cpp hanlding the production portion of the TIPS
language interpreter. (Moves through code, builds nodes, outputs productions...)
***************************************************************************/

#ifndef PRODUCTIONS_H
#define PRODUCTIONS_H

#include "lexer.h"
#include "parse_tree_nodes.h"
#include <stdlib.h>
#include <iostream>

extern map<string, int> symbolTable;
int nextToken = 0;  // token returned from yylex
int level = 0;      // used to indent output to approximate parse tree

bool semi = false;      // used to check for semicolons and prevent too many yylex() calls
bool recursion = false; // used to check if recursive call to prevent too many yylex() calls


extern "C"
{
	// Instantiate global variables used by flex
	extern char* yytext;       // text of current lexeme
	extern int   yylex();      // the generated lexical analyzer
}

// Forward declarations of production parsing functions
ProgramNode* program();
BlockNode* block();
CompNode* statement();
ExprNode* expression();
SimExprNode* simExpress();
TermNode* term();
FactorNode* factor();
AssignNode* assign();
IfNode* ifParse();
WhileNode* whileParse();
ReadNode* readParse();
WriteNode* writeParse();
// EDIT RETURN TYPES!!!!!!!!!!!!!!1

// Forward declarations of functions that check whether the current token is
// in the first set of a production rule
bool first_of_program();
bool first_of_block();
bool first_of_statement();
bool first_of_expression();



inline void indent(){
    for (int i = 0; i<level; i++)
        cout << ("    ");
}

void output(){
    indent();
    cout << "-->found " << yytext << endl;
}

// ************************************************************************
ProgramNode* program() {

    if (!first_of_program()) // Check for PROGRAM
        throw "3: 'PROGRAM' expected";
    
    indent();
    cout << "enter <program>" << endl;
    ++level;

    // TODO: finish this parsing function...
    ProgramNode* progNode = new ProgramNode;

    output();   // Output "PROGRAM"

    nextToken = yylex();            // Get next token
    if(nextToken != TOK_IDENT)      // Check for identifier 
        throw "2: identifier expected";
    
    output();   //output identifier

    progNode->idNode(string(yytext));   // Pass ident back to node

    nextToken = yylex();                // get next token
    if(nextToken != TOK_SEMICOLON)      // Check for semicolon
        throw "14: ';' expected";

    output();   //Output semicolon

    progNode->bNode = block();    // Call block parser function

    while(nextToken != TOK_EOF)
        nextToken = yylex();    // get eof token

    --level;
    indent();
    cout << "exit <program>" << endl;
    
    return progNode;
}

// ************************************************************************
BlockNode* block(){
    nextToken = yylex();    // get next token
    if(!first_of_block)     // check for VAR or BEGIN
        throw "18: error in declaration part OR 17: 'BEGIN' expected";

    indent();
    cout << "enter <block>" << endl;
    ++level;

    if(nextToken == TOK_VAR){   // Check for variables
        output();   // output VAR

        nextToken = yylex();            // get next token
        if(nextToken != TOK_IDENT)      // Check for identifier 
            throw "2: identifier expected";

        while(nextToken == TOK_IDENT){ // parse for every variable

            if(symbolTable.count(yytext))   // variable is already in symbol table
                throw "101: identifier declared twice"; 
            else
                symbolTable.insert(pair<string, int>(yytext, 0)); // store variable in symobol table
            output();   // output identifier
            
            nextToken = yylex();        // get next token
            if(nextToken != TOK_COLON)  // check for :
                throw "5: ':' expected";

            output();   // output :

            nextToken = yylex();                        // get next token
            if((nextToken != TOK_INTEGER) && (nextToken != TOK_REAL))   // check for type of var
                throw "10: error in type";

            output();   // output type of var

            nextToken = yylex();            // get next token
            if(nextToken != TOK_SEMICOLON)  // check for ;
                throw "14: ';' expected";

            output();   // output ;

            cout << endl;   // put space between variable outputs

            nextToken = yylex(); // get next token
        }

        if(nextToken != TOK_BEGIN)  // Check for BEGIN
            throw "17: 'BEGIN' expected";
    }

    BlockNode* blockNode = new BlockNode;

    if(nextToken == TOK_BEGIN){
        indent();
        cout << "enter <compound_statement>" << endl;
        ++level;

        output();   // output BEGIN
        
        nextToken = yylex();  
        blockNode->firstCNode = statement();   // call statement parser function

        if(nextToken != TOK_SEMICOLON)  // check for semicolon
            throw "14: ';' expected";
        output();

        while(nextToken == TOK_SEMICOLON){
            semi = false;                   // reset semicolon truth value
            nextToken = yylex();
            blockNode->restCNodes.push_back(statement());                    // call statement parser function
            
            if(nextToken == TOK_SEMICOLON)
                output();                       // output ;
            if((nextToken != TOK_SEMICOLON)){    //
                nextToken = yylex();            // get next token
                if(nextToken == TOK_SEMICOLON)
                    output();                       // output ;
            }
        }
        if((nextToken == TOK_EOF) || (nextToken == TOK_END)){   // check for END
            indent();
            cout << "-->found END" << endl;   // output END
        }
        else
            throw "13: 'END' expected";


        --level;
        indent();
        cout << "exit <compound_statement>" << endl;
    }

    --level;
    indent();
    cout << "exit <block>" << endl;

    return blockNode;
}

// ************************************************************************
CompNode* statement(){
    if(!first_of_statement) // check for statement
        throw "900: illegal type of statement";

    indent();
    cout << "enter <statement>" << endl;
    ++level;

    CompNode* newCompNode = new CompNode;

    if(nextToken == TOK_IDENT){ // handles assignments
        newCompNode->firstCompTok = TOK_IDENT;
        newCompNode->aNode = assign();
    }

    else if(nextToken == TOK_IF){    // handles ifs
        newCompNode->firstCompTok = TOK_IF;
        newCompNode->iNode = ifParse();
        // if(newCompNode->iNode->comps == 1)
        //     newCompNode->trueComp = 1;
        // else if(newCompNode->iNode->compt == 1)
        //     newCompNode->trueComp = 1;
    }

    else if(nextToken == TOK_WHILE){ // handles whiles
        newCompNode->firstCompTok = TOK_WHILE;
        newCompNode->whNode = whileParse();
    }

    else if(nextToken == TOK_READ){  // handles reads
        newCompNode->firstCompTok = TOK_READ;
        newCompNode->rNode = readParse();
    }

    else if(nextToken == TOK_WRITE){  // handles writes
        newCompNode->firstCompTok = TOK_WRITE;
        newCompNode->wrNode = writeParse();
    }
    
    --level;
    indent();
    cout << "exit <statement>" << endl;
    
    return newCompNode;
}

// ************************************************************************
AssignNode* assign(){
    indent();
    cout << "enter <assignment>" << endl;
    ++level;

    AssignNode* newAssignNode = new AssignNode;

    if(symbolTable.count(yytext)){   // check symbol table for variable
        output();
        newAssignNode->id = string(yytext);
    }                  
    else
        throw "104: identifier not declared";

    nextToken = yylex();            // get next token
    if(nextToken != TOK_ASSIGN)     // check for opperator
        throw "51: ':=' expected";

    output();   // output :=

    newAssignNode->eNode = expression(); // call expression parser function
    
    --level;
    indent();
    cout << "exit <assignment>" << endl;

    return newAssignNode;
}

// ************************************************************************
IfNode* ifParse(){
    indent();
    cout << "enter <if statement>" << endl;
    ++level;

    IfNode* newIfNode = new IfNode;

    output();   // output IF

    newIfNode->firstExpr = expression(); // call expression parser function
    if((nextToken == TOK_IDENT) || (nextToken == TOK_INTLIT) || (nextToken == TOK_FLOATLIT))
        nextToken = yylex();        // get next token

    if(nextToken != TOK_THEN)   // check for THEN
        throw "52: 'THEN' expected";

    output();   // output THEN

    nextToken = yylex();    // gets next token

    if(nextToken == TOK_BEGIN){     // handles compound_statement parser
        indent();
        cout << "enter <statement>" << endl;
        ++level;
        indent();
        cout << "enter <compound_statement>" << endl;
        ++level;

        output();       // output BEGIN
        newIfNode->comps = 1;

        nextToken = yylex();    // get next token
        newIfNode->secondComp = statement();    // recursive call
        newIfNode->secondComp->trueComp = 1;
        
        if((nextToken != TOK_SEMICOLON) && (nextToken != TOK_END))
            nextToken = yylex();    // get next token
        if(nextToken == TOK_SEMICOLON)
            output();
        while(nextToken == TOK_SEMICOLON){
            nextToken = yylex();
            newIfNode->restSecondComp.push_back(statement());       //recusive
            
            if(nextToken == TOK_SEMICOLON){
                output();
            }
        }

        if(nextToken != TOK_END)
            throw "13: 'END' expected";
        output();   // output END

        --level;
        indent();
        cout << "exit <compound_statement>" << endl;
        --level;
        indent();
        cout << "exit <statement>" << endl;
        nextToken = yylex();    // get next
    }
    else{
        newIfNode->secondComp = statement();    // recursive call
        
        if((nextToken == TOK_END) || (nextToken == TOK_SEMICOLON))
            ;
        else if(nextToken != TOK_ELSE)
            throw "14: ';' expected";
        //output();
    }

    if(nextToken == TOK_ELSE){  // check for else statement
        output();               // output ELSE

        nextToken = yylex();    // gets next token

        if(nextToken == TOK_BEGIN){     // handles compound_statement parser
            indent();
            cout << "enter <statement>" << endl;
            ++level;
            indent();
            cout << "enter <compound_statement>" << endl;
            ++level;

            output();       // output BEGIN
            newIfNode->compt = 1;

            nextToken = yylex();
            newIfNode->thirdComp = statement();    // recursive call
            newIfNode->thirdComp->trueComp = 1;

            if((nextToken != TOK_SEMICOLON) && (nextToken != TOK_END))
                nextToken = yylex();    // get next token
            if(nextToken == TOK_SEMICOLON)
                output();

            while(nextToken == TOK_SEMICOLON){
                nextToken = yylex();    //next token
                newIfNode->restThirdComp.push_back(statement());     //recusive
                
                if(nextToken == TOK_SEMICOLON){
                    output();
                }
            }

            if(nextToken != TOK_END)
                throw "13: 'END' expected";
            output();   // output END

            --level;
            indent();
            cout << "exit <compound_statement>" << endl;
            --level;
            indent();
            cout << "exit <statement>" << endl;
        }
        else{
            newIfNode->thirdComp = statement();    // recursive call
            
            if((nextToken != TOK_SEMICOLON) && (nextToken != TOK_END))
                throw "14: ';' expected";
        }
    }

    --level;
    indent();
    cout << "exit <if statement>" << endl;

    return newIfNode;
}

// ************************************************************************
WhileNode* whileParse(){
    indent();
    cout << "enter <while statement>" << endl;
    ++level;

    output();   // output WHILE

    WhileNode* newWNode = new WhileNode;

    newWNode->firstExpr = expression();   // call expression parser function

    if(nextToken == TOK_BEGIN){
        indent();
        cout << "enter <statement>" << endl;
        ++level;
        indent();
        cout << "enter <compound_statement>" << endl;
        ++level;

        output();       // output BEGIN
        newWNode->whComp = 1;

        nextToken = yylex();
        newWNode->secondComp = statement();    // recursive call
        newWNode->secondComp->trueComp = 1;

        if((nextToken != TOK_SEMICOLON) && (nextToken != TOK_END))
            nextToken = yylex();    // get next token
        if(nextToken == TOK_SEMICOLON)
            output();

        while(nextToken == TOK_SEMICOLON){
            nextToken = yylex();    //next token
            newWNode->restSecondComp.push_back(statement());   //recusive
            
            // if(nextToken == TOK_END)
            //     nextToken = yylex();    //next token
            if(nextToken == TOK_SEMICOLON){
                output();
            }
        }
        if(nextToken != TOK_END)
            throw "13: 'END' expected";
        output();   // output END
        nextToken = yylex();

        --level;
        indent();
        cout << "exit <compound_statement>" << endl;
        --level;
        indent();
        cout << "exit <statement>" << endl;
    }
    else{
        if(nextToken == TOK_CLOSEPAREN)
            nextToken = yylex();
        newWNode->secondComp = statement();    // recursive call 
    }

    --level;
    indent();
    cout << "exit <while statement>" << endl;

    return newWNode;
}

// ************************************************************************
ReadNode* readParse(){
    indent();
    cout << "enter <read>" << endl;
    ++level;

    output();   // output READ

    ReadNode* newRNode = new ReadNode;

    nextToken = yylex();            // get next token
    if(nextToken != TOK_OPENPAREN)  // check for (
        throw "9: '(' expected";

    output();   // output (
    
    nextToken = yylex();        // get next token
    if(nextToken != TOK_IDENT)  // check for identifier
        throw "2: identifier expected";

    if(symbolTable.count(yytext)){   // check symbol table for variable
        output();                   // output identifier
        newRNode->id = string(yytext);
    }
    else
        throw "104: identifier not declared";
    
    nextToken = yylex();            // get next token
    if(nextToken != TOK_CLOSEPAREN) // check for )
        throw "4: ')' expected";

    output();   // output )

    nextToken = yylex();    // get next token

    --level;
    indent();
    cout << "exit <read>" << endl;

    return newRNode;
}

// ************************************************************************
WriteNode* writeParse(){
    indent();
    cout << "enter <write>" << endl;
    ++level;

    output();   // output WRITE

    WriteNode* newWrNode = new WriteNode;

    nextToken = yylex();            // get next token
    if(nextToken != TOK_OPENPAREN)  // check for (
        throw "9: '(' expected";

    output();   // output (
    
    nextToken = yylex();        // get next token
    if((nextToken != TOK_IDENT) && (nextToken != TOK_STRINGLIT))  // check for identifier / string lit
        throw "134: illegal type of operand(s)";
    
    if(nextToken == TOK_IDENT){
        if(symbolTable.count(yytext)){   // check symbol table for variable
            output();                   // output identifier
            newWrNode->id = string(yytext);
        }
        else
            throw "104: identifier not declared";
    }
    else{
        output();   // output string literal
        newWrNode->str = string(yytext);
    }  

    
    nextToken = yylex();            // get next token
    if(nextToken != TOK_CLOSEPAREN) // check for )
        throw "4: ')' expected";

    output();   // output )

    nextToken = yylex();    // get next token

    --level;
    indent();
    cout << "exit <write>" << endl;

    return newWrNode;
}

// ************************************************************************
ExprNode* expression(){
    nextToken = yylex();          // get next token 
    if(!first_of_expression){     // check first token of the expression
        throw "144: illegal type of expression";
    }

    indent();
    cout << "enter <expression>" << endl;
    ++level;

    ExprNode* newENode = new ExprNode;

    newENode->firstSimExpr = simExpress(); // Call simple expression parser function

    if(newENode->firstSimExpr->found != 1){
        bool found = false;
        while(!found){
            if((semi == false) && (nextToken != TOK_MULTIPLY) && (nextToken != TOK_DIVIDE) && (nextToken != TOK_AND)
            && (nextToken != TOK_EQUALTO) && (nextToken != TOK_LESSTHAN) && (nextToken != TOK_GREATERTHAN) && (nextToken != TOK_NOTEQUALTO) 
            && (nextToken != TOK_PLUS) && (nextToken != TOK_MINUS) && (nextToken != TOK_OR) && (nextToken != TOK_THEN)
            && (nextToken != TOK_OPENPAREN) && (nextToken != TOK_CLOSEPAREN) && (nextToken != TOK_NOT) && (nextToken != TOK_BEGIN) && (nextToken != TOK_END))   // check for semicolon or operator
                nextToken = yylex();    // get next token
            else
                found = true;

            if(nextToken == TOK_SEMICOLON)  // sets value of semicolon to true
                semi = true;

            if((nextToken != TOK_EQUALTO) && (nextToken != TOK_LESSTHAN) && (nextToken != TOK_GREATERTHAN) && (nextToken != TOK_NOTEQUALTO))
                found = true;

            if((nextToken == TOK_EQUALTO) || (nextToken == TOK_LESSTHAN) 
            || (nextToken == TOK_GREATERTHAN) || (nextToken == TOK_NOTEQUALTO)){   // check for rest of expression
                output();               // output
                newENode->restSimExprOps.push_back(nextToken);
                nextToken = yylex();    // get next token
                newENode->restSimExprs.push_back(simExpress());           // Call simple expression parser function
            }
        }
    }
    
    --level;
    indent();
    cout << "exit <expression>" << endl;

    return newENode;
}

// ************************************************************************
SimExprNode* simExpress(){
    if(!first_of_expression){     // Checks for first token of simple expression
        throw "901: illegal type of simple expression";
    }

    indent();
    cout << "enter <simple expression>" << endl;
    ++level;

    SimExprNode* newSNode = new SimExprNode;

    newSNode->firstTerm = term();   // calls term parser function
    
    if(newSNode->firstTerm->found != 1){
        bool found = false;
        while(!found){
            if((semi == false) && (nextToken != TOK_MULTIPLY) && (nextToken != TOK_DIVIDE) && (nextToken != TOK_AND)
            && (nextToken != TOK_EQUALTO) && (nextToken != TOK_LESSTHAN) && (nextToken != TOK_GREATERTHAN) && (nextToken != TOK_NOTEQUALTO) 
            && (nextToken != TOK_PLUS) && (nextToken != TOK_MINUS) && (nextToken != TOK_OR) && (nextToken != TOK_THEN)
            && (nextToken != TOK_OPENPAREN) && (nextToken != TOK_CLOSEPAREN) && (nextToken != TOK_NOT) && (nextToken != TOK_BEGIN) && (nextToken != TOK_END))   // check for semicolon or operator
                nextToken = yylex();    // get next token
            else if(nextToken == TOK_IDENT)
                nextToken = yylex();    // get next token
            else 
                found = true;

            if(nextToken == TOK_SEMICOLON)  // sets value of semicolon to true
                semi = true;

            if((nextToken != TOK_PLUS) && (nextToken != TOK_MINUS) && (nextToken != TOK_OR))
                if((nextToken == TOK_IDENT) || (nextToken == TOK_BEGIN) || (nextToken == TOK_IF) || (nextToken == TOK_WHILE) || (nextToken == TOK_READ) || (nextToken == TOK_WRITE))
                    newSNode->found = 1;    // Changed
                found = true;

            if((nextToken == TOK_PLUS) || (nextToken == TOK_MINUS) || (nextToken == TOK_OR)){   // check for rest of simple expression
                output();               // output
                newSNode->restTermOps.push_back(nextToken);
                nextToken = yylex();    // get next token
                newSNode->restTerms.push_back(term());                 // calls term parser function
            }
        }
    }
    
    --level;
    indent();
    cout << "exit <simple expression>" << endl;

    return newSNode;
}

// ************************************************************************
TermNode* term(){
    if(!first_of_expression){   // check for first of term
        throw "902: illegal type of term";
    }

    indent();
    cout << "enter <term>" << endl;
    ++level;

    TermNode* newTNode = new TermNode;

    newTNode->firstFactor = factor(); // calls farctor parser function
    
    bool found = false;
    while(!found){
        if((semi == false) && (nextToken != TOK_MULTIPLY) && (nextToken != TOK_DIVIDE) && (nextToken != TOK_AND)
        && (nextToken != TOK_EQUALTO) && (nextToken != TOK_LESSTHAN) && (nextToken != TOK_GREATERTHAN) && (nextToken != TOK_NOTEQUALTO) 
        && (nextToken != TOK_PLUS) && (nextToken != TOK_MINUS) && (nextToken != TOK_OR) && (nextToken != TOK_THEN) 
        && (nextToken != TOK_OPENPAREN) && (nextToken != TOK_BEGIN))   // check for semicolon or operator
            nextToken = yylex();    // get next token
        else if((nextToken == TOK_IDENT) || (nextToken == TOK_INTLIT) 
        || (nextToken == TOK_FLOATLIT) || (nextToken == TOK_CLOSEPAREN)){        // CHANGED
            nextToken = yylex();    // get next token
        }
        else
            found = true;

        if(nextToken == TOK_SEMICOLON)  // sets value of semicolon to true
            semi = true;
        
        if((nextToken != TOK_MULTIPLY) && (nextToken != TOK_DIVIDE) && (nextToken != TOK_AND)){
            if((nextToken == TOK_IDENT) || (nextToken == TOK_BEGIN) || (nextToken == TOK_IF) || (nextToken == TOK_WHILE) || (nextToken == TOK_READ) || (nextToken == TOK_WRITE))
                newTNode->found = 1;    // Changed
            found = true;
        }

        if(nextToken == TOK_MULTIPLY){     // check for rest of term
            output();               // output
            newTNode->restFactorOps.push_back(nextToken);
            nextToken = yylex();    // get next token
            newTNode->restFactors.push_back(factor());                 // call factor parser function
        }
        if(nextToken == TOK_DIVIDE){     // check for rest of term
            output();               // output
            newTNode->restFactorOps.push_back(nextToken);
            nextToken = yylex();    // get next token
            newTNode->restFactors.push_back(factor());                 // call factor parser function
        }
        if(nextToken == TOK_AND){     // check for rest of term
            output();               // output
            newTNode->restFactorOps.push_back(nextToken);
            nextToken = yylex();    // get next token
            newTNode->restFactors.push_back(factor());                 // call factor parser function
        }
    }
    
    --level;
    indent();
    cout << "exit <term>" << endl;

    return newTNode;
}

// ************************************************************************
FactorNode* factor(){
    if(!first_of_expression){   // check for first of factor
        throw "903: illegal type of factor";
    }

    indent();
    cout << "enter <factor>" << endl;
    ++level;

    FactorNode* newFNode = new FactorNode;

    if(nextToken == TOK_FLOATLIT){
        output();   // output float literal
        newFNode->factorTok = TOK_FLOATLIT;
        newFNode->flLitNode(atof(yytext));
    }
    if(nextToken == TOK_INTLIT){
        output();   // output int literal
        newFNode->factorTok = TOK_INTLIT;
        newFNode->intLitNode(atoi(yytext));
    }
    if(nextToken == TOK_IDENT){
        if(symbolTable.count(yytext)){   // check symbol table for variable
            output();                   // output identifier
            newFNode->factorTok = TOK_IDENT;
            newFNode->id = string(yytext);
        }
        else
            throw "104: identifier not declared";
    }
    if(nextToken == TOK_OPENPAREN){
        output();                           // output (
        newFNode->factorTok = TOK_OPENPAREN;
        newFNode->eNode = expression();     // call expression parser function
        
        if(nextToken != TOK_CLOSEPAREN)     // check for )
            throw "4: ')' expected";
        output();                           // output )
    }
    if((nextToken == TOK_NOT) || (nextToken == TOK_MINUS)){
        output();               // output
        newFNode->factorTok = nextToken;
        nextToken = yylex();    //get next token
        newFNode->fNode = factor();               // recursive call
    }
    
    --level;
    indent();
    cout << "exit <factor>" << endl;

    return newFNode;
}

//*****************************************************************************
bool first_of_program(void) {
    return nextToken == TOK_PROGRAM;
}

bool first_of_block(void){
    return nextToken == (TOK_VAR|TOK_BEGIN);
}

bool first_of_statement(void){
    return nextToken == (TOK_IDENT | TOK_BEGIN | TOK_IF | TOK_WHILE | TOK_READ | TOK_WRITE);
}

bool first_of_expression(void){
    return nextToken == (TOK_INTLIT | TOK_FLOATLIT | TOK_IDENT | TOK_OPENPAREN | TOK_NOT | TOK_MINUS);
}

#endif
