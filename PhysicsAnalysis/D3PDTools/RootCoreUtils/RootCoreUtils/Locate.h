#ifndef ROOT_CORE_UTILS__LOCATE_H
#define ROOT_CORE_UTILS__LOCATE_H

//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

#include <string>

namespace RCU
{
  /// effects: find the file with the given name from a list of
  ///   locations separated by "::".  the list may contain either
  ///   files or URLs starting with "http://".  URLs will be
  ///   downloaded into the data/ directory, where they will stay
  ///   permanently.
  /// returns: the path in the local filesystem
  /// guarantee: strong
  /// failures: out of memory III
  /// failures: inconsistent file names
  /// failures: download errors
  /// rationale: depending on where you are executing your code, you
  ///   may or may not have access to /cvmfs where important data
  ///   files are kept.  this mechanism allows to pick it up from
  ///   various alternate places.
  std::string locate (const std::string& locations);
}

#endif
