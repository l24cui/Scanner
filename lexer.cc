#include "lexer.h"
#include "kind.h"
using std::string;
using std::vector;
using std::cout;
using std::endl;

// Use the annonymous namespace to prevent external linking
namespace {
  // An array represents the Token kind that each state represents
  // The ERR Kind represents an invalid token
  ASM::Kind stateKinds[] = {
    ASM::ERR,            // ST_START
    ASM::ERR,            // ST_ERR
    ASM::ID,             // ST_ID
    ASM::NUM,            // ST_NUM
    ASM::NUM,            // ST_ZERO
    ASM::LPAREN,         // ST_LPAREN
    ASM::RPAREN,         // ST_RPAREN
    ASM::LBRACE,         // ST_LBRACE
    ASM::RBRACE,         // ST_RBRACE
    ASM::ID,             // ST_R
    ASM::ID,             // ST_RE
    ASM::ID,             // ST_RET
    ASM::ID,             // ST_RETU
    ASM::ID,             // ST_RETUR
    ASM::RETURN,         // ST_RETURN
    ASM::ID,             // ST_I
    ASM::IF,             // ST_IF
    ASM::ID,             // ST_E
    ASM::ID,             // ST_EL
    ASM::ID,             // ST_ELS
    ASM::ELSE,           // ST_ELSE
    ASM::ID,             // ST_W
    ASM::ID,             // ST_WH
    ASM::ID,             // ST_WHI
    ASM::ID,             // ST_WHIL
    ASM::WHILE,          // ST_WHILE
    ASM::ID,             // ST_P
    ASM::ID,             // ST_PR
    ASM::ID,             // ST_PRI
    ASM::ID,             // ST_PRIN
    ASM::ID,             // ST_PRINT
    ASM::ID,             // ST_PRINTL
    ASM::PRINTLN,        // ST_PRINTLN
    ASM::ID,             // ST_WA
    ASM::ID,             // ST_WAI
    ASM::WAIN,           // ST_WAIN
    ASM::BECOMES,        // ST_BECOMES
    ASM::ID,             // ST_IN
    ASM::INT,            // ST_INT
    ASM::ERR,            // ST_NOT
    ASM::EQ,             // ST_EQ
    ASM::NE,             // ST_NE
    ASM::LT,             // ST_LT
    ASM::GT,             // ST_GT
    ASM::LE,             // ST_LE
    ASM::GE,             // ST_GE
    ASM::PLUS,           // ST_PLUS
    ASM::MINUS,          // ST_MINUS
    ASM::STAR,           // ST_STAR
    ASM::SLASH,          // ST_SLASH
    ASM::PCT,            // ST_PCT
    ASM::COMMA,          // ST_COMMA
    ASM::SEMI,           // ST_SEMI
    ASM::ID,             // ST_n
    ASM::ID,             // ST_Ne
    ASM::NEW,            // ST_NEW
    ASM::ID,             // ST_D
    ASM::ID,             // ST_DE
    ASM::ID,             // ST_DEL
    ASM::ID,             // ST_DELE
    ASM::ID,             // ST_DELET
    ASM::DELETE,         // ST_DELETE
    ASM::LBRACK,         // ST_LBRACK
    ASM::RBRACK,         // ST_RBRACK
    ASM::AMP,            // ST_AMP
    ASM::ID,             // ST_N
    ASM::ID,             // ST_NU
    ASM::ID,             // ST_NUL
    ASM::Null,           // ST_Null
    ASM::WHITESPACE,     // ST_WHITESPACE
    ASM::WHITESPACE      // ST_COMMENT
  };
  const string space(1,32);
  const string tab(1,9);
  const string newline(1,10);
  const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string digits = "0123456789";
  const string oneToNine =  "123456789";
  const string specialchar = "!%&()*+,-/;<=>[]{}";
  const string NoNdeinprw = "ABCDEFGHIJKLMOPQRSTUVWXYZabcfghjklmoqstuvxyz";
  const string NoU = "ABCDEFGHIJKLMNOPQRSTVWXYZabcdefghijklmnopqrstuvwxyz";
  const string NoL = "ABCDEFGHIJKMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string Noe = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdfghijklmnopqrstuvwxyz";
  const string Nol = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
  const string Not = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrsuvwxyz";
  const string Nos = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrtuvwxyz";
  const string Nofn = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdeghijklmopqrstuvwxyz";
  const string Now = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz";
  const string Nor = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqstuvwxyz";
  const string Noi = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghjklmnopqrstuvwxyz";
  const string Non = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz";
  const string Nou = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstvwxyz";
  const string Noah = "ABCDEFGHIJKLMNOPQRSTUVWXYZbcdefgijklmnopqrstuvwxyz";
}

