#include "../precompiled.h" //always first //2.53 Updated to 16th Jan, 2017
#include "AST.h"

////////////////////////////////////////////////////////////////

// AST

AST& AST::operator=(AST const& rhs)
{
	loc = rhs.loc;
	return *this;
}

// ASTProgram

ASTProgram::ASTProgram(LocationData const& location) : AST(location) {}

ASTProgram::ASTProgram(ASTProgram const& base) : AST(base.getLocation())
{
	for (vector<ASTImportDecl*>::const_iterator it = base.imports.begin(); it != base.imports.end(); ++it)
		imports.push_back((*it)->clone());
	for (vector<ASTVarDecl*>::const_iterator it = base.variables.begin(); it != base.variables.end(); ++it)
		variables.push_back((*it)->clone());
	for (vector<ASTArrayDecl*>::const_iterator it = base.arrays.begin(); it != base.arrays.end(); ++it)
		arrays.push_back((*it)->clone());
	for (vector<ASTFuncDecl*>::const_iterator it = base.functions.begin(); it != base.functions.end(); ++it)
		functions.push_back((*it)->clone());
	for (vector<ASTTypeDef*>::const_iterator it = base.types.begin(); it != base.types.end(); ++it)
		types.push_back((*it)->clone());
	for (vector<ASTScript*>::const_iterator it = base.scripts.begin(); it != base.scripts.end(); ++it)
		scripts.push_back((*it)->clone());
}

ASTProgram::~ASTProgram()
{
	for (vector<ASTImportDecl*>::const_iterator it = imports.begin(); it != imports.end(); ++it)
		delete *it;
	for (vector<ASTVarDecl*>::const_iterator it = variables.begin(); it != variables.end(); ++it)
		delete *it;
	for (vector<ASTArrayDecl*>::const_iterator it = arrays.begin(); it != arrays.end(); ++it)
		delete *it;
	for (vector<ASTFuncDecl*>::const_iterator it = functions.begin(); it != functions.end(); ++it)
		delete *it;
	for (vector<ASTTypeDef*>::const_iterator it = types.begin(); it != types.end(); ++it)
		delete *it;
	for (vector<ASTScript*>::const_iterator it = scripts.begin(); it != scripts.end(); ++it)
		delete *it;
}

void ASTProgram::addDeclaration(ASTDecl* declaration)
{
	switch (declaration->declarationClassId())
	{
	case ASTDECL_CLASSID_SCRIPT:
		scripts.push_back((ASTScript*)declaration);
		break;
	case ASTDECL_CLASSID_IMPORT:
		imports.push_back((ASTImportDecl*)declaration);
		break;
	case ASTDECL_CLASSID_FUNCTION:
		functions.push_back((ASTFuncDecl*)declaration);
		break;
	case ASTDECL_CLASSID_ARRAY:
		arrays.push_back((ASTArrayDecl*)declaration);
		break;
	case ASTDECL_CLASSID_VARIABLE:
		variables.push_back((ASTVarDecl*)declaration);
		break;
	case ASTDECL_CLASSID_TYPE:
		types.push_back((ASTTypeDef*)declaration);
		break;
	}
}

ASTProgram& ASTProgram::merge(ASTProgram& other)
{
	for (vector<ASTImportDecl*>::const_iterator it = other.imports.begin(); it != other.imports.end(); ++it)
		imports.push_back(*it);
	other.imports.clear();
	for (vector<ASTVarDecl*>::const_iterator it = other.variables.begin(); it != other.variables.end(); ++it)
		variables.push_back(*it);
	other.variables.clear();
	for (vector<ASTArrayDecl*>::const_iterator it = other.arrays.begin(); it != other.arrays.end(); ++it)
		arrays.push_back(*it);
	other.arrays.clear();
	for (vector<ASTFuncDecl*>::const_iterator it = other.functions.begin(); it != other.functions.end(); ++it)
		functions.push_back(*it);
	other.functions.clear();
	for (vector<ASTTypeDef*>::const_iterator it = other.types.begin(); it != other.types.end(); ++it)
		types.push_back(*it);
	other.types.clear();
	for (vector<ASTScript*>::const_iterator it = other.scripts.begin(); it != other.scripts.end(); ++it)
		scripts.push_back(*it);
	other.scripts.clear();
	return *this;
}

