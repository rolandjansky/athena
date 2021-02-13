/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CoralDB/Encodable.h"

#include <vector>
#include <limits>

namespace CoralDB {
#if 0
} // this brace is for better Emacs indentation
#endif


//================================================================
std::ostream& operator<<(std::ostream& os, const Encodable& s) {
  typedef unsigned char uchar;
  static std::vector<char> encode_mapping; // don't use vector<bool>
  if(encode_mapping.empty()) {
    encode_mapping.resize(1 + std::numeric_limits<uchar>::max(), 0);
    encode_mapping[uchar('\t')] = 't';
    encode_mapping[uchar('\n')] = 'n';
    encode_mapping[uchar('\\')] = '\\';
    // just in case
    encode_mapping[uchar(0)] = '0';
  }

  for(std::string::const_iterator i=s.begin(); i!=s.end(); ++i) {
    char c = encode_mapping[uchar(*i)];
    if(c) {
      os.put('\\');
      os.put(c);
    }
    else {
      os.put(*i);
    }
  }

  return os;
}

//================================================================
std::istream& operator>>(std::istream& is, Encodable& s) {
  typedef unsigned char uchar;
  const int invalid_char = std::numeric_limits<char>::max() + 1;
  static std::vector<int> decode_mapping;
  if(decode_mapping.empty()) {
    decode_mapping.resize(1 + std::numeric_limits<uchar>::max(), invalid_char);
    decode_mapping[uchar('t')] = '\t';
    decode_mapping[uchar('n')] = '\n';
    decode_mapping[uchar('\\')] = '\\';
    decode_mapping[uchar('0')] = 0;
  }

  char in;
  while( !is.get(in).eof() && (in != '\n') ) {
    // cerr<<"Got "<<in<<endl;
    if(in == '\\') {

      char next;
      if( is.get(next).eof() ) { // invalid input: unexpectedly terminated
	//cerr<<"Decoding error, line "<<__LINE__<<endl;
	is.setstate( is.badbit );
	break;
      }

      int c = decode_mapping[uchar(next)];
      if(c == invalid_char) { // invalid input: unknown escape sequence
	//cerr<<"Decoding error, line "<<__LINE__<<endl;
	is.setstate( is.badbit );
	break;
      }

      s.push_back(char(c));

    }
    else {
      s.push_back(in);
    }
  }

  return is;
}

//================================================================

#if 0
{ // to make num braces even
#endif
} // namespace CoralDB