ASM::Lexer::Lexer(){
  // Set default transitions to the Error state
  for(int i=0; i < maxStates; ++i){
    for(int j=0; j < maxTrans; ++j){
      delta[i][j] = ST_ERR;
    }
  }
  // Change transitions as appropriate for the WLP4 recognizer
  setTrans(ST_START, NoNdeinprw, ST_ID);
  setTrans(ST_START, "N", ST_N);
  setTrans(ST_START, "d", ST_D);
  setTrans(ST_START, "e", ST_E);
  setTrans(ST_START, "i", ST_I);
  setTrans(ST_START, "n", ST_n);
  setTrans(ST_START, "p", ST_P);
  setTrans(ST_START, "r", ST_R);
  setTrans(ST_START, "w", ST_W);
  setTrans(ST_START, "!", ST_NOT);
  setTrans(ST_START, "%", ST_PCT);
  setTrans(ST_START, "&", ST_AMP);
  setTrans(ST_START, "(", ST_LPAREN);
  setTrans(ST_START, ")", ST_RPAREN);
  setTrans(ST_START, "*", ST_STAR);
  setTrans(ST_START, "+", ST_PLUS);
  setTrans(ST_START, ",", ST_COMMA);
  setTrans(ST_START, "-", ST_MINUS);
  setTrans(ST_START, "/", ST_SLASH);
  setTrans(ST_START, "0", ST_ZERO);
  setTrans(ST_START, oneToNine, ST_NUM);
  setTrans(ST_START, ";", ST_SEMI);
  setTrans(ST_START, "<", ST_LT);
  setTrans(ST_START, "=", ST_BECOMES);
  setTrans(ST_START, ">", ST_GT);
  setTrans(ST_START, "[", ST_LBRACK);
  setTrans(ST_START, "]", ST_RBRACK);
  setTrans(ST_START, "{", ST_LBRACE);
  setTrans(ST_START, "}", ST_RBRACE);
  setTrans(ST_START, space+tab+newline, ST_WHITESPACE);
  setTrans(ST_N, NoU+digits, ST_ID);
  setTrans(ST_N, "U", ST_NU);
  setTrans(ST_NU, NoL+digits, ST_ID);
  setTrans(ST_NU, "L", ST_NUL);
  setTrans(ST_NUL, NoL+digits, ST_ID);
  setTrans(ST_NUL, "L", ST_Null);
  setTrans(ST_Null, letters+digits, ST_ID);
  setTrans(ST_D, Noe+digits, ST_ID);
  setTrans(ST_D, "e", ST_DE);
  setTrans(ST_DE, Nol+digits, ST_ID);
  setTrans(ST_DE, "l", ST_DEL);
  setTrans(ST_DEL, Noe+digits, ST_ID);
  setTrans(ST_DEL, "e", ST_DELE);
  setTrans(ST_DELE, Not+digits, ST_ID);
  setTrans(ST_DELE, "t", ST_DELET);
  setTrans(ST_DELET, Noe+digits, ST_ID);
  setTrans(ST_DELET, "e", ST_DELETE);
  setTrans(ST_DELETE, letters+digits, ST_ID);
  setTrans(ST_E, Nol+digits, ST_ID);
  setTrans(ST_E, "l", ST_EL);
  setTrans(ST_EL, Nos+digits, ST_ID);
  setTrans(ST_EL, "s", ST_ELS);
  setTrans(ST_ELS, Noe+digits, ST_ID);
  setTrans(ST_ELS, "e", ST_ELSE);
  setTrans(ST_ELSE, letters+digits, ST_ID);
  setTrans(ST_I, Nofn+digits, ST_ID);
  setTrans(ST_I, "f", ST_IF);
  setTrans(ST_I, "n", ST_IN);
  setTrans(ST_IF, letters+digits, ST_ID);
  setTrans(ST_IN, Not+digits, ST_ID);
  setTrans(ST_IN, "t", ST_INT);
  setTrans(ST_INT, digits+letters, ST_ID);
  setTrans(ST_n, Noe+digits, ST_ID);
  setTrans(ST_n, "e", ST_Ne);
  setTrans(ST_Ne, Now+digits, ST_ID);
  setTrans(ST_Ne, "w", ST_NEW);
  setTrans(ST_NEW, digits+letters, ST_ID);
  setTrans(ST_P, Nor+digits, ST_ID);
  setTrans(ST_P, "r", ST_PR);
  setTrans(ST_PR, Noi+digits, ST_ID);
  setTrans(ST_PR, "i", ST_PRI);
  setTrans(ST_PRI, Non+digits, ST_ID);
  setTrans(ST_PRI, "n", ST_PRIN);
  setTrans(ST_PRIN, Not+digits, ST_ID);
  setTrans(ST_PRIN, "t", ST_PRINT);
  setTrans(ST_PRINT, Nol+digits, ST_ID);
  setTrans(ST_PRINT, "l", ST_PRINTL);
  setTrans(ST_PRINTL, Non+digits, ST_ID);
  setTrans(ST_PRINTL, "n", ST_PRINTLN);
  setTrans(ST_PRINTLN, digits+letters, ST_ID);
  setTrans(ST_R, Noe+digits, ST_ID);
  setTrans(ST_R, "e", ST_RE);
  setTrans(ST_RE, Not+digits, ST_ID);
  setTrans(ST_RE, "t", ST_RET);
  setTrans(ST_RET, Nou+digits, ST_ID);
  setTrans(ST_RET, "u", ST_RETU);
  setTrans(ST_RETU, Nor+digits, ST_ID);
  setTrans(ST_RETU, "r", ST_RETUR);
  setTrans(ST_RETUR, Non+digits, ST_ID);
  setTrans(ST_RETUR, "n", ST_RETURN);
  setTrans(ST_RETURN, letters+digits, ST_ID);
  setTrans(ST_W, Noah+digits, ST_ID);
  setTrans(ST_W, "a", ST_WA);
  setTrans(ST_W, "h", ST_WH);
  setTrans(ST_WH, Noi+digits, ST_ID);
  setTrans(ST_WH, "i", ST_WHI);
  setTrans(ST_WHI, Nol+digits, ST_ID);
  setTrans(ST_WHI, "l", ST_WHIL);
  setTrans(ST_WHIL, Noe+digits, ST_ID);
  setTrans(ST_WHIL, "e", ST_WHILE);
  setTrans(ST_WHILE, digits+letters, ST_ID);
  setTrans(ST_WA, Noi+digits, ST_ID);
  setTrans(ST_WA, "i", ST_WAI);
  setTrans(ST_WAI, Non+digits, ST_ID);
  setTrans(ST_WAI, "n", ST_WAIN);
  setTrans(ST_WAIN, letters+digits, ST_ID);
  setTrans(ST_ID, digits+letters, ST_ID);
  setTrans(ST_SLASH, "/", ST_COMMENT);
  setTrans(ST_COMMENT, letters+digits+specialchar+space+tab, ST_COMMENT);
  setTrans(ST_NOT, "=", ST_NE);
  setTrans(ST_NUM, digits, ST_NUM);
  setTrans(ST_LT, "=", ST_LE);
  setTrans(ST_BECOMES, "=", ST_EQ);
  setTrans(ST_GT, "=", ST_GE);

  // A comment can only ever lead to the comment state
  for(int j=0; j < maxTrans; ++j) delta[ST_COMMENT][j] = ST_COMMENT;
}

