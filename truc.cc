#include "keywordtoken.h"
#include "addoptoken.h"
#include "idtoken.h"
#include "muloptoken.h"
#include "numtoken.h"
#include "punctoken.h"
#include "reloptoken.h"
#include "eoftoken.h"
#include "buffer.h"
#include "scanner.h"
#include "parser.h"

#include <iostream>

/* Creates a scanner and parser to parse a TruPL program
   and generate code.
*/
int main(int argc, char *argv[])
{
    bool success;
	//scanner test
	Scanner *scanner = new Scanner(argv[1]);

	Parser *parser = new Parser(scanner);

	success = parser->parse_program();
    if (success) {
        if (parser->done_with_input()) {
            cout << "The parser has succeeded." << endl;
        } else {
            cout << "The parse failed." << endl;
        }
    } else {
        cout << "The parse failed." << endl;
    }
}
