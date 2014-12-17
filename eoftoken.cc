#include "eoftoken.h"

/* The constructor for the Eof Token.  We call the base class constructor here.  
   This isn't strictly necessary, because the default constructor of the base
   class will be called anyway, but we do it here for completeness.
*/
EofToken::EofToken() : Token()
{
  set_token_type (TOKEN_EOF);
}

EofToken::~EofToken()
{
}

string *EofToken::to_string()
{
  string *attribute_name = new string("EOF");
  return attribute_name;
}
