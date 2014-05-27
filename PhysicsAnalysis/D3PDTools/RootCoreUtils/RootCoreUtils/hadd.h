#ifndef ROOT_CORE_UTILS__HADD_H
#define ROOT_CORE_UTILS__HADD_H

//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


#include <RootCoreUtils/Global.h>

#include <string>
#include <vector>

namespace RCU
{
  /// effects: perform the hadd functionality
  /// guarantee: basic
  /// failures: out of memory III
  /// failures: i/o errors
  void hadd (std::string output_file,
	     std::vector<std::string> input_files,
	     unsigned max_files = 0);


  /// effects: perform the core hadd functionality
  /// guarantee: basic
  /// failures: out of memory III
  /// failures: i/o errors
  void hadd_core (std::string output_file,
		  std::vector<std::string> input_files,
		  unsigned max_files);
}

#endif
