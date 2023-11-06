#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"
#include"ad.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

// same as err, but also prints the line of the current tokenNoreturn 
void tkerr(const char *fmt,...){
	fprintf(stderr,"error in line %d: ",tokens[iTk].line-1);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}
bool expr();
bool instr();
bool block();
bool consume(int code) {
	if (tokens[iTk].code == code) {
	
		consumed = &tokens[iTk++];
		return true;
	}
	return false;
}
bool baseType()
{
	if (consume(TYPE_INT)) {
		ret.type = TYPE_INT;

		return true;
	}
	if (consume(TYPE_REAL)) {
		ret.type = TYPE_REAL;

		return true;
	}
	if (consume(TYPE_STR)) {
		ret.type = TYPE_STR;
		return true;
	}
	return false;
}

bool funcParam() {
	int start = iTk;
	if (consume(ID)) {
		const char* name = consumed->text;
		Symbol* s = searchInCurrentDomain(name);
		if (s)tkerr("symbol redefinition: %s", name);
		s = addSymbol(name, KIND_ARG);
		Symbol* sFnParam = addFnArg(crtFn, name);
		if (consume(COLON)) {
			if (baseType()) {
				s->type = ret.type;
				sFnParam->type = ret.type;
				return true;
			}
			else tkerr("Tipul parametrului trebuie specificat");
		}
		else tkerr("Lipsesc : dupa numele parametrului ");
	}
	iTk = start;
	return false;
}
bool funcParams() {
	int start = iTk;
	if (funcParam()) {
		while (consume(COMMA)) {
			if (funcParam()) {}
			else tkerr("Expresie invalida dupa virgula trebuie sa urmeze parametrii");
		}
		return true;
	}
	iTk = start;
	return false;
	
}
bool instr() {
	int start = iTk;
	if (expr()) {
		if (consume(SEMICOLON)) {
			return true;
		}
		else tkerr("Lipsesc ; dupa expresie");
	}
	if (consume(SEMICOLON)) { return true; }
	if (consume(IF)) {
		if (consume(LPAR)) {
			if (expr()) {
				if (consume(RPAR)) {
					if (block()) {
						if (consume(ELSE)) {
							if (block()) {}
						}
						if (consume(END)) { return true; }
					}
					else tkerr("Expected 'END' after block.");
				}
				else tkerr("Paranteza ) nu este inchisa!");
			}
			else tkerr("Invalid expression inside the IF statement.");
		}
		else tkerr("Paranteza ( nu este deschisa!");
	}
	if (consume(RETURN)) {
		if (expr()) {
			if (consume(SEMICOLON)) {
				return true;
			}
			else tkerr("Lipsesc ; dela sfarsitul expresiei");
		}
	}
	if (consume(WHILE)) {
		if (consume(LPAR)) {
			if (expr()) {
				if (consume(RPAR)) {
					if (block()) {
						if (consume(END)) {
							return true;
						}
						else tkerr("Expected 'END' after block.");
					}
				}
				else tkerr("Paranteza ) nu este inchisa!");
			}
		}
		else tkerr("Lipseste ( dupa while");
	}
	
		
		
	
	iTk = start;
	return false;
}

bool block() {
	if (instr()) {
		while (instr()) {}
		return true;

	}
	return false;
}
bool factor() {
	int start = iTk;
	if (consume(INT)) { return true; }
	if (consume(REAL)) { return true; }
	if (consume(STR)) { return true; }
	if (consume(ID)) {
		if (consume(LPAR)) {
			if (expr()) {
				while (consume(COMMA)) {
					if (expr()) {}
					else tkerr("Lipseste parametrul dupa , ");
				}
			}
			if (consume(RPAR)) {}
			else tkerr("Paranteza nu este inchisa!");
		}
		return true;

	}
	if (consume(LPAR)) {
		if (expr()) {
			if (consume(RPAR)) {
				return true;
			}
			else tkerr("Lipseste ) dupa declararea expresiei");
		}
	}
	iTk = start;
	return false;
}