// Set the transitions from one state to another state based upon characters in the
// given string
void ASM::Lexer::setTrans(ASM::State from, const string& chars, ASM::State to){
  for(string::const_iterator it = chars.begin(); it != chars.end(); ++it)
    delta[from][static_cast<unsigned int>(*it)] = to;
}

// Scan a line of input (as a string) and return a vector
// of Tokens representing the WLP4 instruction in that line
void ASM::Lexer::scan(const string& line){
  // Previous encountered Token is white space if and only if separated = 1 
  int separated = 0;
  Kind prev = WHITESPACE;
  if(line.size() == 0) return;
  // Always begin at the start state
  State currState = ST_START;
  // startIter represents the beginning of the next Token
  // that is to be recognized. Initially, this is the beginning
  // of the line.
  // Use a const_iterator since we cannot change the input line
  string::const_iterator startIter = line.begin();
  // Loop over the the line
  for(string::const_iterator it = line.begin();;){
    // Assume the next state is the error state
    State nextState = ST_ERR;
    // If we aren't done then get the transition from the current
    // state to the next state based upon the current character of
    //input
    if(it != line.end())
      nextState = delta[currState][static_cast<unsigned int>(*it)];
    // If there is no valid transition then we have reach the end of a
    // Token and can add a new Token to the return vector
    if(ST_ERR == nextState){
      // Get the kind corresponding to the current state
      Kind currKind = stateKinds[currState];
      // If we are in an Error state then we have reached an invalid
      // Token - so we throw an error and delete the Tokens parsed
      // thus far
      if(ERR == currKind){
        throw string("ERROR: Lexer error");
      }
      // If we are not in Whitespace then we push back a new token
      // based upon the kind of the State we end in
      // Whitespace is ignored for practical purposes
      if(WHITESPACE != currKind){
        if ((currKind == ID) || (currKind == NUM) || (currKind == RETURN) ||
            (currKind == IF) || (currKind == ELSE) || (currKind == WHILE) ||
            (currKind == PRINTLN) || (currKind == WAIN) || (currKind == INT) ||
            (currKind == NEW) || (currKind == Null) || (currKind == DELETE)) {
          if ((prev == ID) || (prev == NUM) || (prev == RETURN) ||
              (prev == IF) || (prev == ELSE) || (prev == WHILE) ||
              (prev == PRINTLN) || (prev == WAIN) || (prev == INT) ||
              (prev == NEW) || (prev == Null) || (prev == DELETE)) {
            if (separated == 0) {
              throw string("ERROR: WHITESPACE TOKEN MISSING!");
            }
          }
          separated = 0;
        } else if ((currKind == EQ) || (currKind == NE) || (currKind == LT) ||
                   (currKind == LE) || (currKind == GT) || (currKind == GE) ||
                   (currKind == BECOMES)) {
          if ((prev == EQ) || (prev == NE) || (prev == LT) ||
              (prev == LE) || (prev == GT) || (prev == GE) ||
              (prev == BECOMES)) {
            if (separated == 0) {
              throw string("ERROR: WHITESPACE TOKEN MISSING!");
            }
          }
          separated = 0;
        } else {
          separated = 1;
        }
        if (currKind == NUM) {
          cout << NumToken(currKind,string(startIter,it)) << endl;
        } else {
          cout << Token(currKind,string(startIter,it)) << endl;
        }
        prev = currKind;
      } else {
        separated = 1;
      }
      // Start of next Token begins here
      startIter = it;
      // Revert to start state to begin recognizing next token
      currState = ST_START;
      if(it == line.end()) break;
    } else {
      // Otherwise we proceed to the next state and increment the iterator
      currState = nextState;
      ++it;
    }
  }
}
