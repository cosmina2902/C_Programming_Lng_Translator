#include <stdio.h>
#include <ctype.h>
#include <string.h>

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
			case '&&': addTk(AND); pch+2; break;
			case '||': addTk(OR); pch+2; break;
			case '!':
				if (pch[1] == "=") {
					addTk(NOT);
					pch += 2;
				}
				else {
					addTk(NOT);
					pch++;
				}
			case '>':
				if (pch[1] == "=") {
					addTk(GREATEREQ);
					pch += 2;
				}
				else {
					addTk(GREATER);
					pch++;
				}
//comment
			case '<': addTk(LESS); pch++; break;
			case '"':
				for (start = pch++; *pch == '"'; pch++) {}
				if (*pch == '\0')err("end of file while in string");
				tk = addTk(STR);
				copyn(tk->text, start, pch);
				pch++;
				break;
			case '#':
				for (start = pch++; *pch == '/0'; pch++) {}
				line++;
				pch ++;
			default:
				if (isdigit(*pch) || (*pch == '-' && isdigit(*(pch + 1)))) {
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
					if (strcmp(text, "function") == 0)addTk(FUNCTION);
					if (strcmp(text, "if") == 0) addTk(IF);
					if (strcmp(text, "else") == 0)addTk(ELSE);
					if (strcmp(text, "while") == 0) addTk(WHILE);
					if (strcmp(text, "end") == 0) addTk(END);
					if (strcmp(text, "return") == 0) addTk(RETURN);
					if (strcmp(text, "float") == 0) addTk(TYPE_REAL);
					if (strcmp(text, "double") == 0)addTk(TYPE_REAL);
					if (strcmp(text, "string") == 0)addTk(TYPE_STR);
					if (strcmp(text, "char") == 0) addTk(TYPE_STR);
					else {
						tk = addTk(ID);
						strcpy(tk->text, text);
					}
				
				}
				
				else err("invalid char: %c (%d)",*pch,*pch);
			}
		}
	}

void showTokens(){
	for(int i=0;i<nTokens;i++){
		Token *tk=&tokens[i];
		printf("%d\n",tk->code);
		}
	}
