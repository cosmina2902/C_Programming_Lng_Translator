#include<stdlib.h>
#include<stdio.h>
#include"utils.h"
#include"lexer.h"
int main() {
	
	//definitii regulate ale limbajului -> definesc atomii lexical -> prin expresii regulate
	// DR = ER -> obt prin expresii "mai relaxate" : sptaii nu mai conteaza, "" = ''
	//ER -> DR-? ALEX(Analizatorul nostru lexical)
	//Met 1. Automat finit : din starea 0 plecam intr-o stare viitoare de ex starea 1 primeste un simbol ia din prima paranteza,
	// apoi merge in starea finala si primeste un tip -> diagrama de tranzictie
	//Met 2. luam definitiile atomilor si incepem sa implementez def cu un singur caracter apoi cu 2 etc...
	char* inbuf = loadFile("test/1.q");
	tokenize(inbuf);
	showTokens();
	free(inbuf);

	return 0;
}