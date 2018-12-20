/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_TOOLS_JOIN_H
#define SAMPLE_HANDLER_TOOLS_JOIN_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines utility functions used for joining samples.
/// The interface provided in this module is intended for the general
/// user.  The module is considered to be in the pre-alpha stage.



//protect
#include <SampleHandler/Global.h>

#include <string>

namespace SH
{
  /// effects: remove all samples matching the name pattern, and join
  ///   them into a single sample named sampleName
  /// guarantee: strong
  /// failures: out of memory II
  /// failures: i/o errors
  void mergeSamples (SampleHandler& sh, const std::string& sampleName,
		     const std::string& pattern);
}

#endif
