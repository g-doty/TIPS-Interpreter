/**************************************************************************
Grant Doty td1131
Purpose: header file of driver.cpp defining the class definitions and method
functions for the nodes being built by productions.h file. overloaded << and
destructors linked by a single root node to make a parse tree.
***************************************************************************/

#ifndef PARSE_TREE_NODES_H
#define PARSE_TREE_NODES_H

#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"

using namespace std;

// map
extern map<string,int> symbolTable;

// Forward declarations of classes
class ProgramNode;
class ExprNode;
class FactorNode;
class TermNode;
class SimExprNode;
class CompNode;
class IfNode;
class WhileNode;
class ReadNode;
class WriteNode;
class AssignNode;
class BlockNode;
// Forward declarations of operator<<
ostream& operator<<(ostream& os, ProgramNode& pn);
ostream& operator<<(ostream& os, BlockNode& bn);
ostream& operator<<(ostream& os, CompNode& cn);
ostream& operator<<(ostream& os, AssignNode& an);
ostream& operator<<(ostream& os, WriteNode& wrn);
ostream& operator<<(ostream& os, ReadNode& rn);
ostream& operator<<(ostream& os, WhileNode& wn);
ostream& operator<<(ostream& os, IfNode& in);
ostream& operator<<(ostream& os, ExprNode& en);
ostream& operator<<(ostream& os, SimExprNode& sen);
ostream& operator<<(ostream& os, TermNode& tn);
ostream& operator<<(ostream& os, FactorNode& fn);

//*****************************************************************************
// class FactorNode
class FactorNode {
public:
	int iLit = 0;				// Stores Int Lit
	string id = "";				// Stores Identifier
	float fLit = 0.0;			// Stores Float Lit Val
	int factorTok = 0;			// Checks the tokens
	void intLitNode(int val);	
	void idNode(string name);
	void flLitNode(float val);
	int interpret();
	friend ostream& operator<<(ostream& os, FactorNode& fn);

	FactorNode* fNode = nullptr;
	ExprNode* eNode = nullptr;
    ~FactorNode();
};

ostream& operator<<(ostream& os, FactorNode& fn) {

	int op = fn.factorTok;

	if(op == TOK_NOT){		// Outputs a factor node for !
		os << "factor( NOT " << *(fn.fNode) << ") ";
	}
	if(op == TOK_MINUS){	// Outputs a factor node for -
		os << "factor( - " << *(fn.fNode) << ") ";
	}
	if(op == TOK_OPENPAREN){	// Outputs an expression node 
		os << "nested_expression( " << *(fn.eNode) << " ) ";
	}
	if(op == TOK_INTLIT){	// Outputs int lit
		os << "factor( " << fn.iLit << " ) ";
	}
	if(op == TOK_FLOATLIT){	// Outputs float lit
		os << "factor( " << fn.fLit << " ) ";
	}
	if(op == TOK_IDENT){	// Outputs identity
		os << "factor( " << fn.id << " ) ";
	}
	return os;
}

void FactorNode::intLitNode(int val){
	iLit = val;
}

void FactorNode::flLitNode(float val){
	fLit = val;
}

void FactorNode::idNode(string name){
	id = name;
}

//*****************************************************************************
// class TermNode (Terminal Node)
class TermNode {
public:
	int found = 0;		// holds value to reduce unwanted calls to yylex() in production.h
    FactorNode* firstFactor = nullptr;
    vector<int> restFactorOps;
    vector<FactorNode*> restFactors;
	int interpret();
	friend ostream& operator<<(ostream& os, TermNode& tn);

    ~TermNode();
};

int TermNode::interpret(){		// interpreter for term node
	// get first val
	int retVal = firstFactor->interpret();
	// get next vals and perform calc on each pair
	int lenght = restFactorOps.size();
	for(int i = 0; i < lenght; ++i){
		// get next value
		int nextVal = restFactors[i]->interpret();
		// check for op
		int op = restFactorOps[i];
		if(op == TOK_MULTIPLY){
			retVal = retVal * nextVal;
		}
		else if(op == TOK_DIVIDE)
			retVal = retVal / nextVal;
		else
			if((retVal > 0) && (nextVal > 0))
				retVal = 1;
			else
				retVal = 0;
	}				
	return retVal;
}

