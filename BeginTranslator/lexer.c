#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"
#define _CRT_SECURE_NO_WARNINGS

Token tokens[MAX_TOKENS];
int nTokens;

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code){
	if(nTokens==MAX_TOKENS)err("too many tokens");
	Token *tk=&tokens[nTokens];
	tk->code=code;
	tk->line=line;
	nTokens++;
	return tk;
	}

// copy in the dst buffer the string between [begin,end)
char *copyn(char *dst,const char *begin,const char *end){
	char *p=dst;
	if(end-begin>MAX_STR)err("string too long");
	while(begin!=end)*p++=*begin++;
	*p='\0';
	return dst;
	}

void tokenize(const char *pch){
	const char *start;
	Token *tk;
	char buf[MAX_STR+1];
	for(;;){
		switch(*pch){
			case ' ':case '\t':pch++;break;
			case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
				if(pch[1]=='\n')pch++;
				// fallthrough to \n
			case '\n':
				line++;
				pch++;
				break;
			case '\0':addTk(FINISH);return;
			case ',':addTk(COMMA);pch++;break;
			case '=':
				if(pch[1]=='='){
					addTk(EQUAL);
					pch+=2;
					}else{
					addTk(ASSIGN);
					pch++;
					}
				break;
			case ':': addTk(COLON); pch++; break;
			case ';': addTk(SEMICOLON); pch++; break;
			case '(': addTk(LPAR); pch++; break;
			case ')': addTk(RPAR); pch++; break;
			case '+': addTk(ADD); pch++; break;
			case '/': addTk(DIV); pch++; break;
			case '&&': addTk(AND); pch+=2; break;
			case '||': addTk(OR); pch+=2; break;
			case '!':
				if (pch[1] == ' = ') {
					addTk(NOT);
					pch += 2;
				}
				else {
					addTk(NOT);
					pch++;
				}
			case '>':
				if (pch[1] == ' = ') {
					addTk(GREATEREQ);
					pch += 2;
				}
				else {
					addTk(GREATER);
					pch++;
				}
			case '<': addTk(LESS); pch++; break;
			case '"':
				for (start = ++pch; *pch != '"'; pch++) {}
				if (*pch == '\0')err("end of file while in string");
				tk = addTk(STR);
				copyn(tk->text, start, pch);
				pch++;
				break;
			case '#':
				while (*pch != '\0' && *pch != '\n') {
					pch++;
				}
				line++;
				pch++;
				break;

			default:
				if (isdigit(*pch) ) {
					for (start = pch; isdigit(*pch) || (*pch == '.' && isdigit(*(pch + 1))); pch++) {}
					char* text = copyn(buf, start, pch);
					
					if (strchr(text, '.') != NULL) {
						tk = addTk(REAL);
						tk->r = atof(text);
					}
					else {
						tk = addTk(INT);
						tk->i = atoi(text);
					}
					
				}

				else if (isalpha(*pch) || *pch == '_') {
					for (start = pch++; isalnum(*pch) || *pch == '_'; pch++) {}
					char* text = copyn(buf, start, pch);
					if (strcmp(text, "var") == 0)addTk(VAR);
					else if (strcmp(text, "function") == 0)addTk(FUNCTION);
					else if (strcmp(text, "if") == 0) addTk(IF);
					else if (strcmp(text, "else") == 0)addTk(ELSE);
					else if (strcmp(text, "while") == 0) addTk(WHILE);
					else if (strcmp(text, "end") == 0) addTk(END);
					else if (strcmp(text, "return") == 0) addTk(RETURN);
					else if (strcmp(text, "int") == 0) addTk(TYPE_INT);
					else if (strcmp(text, "float") == 0) addTk(TYPE_REAL);
					else if (strcmp(text, "double") == 0)addTk(TYPE_REAL);
					else if (strcmp(text, "string") == 0)addTk(TYPE_STR);
					else if (strcmp(text, "char") == 0) addTk(TYPE_STR);
					else {
						tk = addTk(ID);
						strcpy(tk->text, text);
						/*printf("%s \n", text);*/
					}
				
				}
				
				else err("invalid char: %c (%d)",*pch,*pch);
			}
		}
	}

	void showTokens() {
		for (int i = 0; i < nTokens; i++) {
			Token* tk = &tokens[i];
			printf("%d ", tk->line); // Afiseaza mai intai linia

			switch (tk->code) {
			case ID:
				printf("ID: %s", tk->text);
				break;
			case VAR:
				printf("VAR");
				break;
			case FUNCTION:
				printf("FUNCTION");
				break;
			case IF:
				printf("IF");
				break;
			case ELSE:
				printf("ELSE");
				break;
			case WHILE:
				printf("WHILE");
				break;
			case END:
				printf("END");
				break;
			case RETURN:
				printf("RETURN");
				break;
			case TYPE_INT:
				printf("TYPE_INT");
				break;
			case TYPE_REAL:
				printf("TYPE_REAL");
				break;
			case TYPE_STR:
				printf("TYPE_STR");
				break;
			case REAL:
				printf("REAL : %lf", tk->r);
				break;
			case INT:
				printf("INT : %d", tk->i);
				break;
			case STR:
				printf("STR :%s", tk->text);
				break;
			case COMMA:
				printf("COMMA");
				break;
			case FINISH:
				printf("FINISH");
				break;
			case COLON:
				printf("COLON");
				break;
			case SEMICOLON:
				printf("SEMICOLON");
				break;
			case LPAR:
				printf("LPAR");
				break;
			case RPAR:
				printf("RPAR");
				break;
			case SPACE:
				printf("SPACE");
				break;
			case ASSIGN:
				printf("ASSIGN");
				break;
			case EQUAL:
				printf("EQUAL");
				break;
			case ADD:
				printf("ADD");
				break;
			case SUB:
				printf("SUB");
				break;
			case MUL:
				printf("MUL");
				break;
			case DIV:
				printf("DIV");
				break;
			case AND:
				printf("AND");
				break;
			case OR:
				printf("OR");
				break;
			case NOT:
				printf("NOT");
				break;
			case NOTEQ:
				printf("NOTEQ");
				break;
			case LESS:
				printf("LESS");
				break;
			case GREATER:
				printf("GREATER");
				break;
			case GREATEREQ:
				printf("GREATEREQ");
				break;
			default:
				printf("UNKNOWN");
				break;
			}

			printf("\n");
		}
	}

