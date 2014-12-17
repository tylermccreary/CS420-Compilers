#ifndef EOFTOKEN_H
#define EOFTOKEN_H

using namespace std;

#include "token.h"
#include <string>

/* EofToken is a derived class (subclass) of Token */
class EofToken : public Token
{
 public:

	// Construct an eof token
  EofToken() ;

	// Destructor for eof token
  ~EofToken();

  string *to_string();

 private:
 // Storage for the attribute of the token.
  string *attribute;
};

#endif