ostream& operator<<(ostream& os, TermNode& tn) {
	os << "term( ";
	os << *(tn.firstFactor);

	int length = tn.restFactorOps.size();
	for (int i = 0; i < length; ++i) {
		int op = tn.restFactorOps[i];
		if (op == TOK_MULTIPLY)
			os << "* ";
		else if(op == TOK_DIVIDE)
			os << "/ ";
		else
			os << "AND ";
		os << *(tn.restFactors[i]);
	}
	os << ") ";
	return os;
}

//*****************************************************************************
// class SimExprNode (Expression Node)
class SimExprNode {
public:
	int found = 0;		// holds value to reduce unwanted calls to yylex() in production.h
    TermNode* firstTerm = nullptr;
    vector<int> restTermOps;
    vector<TermNode*> restTerms;
	int interpret();
	friend ostream& operator<<(ostream& os, SimExprNode& sen);

    ~SimExprNode();
};

int SimExprNode::interpret(){		// interpreter for simple expression node
	// get first val
	int retVal = firstTerm->interpret();

	// get next vals and calc for each pair
	int length = restTerms.size();
	for(int i = 0; i < length; ++i){
		// get next val
		int nextVal = restTerms[i]->interpret();
		// check for op
		int op = restTermOps[i];
		if(op == TOK_PLUS){
			retVal = retVal + nextVal;
		}
		else if(op = TOK_MINUS)
			retVal = retVal - nextVal;
		else
			if((retVal > 0) || (nextVal > 0))
				retVal = 1;
			else
				retVal = 0;
	}				
	return retVal;
}

ostream& operator<<(ostream& os, SimExprNode& sen) {
	os << "simple_expression( ";
	os << *(sen.firstTerm);

	int length = sen.restTermOps.size();
	for (int i = 0; i < length; ++i) {

		int op = sen.restTermOps[i];
		if (op == TOK_PLUS)
			os << "+ ";
		else if(op == TOK_MINUS)
			os << "- ";
		else
			os << "OR ";
		os << *(sen.restTerms[i]);
	}
	os << ") ";
	return os;
}

//*****************************************************************************
// class ExprNode
class ExprNode{
	public:
		SimExprNode* firstSimExpr = nullptr;
		vector<int> restSimExprOps;
		vector<SimExprNode*> restSimExprs;
		int interpret();
		friend ostream& operator<<(ostream& os, ExprNode& en);

		~ExprNode();
};

int ExprNode::interpret(){		// interpreter for expression node
	// Get first value
	int retVal = firstSimExpr->interpret();

	// Get next vals and calculate for each pair
	int lenght = restSimExprs.size();
	for(int i = 0; i < lenght; ++i){
		int nextVal = restSimExprs[i]->interpret();
		//check for op
		if(restSimExprOps[i] == TOK_LESSTHAN){
			if(retVal < nextVal)
				retVal = 1;
			else
				retVal = 0;
		}
		else if (restSimExprOps[i] == TOK_GREATERTHAN){
			if(retVal > nextVal)
				retVal = 1;
			else	
				retVal = 0;
		}
		else if(restSimExprOps[i] == TOK_EQUALTO){
			if(retVal == nextVal){
				retVal = 1;
			}
			else
				retVal = 0;
		}
		else{
			if(retVal != nextVal){
				retVal = 1;
			}
			else
				retVal = 0;
		}
	}				
	return retVal;
}

ostream& operator<<(ostream& os, ExprNode& en){
	os << "expression( ";
	os << *(en.firstSimExpr);

	int length = en.restSimExprOps.size();
	for(int i = 0; i < length; ++i){
		int op = en.restSimExprOps[i];
		if(op == TOK_LESSTHAN)
			os << "< ";
		else if(op == TOK_GREATERTHAN)
			os << "> ";
		else if(op == TOK_EQUALTO)
			os << "= ";
		else
			os << "<> ";
		os << *(en.restSimExprs[i]);
	}
	os << ")";
	return os;
}

// **********************************************
int FactorNode::interpret(){	// interpreter for factor node
	// get op
	int op = factorTok;

	int retVal = 0;
	// do calc and return
	if(op == TOK_NOT){
		retVal = fNode->interpret();
		retVal = !retVal;
	}
	if(op == TOK_MINUS){
		retVal = fNode->interpret();
		if(retVal < 0){
			retVal = abs(retVal);
		}
		else
			retVal = -abs(retVal);
	}
	if(op == TOK_OPENPAREN)
		retVal = eNode->interpret();
	if(op == TOK_INTLIT)
		retVal = iLit;		
	if(op == TOK_FLOATLIT)
		retVal = fLit;
	if(op == TOK_IDENT){	// get ident val
		retVal = symbolTable.at(id);
	}			
	return retVal;
}

