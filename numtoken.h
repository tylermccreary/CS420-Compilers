#ifndef NUMTOKEN_H
#define NUMTOKEN_H

using namespace std;

#include "token.h"
#include <string>

/* NumToken is a derived class (subclass) of Token */
class NumToken : public Token
{
 public:

  NumToken() ;

  NumToken(string *attribute);

  ~NumToken();

  /* Every subclass of Token (except the EOFToken) needs to have a
     get_attribute() method, but the signature of the method will vary
     depending on the type of the attribute.

     Get_Attribute returns the attribute of this token.  If the
     attribute is an object, we return a copy of the object.  This is
     safer than returning a pointer to the object.
  */
  string *get_attribute() const;

  /* Similarly, every subclass of Token will need to have a
     set_attribute() method.

     Set_attribute() sets the attribute of this token.  If the
     attribute is an object, then we store a pointer to a copy of the
     object with the token.  Again, this is for encapsulation
     purposes.
   */
  void set_attribute(string *attr);

  /* This method is overridden from the base class.  It is for
     debugging only.  Return a string that represents a reasonable
     representation of the token, including its type and attribute, if
     any.
  */
  string *to_string();

 private:
 // Storage for the attribute of the token.
  string *attribute;
};

#endif
