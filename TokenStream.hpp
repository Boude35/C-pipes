#ifndef _TOKEN_STREAM_HPP_
#define _TOKEN_STREAM_HPP_

#include "Token.hpp"




class TokenStream
{
private:
  int **_dfa;  // two dimensional array "go to" representation of DFA for tokens
public:
  TokenStream(); // default constructor

  // copy one token stream into another at costruction
  TokenStream(const TokenStream &ts) {_dfa=ts._dfa;}
  
  Token nextToken(); // get the next token from this stream. 
};

#endif
