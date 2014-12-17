#include "numtoken.h"

/* The constructor for the Num Token.  We call the base class constructor here.  
   This isn't strictly necessary, because the default constructor of the base
   class will be called anyway, but we do it here for completeness.
*/
NumToken::NumToken() : Token()
{
  set_token_type (TOKEN_NUM);
}

NumToken::NumToken (string *attr) : Token()
{
  set_token_type (TOKEN_NUM);
  attribute = attr;
}

NumToken::~NumToken()
{
}

string *NumToken::get_attribute() const
{
	string *attr = new string(*attribute);
  return attr;
}

void NumToken::set_attribute(string *attr)
{ 
  attribute = attr;
}

string *NumToken::to_string()
{
  string *attribute_name = new string ("NUM:" + *attribute);
  return attribute_name;
}
