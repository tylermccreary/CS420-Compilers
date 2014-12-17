#include "scanner.h"

Scanner::Scanner(char *filename)
{
	buf = new Buffer(filename);
}

Scanner::~Scanner()
{
	delete buf;
}

// Return the next token in the file.
Token *Scanner::next_token()
{
	char c;
	int state = 0;
	attr = new string("");
	while(state != 1) //final state is state 1
	{
		c = buf -> next_char();
		switch(state)
		{
			case 0: //start state
				if(c == 'p')
				{
					attr -> push_back(c);
					state = 1000;
				}
				else if(c == 'i')
				{
					attr -> push_back(c);
					state = 1020;
				}
				else if(c == 'b')
				{
					attr -> push_back(c);
					state = 1030;
				}
				else if(c == 'e')
				{
					attr -> push_back(c);
					state = 1040;
				}
				else if(c == 't')
				{
					attr -> push_back(c);
					state = 1050;
				}
				else if(c == 'w')
				{
					attr -> push_back(c);
					state = 1060;
				}
				else if(c == 'n')
				{
					attr -> push_back(c);
					state = 1070;
				}
				else if(c == 'a')
				{
					attr -> push_back(c);
					state = 601;
				}
				else if(c == 'o')
				{
					attr -> push_back(c);
					state = 401;
				}
			//test if any other letter
				else if(is_alpha(c))
				{
					attr -> push_back(c);
					state = 700;
				}
			//test if number
				else if(is_digit(c))
				{
					attr -> push_back(c);
					state = 900;
				}
			//test other legal chars
				else if(c == ';')
				{
					attr -> push_back(c);
					state = 1;
					token = new PuncToken(PUNC_SEMI);
				}
				else if(c == ',')
				{
					attr -> push_back(c);
					state = 1;
					token = new PuncToken(PUNC_COMMA);
				}
				else if(c == '(')
				{
					attr -> push_back(c);
					state = 1;
					token = new PuncToken(PUNC_OPEN);
				}
				else if(c == ')')
				{
					attr -> push_back(c);
					state = 1;
					token = new PuncToken(PUNC_CLOSE);
				}
				else if(c == ':')
				{
					attr -> push_back(c);
					state = 2010;
				}
				else if(c == '=')
				{
					attr -> push_back(c);
					state = 1;
					token = new RelopToken(RELOP_EQ);
				}
				else if(c == '>')
				{
					attr -> push_back(c);
					state = 302;
				}
				else if(c == '<')
				{
					attr -> push_back(c);
					state = 303;
				}
				else if(c == '+')
				{
					//attr -> push_back(c);
					state = 1;
					token = new AddopToken(ADDOP_ADD);
				}
				else if(c == '-')
				{
					attr -> push_back(c);
					state = 1;
					token = new AddopToken(ADDOP_SUB);
				}
				else if(c == '*')
				{
					attr -> push_back(c);
					state = 1;
					token = new MulopToken(MULOP_MUL);
				}
				else if(c == '/')
				{
					attr -> push_back(c);
					state = 1;
					token = new MulopToken(MULOP_DIV);
				}
				else if(c == '$')
				{
					state = 800;
				}
				else if(!is_legal_char(c))
				{
					scanner_fatal_error();
				}
				break;
			case 1000:  //p
				if(c == 'r')
				{
					attr -> push_back(c);
					state = 1001;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1001: //pr
				if(c == 'o')
				{
					attr -> push_back(c);
					state = 1002;
				}
				else if(c == 'i')
				{
					attr -> push_back(c);
					state = 1013;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1002: //pro
				if(c == 'g')
				{
					attr -> push_back(c);
					state = 1003;
				}
				else if(c == 'c')
				{
					attr -> push_back(c);
					state = 1007;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);				
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1003: //prog
				if(c == 'r')
				{
					attr -> push_back(c);
					state = 1004;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1004: //progr
				if(c == 'a')
				{
					attr -> push_back(c);
					state = 1005;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1005: //progra
				if(c == 'm')
				{
					attr -> push_back(c);
					state = 1006;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1006: //program
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_PROGRAM);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1007: //proc
				if(c == 'e')
				{
					attr -> push_back(c);
					state = 1008;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1008: //proce
				if(c == 'd')
				{
					attr -> push_back(c);
					state = 1009;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1009: //proced
				if(c == 'u')
				{
					attr -> push_back(c);
					state = 1010;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1010: //procedu
				if(c == 'r')
				{
					attr -> push_back(c);
					state = 1011;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1011: //procedur
				if(c == 'e')
				{
					attr -> push_back(c);
					state = 1012;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1012: //procedure
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_PROCEDURE);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1013: //pri
				if(c == 'n')
				{
					attr -> push_back(c);
					state = 1014;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1014: //prin
				if(c == 't')
				{
					attr -> push_back(c);
					state = 1015;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1015: //print
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_PRINT);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1020: //i
				if(c == 'f')
				{
					attr -> push_back(c);
					state = 1021;
				}
				else if(c == 'n')
				{
					attr -> push_back(c);
					state = 1022;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1021: //if
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_IF);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1022: //in
				if(c == 't')
				{
					attr -> push_back(c);
					state = 1023;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1023: //int
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_INT);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1030: //b
				if(c == 'o')
				{
					attr -> push_back(c);
					state = 1031;
				}
				else if(c == 'e')
				{
					attr -> push_back(c);
					state = 1034;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1031: //bo
				if(c == 'o')
				{
					attr -> push_back(c);
					state = 1032;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1032: //boo
				if(c == 'l')
				{
					attr -> push_back(c);
					state = 1033;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1033: //bool
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_BOOL);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1034: //be
				if(c == 'g')
				{
					attr -> push_back(c);
					state = 1035;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1035: //beg
				if(c == 'i')
				{
					attr -> push_back(c);
					state = 1036;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1036: //begi
				if(c == 'n')
				{
					attr -> push_back(c);
					state = 1037;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1037: //begin
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_BEGIN);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1040: //e
				if(c == 'l')
				{
					attr -> push_back(c);
					state = 1041;
				}
				else if(c == 'n')
				{
					attr -> push_back(c);
					state = 1044;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1041: //el
				if(c == 's')
				{
					attr -> push_back(c);
					state = 1042;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1042: //els
				if(c == 'e')
				{
					attr -> push_back(c);
					state = 1043;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1043: //else
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_ELSE);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1044: //en
				if(c == 'd')
				{
					attr -> push_back(c);
					state = 1045;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1045: //end
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_END);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1050: //t
				if(c == 'h')
				{
					attr -> push_back(c);
					state = 1051;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1051: //th
				if(c == 'e')
				{
					attr -> push_back(c);
					state = 1052;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1052: //the
				if(c == 'n')
				{
					attr -> push_back(c);
					state = 1053;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1053: //then
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_THEN);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1060: //w
				if(c == 'h')
				{
					attr -> push_back(c);
					state = 1061;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1061: //wh
				if(c == 'i')
				{
					attr -> push_back(c);
					state = 1062;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1062: //whi
				if(c == 'l')
				{
					attr -> push_back(c);
					state = 1063;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1063: //whil
				if(c == 'e')
				{
					attr -> push_back(c);
					state = 1064;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);	
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1064: //while
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_WHILE);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1070: //n
				if(c == 'o')
				{
					attr -> push_back(c);
					state = 1071;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1071: //no
				if(c == 't')
				{
					attr -> push_back(c);
					state = 1072;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 1072: //not
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new KeywordToken(KW_NOT);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 2010: //:
				if(c == '=')
				{
					attr -> push_back(c);
					state = 1;
					token = new PuncToken(PUNC_ASSIGN);
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new PuncToken(PUNC_COLON);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 302: //>
				if(c == '=')
				{
					attr -> push_back(c);
					state = 1;
					token = new RelopToken(RELOP_GE);
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new RelopToken(RELOP_GT);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 303: //<
				if(c == '=')
				{
					attr -> push_back(c);
					state = 1;
					token = new RelopToken(RELOP_LE);
				}
				else if(c == '>')
				{
					attr -> push_back(c);
					state = 1;
					token = new RelopToken(RELOP_NEQ);								
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new RelopToken(RELOP_LT);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 401: //o
				if(c == 'r')
				{
					attr -> push_back(c);
					state = 402;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 402: //or
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new AddopToken(ADDOP_OR);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 601: //a
				if(c == 'n')
				{
					attr -> push_back(c);
					state = 602;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					state = 1;
					token = new IdToken(attr);
					buf -> unread_char(c);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 602: //an
				if(c == 'd')
				{
					attr -> push_back(c);
					state = 603;
				}
				else if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 603: //and
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					state = 1;
					token = new MulopToken(MULOP_AND);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 900: //number
				if(is_digit(c))
				{
					attr -> push_back(c);
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new NumToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 700: //identifier
				if(is_alphanumeric(c))
				{
					attr -> push_back(c);
					state = 700;
				}
				else if(is_legal_char(c))
				{
					buf -> unread_char(c);
					state = 1;
					token = new IdToken(attr);
				}
				else
				{
					scanner_fatal_error();
				}
				break;
			case 800: //eof
				token = new EofToken();
				state = 1;
				break;
			default:
				scanner_fatal_error();
		}
	}
	return token;
}

/* If you encounter an error that you can't recover from, print an
	error message and call this function.
*/
void Scanner::scanner_fatal_error()
{
	cout << "Scanner fatal error" << endl;
	exit(0);
}



