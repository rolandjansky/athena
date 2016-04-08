/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORUTILS_STRINGPARSE_H
#define GENERATORUTILS_STRINGPARSE_H
#include <string>
#include <vector>
#include <sstream>
#include "boost/lexical_cast.hpp"

/// @brief Utility object for parsing a string into tokens and returning them as a variety of types
///
/// This code is used to parse a string, tokenising space-separated parts into component 'pieces'.
///
/// Methods can then be called to
///   1. Return the nth component as a string (piece method)
///   2. Return the nth component as an integer (intpiece and longpiece methods) if conversion is possible
///      (returns -1 if it is off the end and 0 if it cannot be converted)
///   3. Return the nth component as a double (numpiece method) if conversion is possible
///      (returns -1.1 if it is off the end and 0 if it cannot be converted)
///
/// Note that the 'num' index used to retrieve the pieces starts at 1, not the C-conventional 0.
///
/// @author Ian Hinchliffe, April 2000
/// @author Yun-Ha.Shin, June 2006
/// @author Andy Buckley, September 2012
///
class StringParse {
public:

  /// Constructor, taking a string of whitespace-separated tokens
  StringParse(const std::string& input) {
    m_lstring = input;
    std::istringstream instring(input);
    std::string token;
    while (instring >> token) {
      m_lsubstring.push_back(token);
    }
  }

  /// Templated function to get the num'th token as any type (via stringstream)
  template <typename T>
  T piece(size_t num) {
    try {
      std::string token = (num > num_pieces()) ? "?!?" : m_lsubstring[num-1];
      return boost::lexical_cast<T>(token);
    } catch (const boost::bad_lexical_cast& ex) {
      return boost::lexical_cast<T>("-1");
    }
  }

  /// Function to get the num'th token as a string
  std::string piece(size_t num) {
    return piece<std::string>(num);
  }

  /// Function to get the num'th token as an int
  int intpiece(size_t num) {
    if (num > num_pieces()) return -1;
    return piece<int>(num);
  }

  /// Function to get the num'th token as a long int
  long longpiece(size_t num) {
    if (num > num_pieces()) return -1;
    return piece<long>(num);
  }

  /// Function to get the num'th token as a double
  double numpiece(size_t num) {
    if (num > num_pieces()) return -1;
    return piece<double>(num);
  }

  /// Number of tokens in the input string
  size_t num_pieces() const { return m_lsubstring.size(); }

  /// @brief Number of tokens in the input string
  /// @deprecated Prefer num_pieces, which is a more accurate name
  unsigned string_size() const { return num_pieces(); }

private:

  std::string m_lstring;
  std::vector<std::string> m_lsubstring;

};


#endif