// ASTFloat

ASTFloat::ASTFloat(char *Value, int Type, LocationData Loc)
		: AST(Loc), type(Type), negative(false), val((string)Value)
{}
    
ASTFloat::ASTFloat(const char *Value, int Type, LocationData Loc)
		: AST(Loc), type(Type), negative(false), val((string)Value)
{}
    
ASTFloat::ASTFloat(string Value, int Type, LocationData Loc)
		: AST(Loc), type(Type), negative(false), val(Value)
{}

ASTFloat::ASTFloat(long Value, int Type, LocationData Loc)
		: AST(Loc), type(Type), negative(false)
{
	char tmp[15];
	sprintf(tmp, "%ld", Value);
	val = string(tmp);
}

ASTFloat* ASTFloat::clone() const
{
	ASTFloat* c = new ASTFloat(val, type, getLocation());
	c->set_negative(negative);
	return c;
}

pair<string, string> ASTFloat::parseValue()
{
    string f = getValue();
    string intpart;
    string fpart;

    switch(getType())
{
    case TYPE_DECIMAL:
    {
        bool founddot = false;

        for(unsigned int i=0; i<f.size(); i++)
        {
            if(f.at(i) == '.')
            {
                intpart = f.substr(0, i);
                fpart = f.substr(i+1,f.size()-i-1);
                founddot = true;
                break;
            }
}

        if(!founddot)
{
            intpart = f;
            fpart = "";
}

        if(negative) intpart = "-" + intpart;

        break;
    }

    case TYPE_HEX:
{
        // Trim off the "0x".
        f = f.substr(2,f.size()-2);
        // Parse the hex.
        long val2=0;
    
        for(unsigned int i=0; i<f.size(); i++)
    {
            char d = f.at(i);
            val2*=16;

            if('0' <= d && d <= '9')
                val2+=(d-'0');
            else if('A' <= d && d <= 'F')
                val2+=(10+d-'A');
            else
                val2+=(10+d-'a');
    }
    
        if(negative && val2 > 0) val2 *= -1;

        char temp[60];
        sprintf(temp, "%ld", val2);
        intpart = temp;
        fpart = "";
        break;
}

    case TYPE_BINARY:
{
        //trim off the 'b'
        f = f.substr(0,f.size()-1);
        long val2=0;

        for(unsigned int i=0; i<f.size(); i++)
{
            char b = f.at(i);
            val2<<=1;
            val2+=b-'0';
}

        if(negative && val2 > 0) val2 *= -1;

        char temp[60];
        sprintf(temp, "%ld", val2);
        intpart = temp;
        fpart = "";
        break;
    }
}

    return pair<string,string>(intpart, fpart);
}

// ASTString

ASTString::ASTString(const char *strval, LocationData Loc)
		: AST(Loc), str((string)strval)
{}

ASTString::ASTString(string Str, LocationData Loc)
		: AST(Loc), str(Str)
{}

ASTString* ASTString::clone() const
{
	return new ASTString(str, getLocation());
}

////////////////////////////////////////////////////////////////
// Statements

// ASTStmt

ASTStmt& ASTStmt::operator=(ASTStmt const& rhs)
{
	AST::operator=(rhs);
	return *this;
}

// ASTBlock

ASTBlock::~ASTBlock()
{
    list<ASTStmt *>::iterator it;
    
    for (it = statements.begin(); it != statements.end(); it++)
    {
        delete *it;
    }
    
    statements.clear();
}

