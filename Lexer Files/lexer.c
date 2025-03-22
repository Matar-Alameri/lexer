#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

int NumberKeywords = 21;
const char* keywords[] = { "class" , "constructor" , "method", "function" , "int" ,
	 "boolean" , "char" , "void" , "var", "static", "field" , "let" ,
	  "do", "if", "else", "while", "return", "true", "false", "null", "this"};
int NumberSymbols = 19;
char symbols[] =  {'=', '+', '-', '*', '/', '&', '|', '<', '>', '~',
	 '(', ')', '[', ']', '{', '}', ',', ';', '.'};
FILE* f;
int TokenReady; 
Token t;
int LineCount;

int Init(char* file_name) // init the lexer
{
	
	
	f = fopen(file_name, "r");
	if (f == 0)
		return 0;
	TokenReady = 0;
	LineCount = 1;
	return 1;
}


int IsKeyWord(char* str)
{
	for (int i = 0; i < NumberKeywords; i++)
		if (!strcmp(keywords[i], str))
			return 1;
	return 0;
}
int IsSymbol(char str)
{
	for (int i = 0; i < NumberSymbols; i++)
		if (symbols[i] == str)
			return 1;
	return 0;
}

// eat ws and c and return first non white space char
int EatWC()
{
	// consume white space and comments
	int c;
	c = getc(f);
	while (c != EOF && isspace(c))
	{
		if ((char) c == '\n') LineCount++;
		c = getc(f);
	}
	// we have hit non-white-space
	
	if (c == '/')
	{
		c = getc(f);
		while (c != EOF && c != '\n')
		{
			if (c == '/')
				c = getc(f);
				while (c != EOF && c != '\n')
				{
					ungetc(c, f);
					return EatWC();
				}
				if (c == EOF)
				{
					t.ec = EofInCom;
					return t.ec;
				}
			else if (c == '*')
			{
				int prev_c = '\0';
				c = getc(f);
				while (c != EOF && (c != '/' || prev_c != '*'))
				{
					if ((char)c == '\n') LineCount++;
					prev_c = c;
					c = getc(f);
				}	
				if (c == EOF)
				{
					t.ec = EofInCom;
					return t.ec;
				}
				EatWC();
			}
			else {
				ungetc(c, f);
			}
		}	
	}
	

	
	return c;
}
int StopLexer ()
{
  if(f != NULL){
    fclose(f);
    f = NULL;
  }
  return 1;
}


Token BuildToken()
{

	int c = EatWC();
	char temp[128];
	int i = 0;
	while ((c = getc(f)) != EOF)
	{
		if (isalpha(c) || c == '_')
		{
			while (c != EOF && (isalpha(c) || c == '_' || isdigit(c)))
			{
				temp[i++] = c;
				c = getc(f);
			}
			temp[i] = '\0';
			ungetc(c, f);

			strcpy(t.lx, temp);
			if (IsKeyWord(temp))
				t.tp = RESWORD;
			else
				t.tp = ID;
			t.ln = LineCount;
			return t;
		}
		else if (isdigit(c))
		{
			while (c != EOF && isdigit(c))
			{
				temp[i++] = c;
				c = getc(f);
			}
			temp[i] = '\0';
			ungetc(c, f);
			strcpy(t.lx, temp);
			t.tp = INT;
			t.ln = LineCount;
			return t;

		}
		
		else if (c == '"')
		{
			c = getc(f);
			while (c != '"')
			{
				if(c == '\n')
				{
					strcpy(t.lx, "Error: new line in string constant");
					t.ln = LineCount;
					t.tp = ERR;
					return t;
				}
				else if(c == EOF)
				{
					strcpy(t.lx, "Error: unexpected eof in string constant");
					t.ln = LineCount;
					t.tp = ERR;
					return t;
				}
				temp[i++] = c;
				c = getc(f);
			}
			temp[i] = '\0';
			strcpy(t.lx, temp);
			t.tp = STRING;
			t.ln = LineCount;
			return t;
		}
		else
		{
			temp[0] = c;
			temp[1] = '\0';
			t.tp = ERR;
			strcpy(t.lx, temp);
			if (IsSymbol(c))
				t.tp = SYMBOL;
			else
				strcpy(t.lx, "Error: illegal symbol in source file");
			
			t.ln = LineCount;
			return t;
		}
	}//return t;
	if (c == EOF)
	{
		t.tp = EOFile;
		t.ln = LineCount;
		return t;
	}
}

Token GetToken()
{
	if (TokenReady)
	{
		TokenReady = 0;
		return t;
	}
	t = BuildToken();
	TokenReady = 0;
	return t;
}
Token PeekToken()
{
	if (TokenReady)
		return t;
	t = BuildToken();
	TokenReady = 1;
	return t;
}
int main()
{
	
	Init("Main.jack");
	EatWC();
	GetToken();
	
	

	return 0;
}