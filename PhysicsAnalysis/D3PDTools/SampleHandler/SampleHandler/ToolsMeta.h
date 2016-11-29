/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__TOOLS_META_H
#define SAMPLE_HANDLER__TOOLS_META_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <SampleHandler/Global.h>

#include <string>

namespace SH
{
  /// effects: read the susy-meta-data file and add its information to
  ///   the samples from the given sample handler
  /// guarantee: basic
  /// failures: i/o errors
  /// warning: there are no checks for duplicate and inconsistent
  ///   sample definitions
  void readSusyMeta (const SampleHandler& sh, const std::string& inputFile);

  /// effects: read an entire directory of susy-meta-data files and
  ///   add their information to the samples from the given sample
  ///   handle.  for that it assumes that all files ending in ".txt"
  ///   are susy-meta-data files.
  /// guarantee: basic
  /// failures: i/o errors
  /// warning: there are no checks for duplicate and inconsistent
  ///   sample definitions
  void readSusyMetaDir (const SampleHandler& sh, const std::string& inputDir);
}

#endif