ASTBlock* ASTBlock::clone() const
{
	ASTBlock* c = new ASTBlock(getLocation());
	for (list<ASTStmt*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
		c->statements.push_back((*it)->clone());
	return c;
}

void ASTBlock::addStatement(ASTStmt *stmt)
{
    statements.push_back(stmt);
}

// ASTStmtIf

ASTStmtIf::~ASTStmtIf()
{
	delete cond;
	delete stmt;
}

ASTStmtIf* ASTStmtIf::clone() const
{
	return new ASTStmtIf(
			cond != NULL ? cond->clone() : NULL,
			stmt != NULL ? stmt->clone() : NULL,
			getLocation());
}

// ASTStmtIfElse
    
ASTStmtIfElse::ASTStmtIfElse(ASTExpr *Cond, ASTStmt *Ifstmt, ASTStmt *Elsestmt, LocationData Loc)
		: ASTStmtIf(Cond, Ifstmt, Loc), elsestmt(Elsestmt)
{}

ASTStmtIfElse::~ASTStmtIfElse()
{
	delete elsestmt;
}

ASTStmtIfElse* ASTStmtIfElse::clone() const
{
	return new ASTStmtIfElse(
			getCondition() != NULL ? getCondition()->clone() : NULL,
			getStmt() != NULL ? getStmt()->clone() : NULL,
			elsestmt != NULL ? elsestmt->clone() : NULL,
			getLocation());
}

// ASTStmtSwitch

ASTStmtSwitch::~ASTStmtSwitch()
{
	delete key;
	for (vector<ASTSwitchCases*>::iterator it = cases.begin(); it != cases.end(); ++it)
		delete *it;
}

ASTStmtSwitch* ASTStmtSwitch::clone() const
{
	ASTStmtSwitch* c = new ASTStmtSwitch(getLocation());
	for (vector<ASTSwitchCases*>::const_iterator it = cases.begin(); it != cases.end(); ++it)
		c->addCases((*it)->clone());
	return c;
}

// ASTSwitchCases

ASTSwitchCases::~ASTSwitchCases()
{
	for (vector<ASTExprConst*>::iterator it = cases.begin(); it != cases.end(); ++it)
		delete *it;
	delete block;
}

ASTSwitchCases* ASTSwitchCases::clone() const
{
	ASTSwitchCases* c = new ASTSwitchCases(getLocation());
	for (vector<ASTExprConst*>::const_iterator it = cases.begin(); it != cases.end(); ++it)
		c->addCase((*it)->clone());
	if (isDefault)
		c->addDefaultCase();
	c->setBlock(block->clone());
	return c;
}

// ASTStmtFor

ASTStmtFor::ASTStmtFor(ASTStmt *Prec, ASTExpr *Term, ASTStmt *Incr, ASTStmt *Stmt, LocationData Loc)
		: ASTStmt(Loc), prec(Prec), term(Term), incr(Incr), stmt(Stmt)
{}

ASTStmtFor::~ASTStmtFor()
{
	delete prec;
	delete term;
	delete incr;
	delete stmt;
}

ASTStmtFor* ASTStmtFor::clone() const
{
	return new ASTStmtFor(
			prec != NULL ? prec->clone() : NULL,
			term != NULL ? term->clone() : NULL,
			incr != NULL ? incr->clone() : NULL,
			stmt != NULL ? stmt->clone() : NULL,
			getLocation());
}

// ASTStmtWhile

ASTStmtWhile::ASTStmtWhile(ASTExpr *Cond, ASTStmt *Stmt, LocationData Loc)
		: ASTStmt(Loc), cond(Cond), stmt(Stmt)
{}

ASTStmtWhile::~ASTStmtWhile()
{
	delete cond;
	delete stmt;
}

ASTStmtWhile* ASTStmtWhile::clone() const
{
	return new ASTStmtWhile(
			cond != NULL ? cond->clone() : NULL,
			stmt != NULL ? stmt->clone() : NULL,
			getLocation());
}

// ASTStmtDo

ASTStmtDo::ASTStmtDo(ASTExpr *Cond, ASTStmt *Stmt, LocationData Loc)
		: ASTStmt(Loc), cond(Cond), stmt(Stmt)
{}

ASTStmtDo::~ASTStmtDo()
{
	delete cond;
	delete stmt;
}

ASTStmtDo* ASTStmtDo::clone() const
{
	return new ASTStmtDo(
			cond != NULL ? cond->clone() : NULL,
			stmt != NULL ? stmt->clone() : NULL,
			getLocation());
}

// ASTStmtReturn

ASTStmtReturn* ASTStmtReturn::clone() const
{
	return new ASTStmtReturn(getLocation());
}

// ASTStmtReturnVal

ASTStmtReturnVal::~ASTStmtReturnVal()
{
	delete retval;
}

ASTStmtReturnVal* ASTStmtReturnVal::clone() const
{
	return new ASTStmtReturnVal(
			retval != NULL ? retval->clone() : NULL,
			getLocation());
}

// ASTStmtBreak

ASTStmtBreak* ASTStmtBreak::clone() const
{
	return new ASTStmtBreak(getLocation());
}

// ASTStmtContinue

ASTStmtContinue* ASTStmtContinue::clone() const
{
	return new ASTStmtContinue(getLocation());
}

// ASTStmtEmpty

ASTStmtEmpty* ASTStmtEmpty::clone() const
{
	return new ASTStmtEmpty(getLocation());
}

////////////////////////////////////////////////////////////////
// Declarations

// ASTDecl

ASTDecl& ASTDecl::operator=(ASTDecl const& rhs)
{
	ASTStmt::operator=(rhs);
	return *this;
}

// ASTScript

ASTScript::ASTScript(LocationData const& location)
	: ASTDecl(location), type(NULL), name("") {}

ASTScript::ASTScript(ASTScript const& base)
	: ASTDecl(base.getLocation()),
	  type(base.type ? base.type->clone() : NULL),
	  name(base.name)
{
	for (vector<ASTVarDecl*>::const_iterator it = base.variables.begin();
		 it != base.variables.end(); ++it)
		variables.push_back((*it)->clone());
	for (vector<ASTArrayDecl*>::const_iterator it = base.arrays.begin();
		 it != base.arrays.end(); ++it)
		arrays.push_back((*it)->clone());
	for (vector<ASTFuncDecl*>::const_iterator it = base.functions.begin();
		 it != base.functions.end(); ++it)
		functions.push_back((*it)->clone());
	for (vector<ASTTypeDef*>::const_iterator it = base.types.begin();
		 it != base.types.end(); ++it)
		types.push_back((*it)->clone());
}

ASTScript::~ASTScript()
{
	delete type;
	for (vector<ASTVarDecl*>::const_iterator it = variables.begin();
		 it != variables.end(); ++it)
		delete *it;
	for (vector<ASTArrayDecl*>::const_iterator it = arrays.begin();
		 it != arrays.end(); ++it)
		delete *it;
	for (vector<ASTFuncDecl*>::const_iterator it = functions.begin();
		 it != functions.end(); ++it)
		delete *it;
	for (vector<ASTTypeDef*>::const_iterator it = types.begin();
		 it != types.end(); ++it)
		delete *it;
}

void ASTScript::addDeclaration(ASTDecl& declaration)
{
	switch (declaration.declarationClassId())
    {
	case ASTDECL_CLASSID_FUNCTION:
		functions.push_back(&(ASTFuncDecl&)declaration);
		break;
	case ASTDECL_CLASSID_ARRAY:
		arrays.push_back(&(ASTArrayDecl&)declaration);
		break;
	case ASTDECL_CLASSID_VARIABLE:
		variables.push_back(&(ASTVarDecl&)declaration);
		break;
	case ASTDECL_CLASSID_TYPE:
		types.push_back(&(ASTTypeDef&)declaration);
		break;
	}
}

// ASTImportDecl

ASTImportDecl* ASTImportDecl::clone() const
{
	return new ASTImportDecl(filename, getLocation());
}

// ASTFuncDecl

ASTFuncDecl::~ASTFuncDecl()
{
    delete rettype;
    delete block;
    list<ASTVarDecl *>::iterator it;

    for(it = params.begin(); it != params.end(); it++)
    {
        delete *it;
    }

    params.clear();
}

ASTFuncDecl* ASTFuncDecl::clone() const
{
	ASTFuncDecl* c = new ASTFuncDecl(getLocation());
	c->setName(name);
	for (list<ASTVarDecl*>::const_iterator it = params.begin(); it != params.end(); ++it)
		c->params.push_back((*it)->clone());
	c->setReturnType(rettype->clone());
	c->setBlock(block->clone());
	return c;
}

void ASTFuncDecl::addParam(ASTVarDecl *param)
{
    params.push_front(param);
}

// ASTDataDecl

ASTDataDecl& ASTDataDecl::operator=(ASTDataDecl const& rhs)
{
	AST::operator=(rhs);
	variable = rhs.variable;
	return *this;
}


// ASTArrayDecl

ASTArrayDecl::ASTArrayDecl(ASTVarType* type, string const& name, ASTExpr* size,
						   ASTArrayList* list, LocationData const& location)
	: ASTDataDecl(location), type(type), name(name), size(size), list(list)
{}

ASTArrayDecl::ASTArrayDecl(ASTArrayDecl const& base)
	: ASTDataDecl(base),
	  type(base.type ? base.type->clone() : NULL),
	  name(base.name),
	  size(base.size ? base.size->clone() : NULL),
	  list(base.list ? base.list->clone() : NULL)
{}

ASTArrayDecl& ASTArrayDecl::operator=(ASTArrayDecl const& rhs)
{
	ASTDataDecl::operator=(rhs);
    delete type;
    delete size;
	delete list;
	type = rhs.type ? rhs.type->clone() : NULL;
	name = rhs.name;
	size = rhs.size ? rhs.size->clone() : NULL;
	list = rhs.list ? rhs.list->clone() : NULL;
	return *this;
}

ASTArrayDecl* ASTArrayDecl::clone() const
{
	return new ASTArrayDecl(*this);
}

ASTArrayDecl::~ASTArrayDecl()
{
    delete type;
    delete list;
    delete size;
}

// ASTArrayList

ASTArrayList::~ASTArrayList()
{
    list<ASTExpr *>::iterator it;

    for(it = exprs.begin(); it != exprs.end(); it++)
    {
        delete *it;
    }

    exprs.clear();
}

ASTArrayList* ASTArrayList::clone() const
{
	ASTArrayList* c = new ASTArrayList(getLocation());
	if (listIsString) c->makeString();
	for (list<ASTExpr*>::const_iterator it = exprs.begin(); it != exprs.end(); ++it)
		c->exprs.push_back((*it)->clone());
	return c;
}

void ASTArrayList::addParam(ASTExpr *expr)
{
    exprs.push_back(expr);
}

void ASTArrayList::addString(string const & str)
{
	LocationData & loc = getLocation();
	for (unsigned int i = 1; i < str.length() - 1; ++i)
		this->addParam(new ASTNumberLiteral(new ASTFloat(long(str[i]), 0, loc), loc));
	this->addParam(new ASTNumberLiteral(new ASTFloat(0L, 0, loc), loc));
}

// ASTVarDecl

ASTVarDecl::ASTVarDecl(ASTVarType* type, string const& name, LocationData const& location)
	: ASTDataDecl(location), type(type), name(name)
{}

ASTVarDecl::ASTVarDecl(ASTVarDecl const& base)
	: ASTDataDecl(base),
	  type(base.type ? base.type->clone() : NULL),
	  name(base.name)
{}

ASTVarDecl& ASTVarDecl::operator=(ASTVarDecl const& rhs)
{
	ASTDataDecl::operator=(rhs);
    delete type;
	type = rhs.type ? rhs.type->clone() : NULL;
	name = rhs.name;
	return *this;
}

ASTVarDecl* ASTVarDecl::clone() const
{
	return new ASTVarDecl(*this);
}

ASTVarDecl::~ASTVarDecl()
{
    delete type;
}

// ASTVarDeclInitializer

ASTVarDeclInitializer::ASTVarDeclInitializer(ASTVarType* type, string const& name,
											 ASTExpr* initial, LocationData const& location)
	: ASTVarDecl(type, name, location), initial(initial)
{}

ASTVarDeclInitializer::ASTVarDeclInitializer(ASTVarDeclInitializer const& base)
	: ASTVarDecl(base),
	  initial(base.initial ? base.initial->clone() : NULL)
{}

ASTVarDeclInitializer& ASTVarDeclInitializer::operator=(ASTVarDeclInitializer const& rhs)
{
	ASTVarDecl::operator=(rhs);
    delete initial;
	initial = rhs.initial ? rhs.initial->clone() : NULL;
	return *this;
}

ASTVarDeclInitializer* ASTVarDeclInitializer::clone() const
{
	return new ASTVarDeclInitializer(*this);
}

ASTVarDeclInitializer::~ASTVarDeclInitializer()
{
    delete initial;
}

// ASTTypeDef

ASTTypeDef::~ASTTypeDef()
{
	delete type;
}

ASTTypeDef* ASTTypeDef::clone() const
{
	return new ASTTypeDef(type->clone(), name, getLocation());
}

////////////////////////////////////////////////////////////////
// Expressions

// ASTExpr

ASTExpr::ASTExpr(ASTExpr const& base)
	: ASTStmt(base), hasValue(base.hasValue), value(base.value), lval(false), varType(base.varType)
{}

ASTExpr& ASTExpr::operator=(ASTExpr const& rhs)
{
	ASTStmt::operator=(rhs);
	lval = rhs.lval;
	hasValue = rhs.hasValue;
	value = rhs.value;
	varType = rhs.varType;
	return *this;
}



// ASTExprConst

ASTExprConst::ASTExprConst(ASTExprConst const& base) : ASTExpr(base)
{
	content = base.content ? base.content->clone() : NULL;
}

ASTExprConst& ASTExprConst::operator=(ASTExprConst const& rhs)
{
	ASTExpr::operator=(rhs);
	content = rhs.content->clone();
	return *this;
}

// ASTExprAssign

ASTExprAssign::~ASTExprAssign()
{
	delete lval;
	delete rval;
}

ASTExprAssign* ASTExprAssign::clone() const
{
	return new ASTExprAssign(
			lval != NULL ? lval->clone() : NULL,
			rval != NULL ? rval->clone() : NULL,
			getLocation());
}

// ASTExprIdentifier

ASTExprIdentifier::ASTExprIdentifier(string const& name, LocationData const& location)
	: ASTExpr(location), isConstant_(false)
{
	components.push_back(name);
}

ASTExprIdentifier::ASTExprIdentifier(ASTExprIdentifier const& base)
	: ASTExpr(base), isConstant_(base.isConstant_), components(base.components)
{}

ASTExprIdentifier& ASTExprIdentifier::operator=(ASTExprIdentifier const& base)
{
	ASTExpr::operator=(base);
	components = base.components;
	isConstant_ = base.isConstant_;
	return *this;
}

string ASTExprIdentifier::asString() const
{
	string s = components.front();
	for (vector<string>::const_iterator it = components.begin() + 1;
	   it != components.end();
	   ++it)
	{
		s = s + "." + *it;
	}

	return s;
}

// ASTExprArrow

ASTExprArrow::ASTExprArrow(ASTExpr* left, string const& right, LocationData const& location)
	: ASTExpr(location), left(left), right(right), index(NULL)
{}

ASTExprArrow::ASTExprArrow(ASTExprArrow const& base)
	: ASTExpr(base), left(base.left->clone()), right(base.right),
	  index(base.index ? base.index->clone() : NULL)
{}

ASTExprArrow& ASTExprArrow::operator=(ASTExprArrow const& rhs)
{
	ASTExpr::operator=(rhs);
	delete left;
	left = rhs.left->clone();
	right = rhs.right;
	delete index;
	index = rhs.index ? rhs.index->clone() : NULL;
	return *this;
}

ASTExprArrow::~ASTExprArrow()
{
	delete left;
	delete index;
}

string ASTExprArrow::asString() const
{
	string s = left->asString() + "->" + right;
	if (index != NULL) s += "[" + index->asString() + "]";
	return s;
}

// ASTExprIndex

ASTExprIndex::ASTExprIndex(ASTExpr* array, ASTExpr* index, LocationData const& location)
	: ASTExpr(location), array(array), index(index)
{}

ASTExprIndex::ASTExprIndex(ASTExprIndex const& base)
	: ASTExpr(base.getLocation()),
	  array(base.array ? base.array->clone() : NULL),
	  index(base.index ? base.index->clone() : NULL)
{}

ASTExprIndex& ASTExprIndex::operator=(ASTExprIndex const& rhs)
{
	ASTExpr::operator=(rhs);
	array = rhs.array ? rhs.array->clone() : NULL;
	index = rhs.index ? rhs.index->clone() : NULL;
	return *this;
}

bool ASTExprIndex::isConstant() const
{
	if (array == NULL || index == NULL) return false;
	return array->isConstant() && index->isConstant();
}


// ASTExprCall

ASTExprCall::ASTExprCall(LocationData const& location)
	: ASTExpr(location), left(NULL)
{}

ASTExprCall::ASTExprCall(ASTExprCall const& base)
	: ASTExpr(base), left(base.left)
{
	for (list<ASTExpr*>::const_iterator it = base.params.begin(); it != base.params.end(); ++it)
		params.push_back((*it)->clone());
}

ASTExprCall& ASTExprCall::operator=(ASTExprCall const& rhs)
{
	ASTExpr::operator=(rhs);
	delete left;
	left = rhs.left;
	for (list<ASTExpr*>::iterator it = params.begin(); it != params.end(); ++it)
		delete *it;
	for (list<ASTExpr*>::const_iterator it = rhs.params.begin(); it != rhs.params.end(); ++it)
		params.push_back((*it)->clone());
	return *this;
}

ASTExprCall::~ASTExprCall()
{
    delete left;
	for (list<ASTExpr*>::iterator it = params.begin(); it != params.end(); ++it)
		delete *it;
}

// ASTUnaryExpr

ASTUnaryExpr::ASTUnaryExpr(ASTUnaryExpr const& base)
	: ASTExpr(base), operand(base.operand ? base.operand->clone() : NULL)
{}

ASTUnaryExpr& ASTUnaryExpr::operator=(ASTUnaryExpr const& rhs)
{
	ASTExpr::operator=(rhs);
	delete operand;
	operand = rhs.operand ? rhs.operand->clone() : NULL;
	return *this;
}

// ASTExprNegate

// ASTExprNot

// ASTExprBitNot

// ASTExprIncrement

// ASTExprPreIncrement

// ASTExprDecrement

// ASTExprPreDecrement

// ASTBinaryExpr

ASTBinaryExpr::ASTBinaryExpr(ASTBinaryExpr const& base)
	: ASTExpr(base),
	  first(base.first ? base.first->clone() : NULL),
	  second(base.second ? base.second->clone() : NULL)
{}

ASTBinaryExpr& ASTBinaryExpr::operator=(ASTBinaryExpr const& rhs)
{
	ASTExpr::operator=(rhs);
	delete first;
	first = rhs.first ? rhs.first->clone() : NULL;
	delete second;
	second = rhs.second ? rhs.second->clone() : NULL;
	return *this;
}

ASTBinaryExpr::~ASTBinaryExpr()
{
	delete first;
	delete second;
}

// ASTLogExpr

// ASTExprAnd

// ASTExprOr

// ASTRelExpr

// ASTExprGT

// ASTExprGE

// ASTExprLT

// ASTExprLE

// ASTExprEQ

// ASTExprNE

// ASTAddExpr

// ASTExprPlus

// ASTExprMinus

// ASTMultExpr

// ASTExprTimes

// ASTExprDivide

// ASTExprModulo
    
// ASTBitExpr

// ASTExprBitAnd

// ASTExprBitOr

// ASTExprBitXor

// ASTShiftExpr

// ASTExprLShift

// ASTExprRShift

////////////////////////////////////////////////////////////////
// Literals

// ASTLiteral

ASTLiteral& ASTLiteral::operator=(ASTLiteral const& rhs)
{
	ASTExpr::operator=(rhs);
	manager = rhs.manager;
	return *this;
}

// ASTNumberLiteral

ASTNumberLiteral::ASTNumberLiteral(ASTNumberLiteral const& base) : ASTLiteral(base)
{
	val = base.val ? base.val->clone() : NULL;
}

ASTNumberLiteral& ASTNumberLiteral::operator=(ASTNumberLiteral const& rhs)
{
	ASTLiteral::operator=(rhs);
	delete val;
	val = rhs.val ? rhs.val->clone() : NULL;
	return *this;
}

// ASTBoolLiteral

ASTBoolLiteral::ASTBoolLiteral(ASTBoolLiteral const& base)
	: ASTLiteral(base), value(base.value)
{}

ASTBoolLiteral& ASTBoolLiteral::operator=(ASTBoolLiteral const& rhs)
{
	ASTLiteral::operator=(rhs);
	value = rhs.value;
	return *this;
}

// ASTStringLiteral

ASTStringLiteral::ASTStringLiteral(char const* str, LocationData const& location)
	: ASTLiteral(location), data(str)
{}

ASTStringLiteral::ASTStringLiteral(string const& str, LocationData const& location)
	: ASTLiteral(location), data(str)
{}

ASTStringLiteral::ASTStringLiteral(ASTString const& raw)
	: ASTLiteral(raw.getLocation()),
	  data(raw.getValue().substr(1, raw.getValue().size() - 2))
{}

ASTStringLiteral::ASTStringLiteral(ASTStringLiteral const& base)
	: ASTLiteral(base), data(base.data)
{}

ASTStringLiteral& ASTStringLiteral::operator=(ASTStringLiteral const& rhs)
{
	ASTLiteral::operator=(rhs);
	data = rhs.data;
	return *this;
}

// ASTArrayLiteral

ASTArrayLiteral::ASTArrayLiteral(LocationData const& location)
	: ASTLiteral(location), type(NULL), size(NULL)
{}

ASTArrayLiteral::ASTArrayLiteral(ASTArrayLiteral const& base)
	: ASTLiteral(base),
	  type(base.type ? base.type->clone() : NULL),
	  size(base.size ? base.size->clone() : NULL)
{
	for (vector<ASTExpr*>::const_iterator it = base.elements.begin();
		 it != base.elements.end(); ++it)
		elements.push_back((*it)->clone());
}

ASTArrayLiteral& ASTArrayLiteral::operator=(ASTArrayLiteral const& rhs)
{
	ASTLiteral::operator=(rhs);

	delete type;
	delete size;
	for (vector<ASTExpr*>::iterator it = elements.begin();
		 it != elements.end(); ++it)
		delete *it;

	type = rhs.type ? rhs.type->clone() : NULL;
	size = rhs.size ? rhs.size->clone() : NULL;
	for (vector<ASTExpr*>::const_iterator it = rhs.elements.begin();
		 it != rhs.elements.end(); ++it)
		elements.push_back((*it)->clone());

	return *this;
}

ASTArrayLiteral::~ASTArrayLiteral()
{
	delete type;
	delete size;
	for (vector<ASTExpr*>::iterator it = elements.begin();
		 it != elements.end(); ++it)
		delete *it;
}


////////////////////////////////////////////////////////////////
// Types

// ASTScriptType

// ASTVarType

ZVarType const& ASTVarType::resolve(Scope& scope)
{
	ZVarType* resolved = type->resolve(scope);
	if (type != resolved)
	{
		delete type;
		type = resolved;
	}
	return *type;
}
