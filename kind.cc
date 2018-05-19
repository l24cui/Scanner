#include "kind.h"
#include <sstream>
using std::string;
using std::istringstream;

// Use the annonymous namespace to prevent external linking
namespace {
  // Template function to convert from a string to some Type
  // Primarily used for string to integer conversions
  template <typename T>
  T fromString(const string& s, bool hex = false){
    istringstream iss(s);
    T n;
    if(hex)
      iss >> std::hex;
    if(iss >> n)
      return n;
    else
      throw string("ERROR: Type not convertible from string.");
  }
  // Strings representing the Kind of Token for printing
  const string kindStrings[] ={
    "ID",
    "NUM",
    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "RETURN",
    "IF",
    "ELSE",
    "WHILE",
    "PRINTLN",
    "WAIN",
    "BECOMES",
    "INT",
    "EQ",
    "NE",
    "LT",
    "GT",
    "LE",
    "GE",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PCT",
    "COMMA",
    "SEMI",
    "NEW",
    "DELETE",
    "LBRACK",
    "RBRACK",
    "AMP",
    "NULL",
    "WHITESPACE",
    "ERR"
  };
}

ASM::Token::Token(ASM::Kind kind, string lexeme)
: kind(kind), lexeme(lexeme){}

// Cannot convert Tokens that do not represent numbers
// to an Integer
int ASM::Token::toInt() const{
  throw "ERROR: attempt to convert non-integer token: " + lexeme;
}

string ASM::Token::toString() const{
  return ::kindStrings[kind];
}

string ASM::Token::getLexeme() const{
  return lexeme;
}

ASM::Kind ASM::Token::getKind() const{
  return kind;
}

ASM::NumToken::NumToken(ASM::Kind kind, string lexeme)
: Token(kind,lexeme){}

// Ensure that the IntToken is a valid Integer for conversion
// Arguably, we could do this check in the constructor
int ASM::NumToken::toInt() const{
  long long l = ::fromString<long long>(lexeme);
  if(l >= 0){
    if(l <= 2147483647LL) {
      return l;
    } else {
      throw "ERROR: constant out of range: "  + lexeme;
    }
  } else{
    throw "ERROR: constant out of range: " + lexeme;
  }
}

// Print the pertinent information to the ostream
std::ostream& ASM::operator<<(std::ostream& out, const ASM::Token& t){
  out << t.toString() << " ";
  if (t.getKind() == NUM) {
    out << t.toInt();
  } else {
    out << t.getLexeme();
  }
  return out;
}

