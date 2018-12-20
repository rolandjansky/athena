/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__TOOLS_DUPLICATES_H
#define SAMPLE_HANDLER__TOOLS_DUPLICATES_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <SampleHandler/Global.h>

namespace SH
{
  /// effects: check the given sample for duplicate events and then
  ///   print them out
  /// guarantee: basic, may print partially
  /// failures: out of memory III
  /// failures: i/o errors
  void printDuplicateEvents (const Sample& sample);


  /// effects: check each sample for duplicate events and then
  ///   print them out
  /// guarantee: basic, may print partially
  /// failures: out of memory III
  /// failures: i/o errors
  void printDuplicateEventsSplit (const SampleHandler& sh);


  /// effects: check for duplicate events between all the samples and
  ///   then print them out
  /// guarantee: basic, may print partially
  /// failures: out of memory III
  /// failures: i/o errors
  void printDuplicateEventsJoint (const SampleHandler& sh);
}

#endif