//*****************************************************************************
// class IfNode
class IfNode{
	public:
		int comps = 0;	//Handles compund statement outputs
		int compt = 0;
		ExprNode* firstExpr = nullptr;	
		CompNode* secondComp = nullptr;
		vector<CompNode*> restSecondComp;
		CompNode* thirdComp = nullptr;
		vector<CompNode*> restThirdComp;
		int interpret();
		friend ostream& operator<<(ostream& os, IfNode& in);

		~IfNode();
};

ostream& operator<<(ostream& os, IfNode& in){
	os << "If ";
	os << *(in.firstExpr);
	os << endl << "%%%%%%%% True Statement %%%%%%%%" << endl;
	if(in.comps == 1)
		os << "Begin Compound Statement" << endl;

	os << *(in.secondComp);

	int length = in.restSecondComp.size();
	for(int i = 0; i < length; ++i)
		os << *(in.restSecondComp[i]);

	if(in.comps == 1)
		os <<  "End Compound Statement" << endl;
	os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

	if(in.thirdComp != NULL){
		os << "%%%%%%%% False Statement %%%%%%%%" << endl;
		if(in.compt == 1)
			os << "Begin Compound Statement" << endl;

		os << *(in.thirdComp);

		int tlength = in.restThirdComp.size();
		for(int i = 0; i < tlength; ++i)
			os << *(in.restThirdComp[i]);

		if(in.compt == 1)
			os << "End Compound Statement" << endl;
		os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	
	return os;
}

//*****************************************************************************
// class WhileNode
class WhileNode{
	public:
		int whComp = 0;		// Handles compound statement output
		ExprNode* firstExpr = nullptr;
		CompNode* secondComp = nullptr;
		vector<CompNode*> restSecondComp;
		int interpret();
		friend ostream& operator<<(ostream& os, WhileNode& wn);

		~WhileNode();
};

ostream& operator<<(ostream& os, WhileNode& wn){
	os << "While ";
	os << *(wn.firstExpr);
	os << endl << "%%%%%%%% Loop Body %%%%%%%%" << endl;
	if(wn.whComp == 1)
		os << "Begin Compound Statement" << endl;
	os << *(wn.secondComp);

	int length = wn.restSecondComp.size();
	for(int i = 0; i < length; ++i)
		os << *(wn.restSecondComp[i]);

	if(wn.whComp == 1){
		os <<  "End Compound Statement" << endl;
	}
	os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

	return os;
}

//*****************************************************************************
// class ReadNode
class ReadNode{
	public:
		string id = "";		// Holds read Identifier
		void idNode(string name);
		int interpret();
		friend ostream& operator<<(ostream& os, ReadNode& rn);

		~ReadNode();
};

ostream& operator<<(ostream& os, ReadNode& rn){
	os << "Read Value ";
	os << rn.id << endl;

	return os;
}

void ReadNode::idNode(string name){
	id = name;
}

//*****************************************************************************
// class WriteNode
class WriteNode{
	public:
		string id = "";		// Holds Identifier
		string str = "";	// Holds string
		void idNode(string name);
		void strNode(string strName);
		int interpret();
		friend ostream& operator<<(ostream& os, WriteNode& wrn);

		~WriteNode();
};

ostream& operator<<(ostream& os, WriteNode& wrn){
	os << "Write ";

	if(wrn.id == "")
		os << "String " << wrn.str << endl;
	else
		os << "Value " << wrn.id << endl;

	return os;
}

void WriteNode::idNode(string name){
	id = name;
}

void WriteNode::strNode(string strName){
	str = strName;
}

//*****************************************************************************
// class AssignNode
class AssignNode{
	public:
		string id = "";		// Holds identifier
		void idNode(string name);
		ExprNode* eNode = nullptr;
		int interpret();
		friend ostream& operator<<(ostream& os, AssignNode& an);

		~AssignNode();
};

ostream& operator<<(ostream& os, AssignNode& an){
	os << "Assignment " << an.id << " := " << *(an.eNode) << endl;

	return os;
}

void AssignNode::idNode(string name){
	id = name;
}

//*****************************************************************************
// class CompNode (handles statement nodes as well)
class CompNode{
	public:
		int firstCompTok = 0; 	// find token of statement
		int trueComp = 0;		// set to 1 if a true compound node, used for destructor
		AssignNode* aNode = nullptr;
		IfNode* iNode = nullptr;
		WhileNode* whNode = nullptr;
		ReadNode* rNode = nullptr;
		WriteNode* wrNode = nullptr;
		CompNode* cNode = nullptr;
		int interperet();
		friend ostream& operator<<(ostream& os, CompNode& cn);

		~CompNode();
};

int CompNode::interperet(){		// interpreter for comp node
	if(aNode != NULL){
		return aNode->interpret();
	}
	else if(iNode != NULL){
		return iNode->interpret();
	}
	else if(whNode != NULL){
		return whNode->interpret();
	}
	else if(rNode != NULL){
		return rNode->interpret();
	}
	else if(wrNode != NULL){
		return wrNode->interpret();
	}
	else{
		return cNode->interperet();
	}
}

ostream& operator<<(ostream& os, CompNode& cn){
	int op = cn.firstCompTok;

	if(op == TOK_IDENT)
		os << *(cn.aNode);
	else if(op == TOK_IF)
		os << *(cn.iNode);
	else if(op == TOK_WHILE)
		os << *(cn.whNode);
	else if(op == TOK_READ)
		os << *(cn.rNode);
	else if(op == TOK_WRITE)
		os << *(cn.wrNode);
	else if(cn.cNode != NULL){
		os << *(cn.cNode);
	}

	return os;
}

// **********************************************
int AssignNode::interpret(){	// interpreter for assign node
	symbolTable.at(id) = eNode->interpret();	

	return 1;
}

// **********************************************
int WriteNode::interpret(){		// interpreter for write node
	if(str != ""){
		string a = str;
		//remove quotes
		a.pop_back();
		a.erase(a.begin());
		cout << a << endl;
	}
	else{
		cout << symbolTable.at(id) << endl;
	}
	return 1;
}

// **********************************************
int ReadNode::interpret(){		// interpreter for read node
	int val = 0;
	cin >> val;

	symbolTable.at(id) = val;

	return 1;
}

// **********************************************
int WhileNode::interpret(){		// interpreter for while node
	while(firstExpr->interpret() > 0){
		secondComp->interperet();

		int length = restSecondComp.size();
		for(int i = 0; i < length; ++i){
			restSecondComp[i]->interperet();
		}
	}
	return 1;
}

// **********************************************
int IfNode::interpret(){	// interpreter for if node
	int ifVal = firstExpr->interpret();

	if(ifVal > 0){
		secondComp->interperet();

		int length = restSecondComp.size();
		for(int i = 0; i < length; ++i){
			restSecondComp[i]->interperet();
		}
	}
	else{
		thirdComp->interperet();

		int lenght = restThirdComp.size();
		for(int i = 0; i < lenght; ++i){
			restThirdComp[i]->interperet();
		}
	}
	return 1;
}

//*****************************************************************************
// class BlockNode
class BlockNode{
	public:
		CompNode* firstCNode = nullptr;
		vector<CompNode*> restCNodes;
		int interpret();
		friend ostream& operator<<(ostream& os, BlockNode& bn);

		~BlockNode();
};

int BlockNode::interpret(){			// interpreter for block node
	firstCNode->interperet();

	int length = restCNodes.size();
	for(int i = 0; i < length; ++i){
		restCNodes[i]->interperet();
	}
	return 1;
}

ostream& operator<<(ostream& os, BlockNode& bn){
	os << "Begin Compound Statement" << endl;
	os << *(bn.firstCNode);

	int length = bn.restCNodes.size();
	for(int i = 0; i < length; ++i){
		os << *(bn.restCNodes[i]);
	}

	os << "End Compound Statement" << endl;

	return os;
}

//*****************************************************************************
// class ProgramNode
class ProgramNode{
	public:
		string id = "";		// Holds Identifier (program name)
		void idNode(string name);
		BlockNode* bNode = nullptr;
		int interpret();
		friend ostream& operator<<(ostream& os, ProgramNode& pn);

		~ProgramNode();
};

int ProgramNode::interpret(){	// Interpreter for program node
	return bNode->interpret();
}

ostream& operator<<(ostream& os, ProgramNode& pn){
	os << "Program Name " << pn.id << endl;
	os << *(pn.bNode);

	return os;
}

void ProgramNode::idNode(string name){
	id = name;
}

//*****************************************************************************
// Desturctors

FactorNode::~FactorNode() {				// Delete factor node
	cout << "Deleting a factorNode" << endl;
	
	int op = factorTok;

	if(op == TOK_NOT){	// Delete node based on op
		delete fNode;
		fNode = nullptr;
	}
	if(op == TOK_MINUS){
		delete fNode;
		fNode = nullptr;
	}
	if(op == TOK_OPENPAREN){
		delete eNode;
		eNode = nullptr;
	}
}

TermNode::~TermNode() {			// Delete term node
	cout << "Deleting a termNode" << endl;
	delete firstFactor;	// Delete factor node
	firstFactor = nullptr;

	int length = restFactorOps.size();
	for (int i = 0; i < length; ++i) {
		delete restFactors[i];	// Delete factor nodes
		restFactors[i] = nullptr;
	}
}

SimExprNode::~SimExprNode() {	// Delete simple Expression node
	cout << "Deleting a simpleExpressionNode" <<endl;
	delete firstTerm;	// Delete term node
	firstTerm = nullptr;

	int length = restTermOps.size();
	for (int i = 0; i < length; ++i) {
		delete restTerms[i];	// Delete term nodes
		restTerms[i] = nullptr;
	}
}

ExprNode::~ExprNode(){		// Delete expression Node
	cout << "Deleting an expressionNode" <<endl;
	delete firstSimExpr;	// Delete sim expr node
	firstSimExpr = nullptr;

	int length = restSimExprOps.size();
	for (int i = 0; i < length; ++i) {
		delete restSimExprs[i];	// Delete sim expr nodes
		restSimExprs[i] = nullptr;
	}
}

IfNode::~IfNode(){		// Delete if node
	cout << "Deleting an ifNode" << endl;

	delete firstExpr;	// Delete expression node
	firstExpr = nullptr;
	delete secondComp;	// Delete comp node
	secondComp = nullptr;

	int length = restSecondComp.size();
	for(int i = 0; i < length; ++i){
		delete restSecondComp[i];	// Delete comp nodes
		restSecondComp[i] = nullptr;
	}

	if(thirdComp != NULL){
		delete thirdComp;	// Delete comp node
		thirdComp = nullptr;
	}

	int tlength = restThirdComp.size();
	for(int i = 0; i < tlength; ++i){
		delete restThirdComp[i];	// Delete comp nodes
		restThirdComp[i] = nullptr;
	}
}

WhileNode::~WhileNode(){	// Delete While node
	cout << "Deleting a whileNode" << endl;

	delete firstExpr;	// Delete expr node
	firstExpr = nullptr;
	delete secondComp;	// Delete comp node
	secondComp = nullptr;

	int length = restSecondComp.size();
	for(int i = 0; i < length; ++i){
		delete restSecondComp[i];	// Delete comp node
		restSecondComp[i] = nullptr;
	}
}

ReadNode::~ReadNode(){	// Delete Read node (nothing to delete)
	cout << "Deleting a readNode" << endl;
}

WriteNode::~WriteNode(){	// Delete write node (nothing to delete)
	cout << "Deleting a writeNode" << endl;
}

CompNode::~CompNode(){	// Delete comp node
	if(trueComp == 1)
		cout << "Deleting a compoundNode" << endl;
	int op = firstCompTok;

	if(op == TOK_IDENT){	// Delete node based on op
		delete aNode;
		aNode = nullptr;
	}
	else if(op == TOK_IF){
		delete iNode;
		iNode = nullptr;
	}
	else if(op == TOK_WHILE){
		delete whNode;
		whNode = nullptr;
	}
	else if(op == TOK_READ){
		delete rNode;
		rNode = nullptr;
	}
	else if(op == TOK_WRITE){
		delete wrNode;
		wrNode = nullptr;
	}
	else if(cNode != NULL){
		delete cNode;
		cNode = nullptr;
	}
}

AssignNode::~AssignNode(){	// Delete assign node
	cout << "Deleting an assignmentNode" << endl;

	delete eNode;	// Delete expression node
	eNode = nullptr;
}

BlockNode::~BlockNode(){	// Delete block node
	cout << "Deleting a blockNode" << endl;
	cout << "Deleting a compoundNode" << endl;

	delete firstCNode;	// Delete comp node
	firstCNode = nullptr;

	int length = restCNodes.size();
	for(int i = 0; i < length; ++i){
		delete restCNodes[i];	// Delete comp nodes
		restCNodes[i] = nullptr;
	}
}

ProgramNode::~ProgramNode(){	// Delete program node
	cout << "Deleting a programNode" << endl;

	delete bNode;	// Delete block node
	bNode = nullptr;
}

#endif /* PARSE_TREE_NODES_H */