bool exprPrefix() {
	int start = iTk;
	
	if (consume(SUB)) {}
	else if (consume(NOT)) {}
	if (factor()) {
		return true;
	}
	
	iTk = start;
	return false;
}
bool exprMul() {
	int start = iTk;
	if (exprPrefix()) {
		for (;;) {
			if (consume(MUL)) {}
			else if (consume(DIV)) {}
			else return true;
			if (consume(exprPrefix())) {
				return true;
			}
		}
	}
	iTk = start;
	return false;
}
bool exprAdd() {
	int start = iTk;
	if (exprMul()) {
		for (;;) {
			if (consume(ADD)) {}
			else if (consume(SUB)) {}
			else return true;
			if (exprMul()) {
				return true;
			}
		}
	}
	iTk = start;
	return false;
}
bool exprComp() {
	int start = iTk;
	if (exprAdd()) {
		if (consume(LESS)) {}
		else if (consume(EQUAL)) {}
		else return true;
		if (exprAdd()) {}
		else tkerr("Expresie invalida!");
		return true;
	}
	iTk = start;
	return false;
}

bool exprAssign() {
	int start = iTk;

		if (consume(ID)) {
			if (consume(ASSIGN)) {
				if (exprComp()) {
					return true;
				}
				else tkerr("Dupa '<' sau '=' trebuie sa urmeze o expresie");
			}
			else
				iTk--;

		}
		if (exprComp()) {
			return true;
		}
		
	iTk = start;
	return false;
}

bool exprLogic() {
	int start = iTk;
	if (exprAssign()) {
		for (;;) {
			if (consume(ADD)) {}
			else if (consume(OR)) {}
			else return true;
		}
		if (exprAssign()) {
			return true;
		}
	}
	iTk = start;
	return false;
}
bool expr() {
	return exprLogic(); 
}


bool defVar()
{
	int start = iTk;
	if (consume(VAR)) {
		if (consume(ID)) {
			const char* name = consumed->text;
			Symbol* s = searchInCurrentDomain(name);
			if (s)tkerr("symbol redefinition: %s", name);
			s = addSymbol(name, KIND_VAR);
			s->local = crtFn != NULL;
			if (consume(COLON)) {
				if (baseType()) {
					s->type = ret.type;
					if (consume(SEMICOLON)) {
						return true;
					}
					else tkerr("Lipseste ; de la sfarsitul decalaratiei variabilei ");
				}
				else tkerr("Tipul variabilei trebuie specificat!");
			}
			else tkerr("Lipseste : dupa numele variabilei");
		}
		else tkerr("Lipseste numele variabilei");
	}
	iTk = start;
	return false;

}

bool defFunc()
{
	int start = iTk;
	if (consume(FUNCTION)) {
		if (consume(ID)) {
			const char* name = consumed->text;
			Symbol* s = searchInCurrentDomain(name);
			if (s)tkerr("symbol redefinition: %s", name);
			crtFn = addSymbol(name, KIND_FN);
			crtFn->args = NULL;
			addDomain();
			if (consume(LPAR)) {
				if (funcParams()) {}
				if (consume(RPAR)) {
					if (consume(COLON)) {
						if (baseType()) {
							crtFn->type = ret.type;
							while (defVar()) {}
							if (block()) {
								if (consume(END)) {
									delDomain();
									crtFn = NULL;

									return true;
								}
								else tkerr("Lipseste END la sfarsitul functiei");
							}
							else tkerr("Nu se poate declara doar prototipul functie");

						}
						else tkerr("Tipul functiei trebuie specificat!");
					}
					else tkerr("Lipsesc : dupa declararea functiei");
				}
				else tkerr("Nu s-a inchis ) ");
			}
			else tkerr("Lipseste ( dupa numele functiei");
		}
		else tkerr("Lipseste numele functiei");
	}
	iTk = start;
	return false;
}

bool program() {
	addDomain();
	for (;;) {
		if (defVar()) {}
		else if (defFunc()) {}
		else if (block()) {}
		else break;
	}
	if (consume(FINISH)) {
		//printf("Program rulat cu succes!");
		delDomain();
		return true;
	}
	else tkerr("syntax error");
	return false;
}

// program ::= ( defVar | defFunc | block )* FINISH

void parse(){
	iTk=0;
	program();
	}
