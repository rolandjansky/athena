#ifndef ROOT_CORE_UTILS__STRING_UTIL_H
#define ROOT_CORE_UTILS__STRING_UTIL_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wkeyword-macro"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop

#include <string>

class TString;

namespace RCU
{
  /// effects: substitute all occurences of "pattern" with "with" in
  ///   the string "str"
  /// returns: the substituted string
  /// guarantee: out of memory II
  /// requires: !pattern.empty()
  std::string substitute (const std::string& str, const std::string& pattern,
			  const std::string& with);


  /// returns: whether we can match the entire string with the regular
  ///   expression
  /// guarantee: strong
  /// failures: out of memory II
  bool match_expr (const boost::regex& expr, const std::string& str);


  /// returns: a string that is the regular expression equivalent of
  ///   the given glob expression
  /// guarantee: strong
  /// failures: out of memory II
  /// rationale: I am returning a TString instead of an std::string,
  ///   so that this can be passed directly into regexp
  std::string glob_to_regexp (const std::string& glob);
}

#endif
