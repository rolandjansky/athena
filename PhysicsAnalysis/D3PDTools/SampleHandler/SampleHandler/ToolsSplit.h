/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_TOOLS_SPLIT_H
#define SAMPLE_HANDLER_TOOLS_SPLIT_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines utility functions used for splitting samples.
/// The interface provided in this module is intended for the general
/// user.  The module is considered to be in the pre-alpha stage.



//protect
#include <SampleHandler/Global.h>

#include <Rtypes.h>

namespace SH
{
  /// effects: scan each sample in the sample handler and store the
  ///   number of entries per file in the meta-data
  /// guarantee: basic, may only scan some
  /// failures: out of memory
  /// failures: read errors
  /// failures: invalid sample type
  void scanNEvents (SampleHandler& sh);


  /// effects: scan the given sample and store the number of entries
  ///   per file in the meta-data
  /// guarantee: strong
  /// failures: out of memory
  /// failures: read errors
  /// failures: invalid sample type
  void scanNEvents (Sample& sample);


  /// effects: split the given sample into a set of samples, with each
  ///   sample containing either exactly one file or at most nevt
  ///   events
  /// side effects: if scanNEvents hasn't been run on this sample, run
  ///   it.
  /// guarantee: strong
  /// failures: out of memory
  /// failures: scanning errors
  SampleHandler splitSample (Sample& sample, Long64_t nevt);
}

#endif
