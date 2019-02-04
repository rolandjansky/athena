/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2013.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/DiskOutput.h>

#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskWriter.h>

//
// method implementations
//

ClassImp (SH::DiskOutput)

namespace SH
{
  void DiskOutput :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DiskOutput :: 
  DiskOutput ()
  {
    RCU_NEW_INVARIANT (this);
  }



  std::unique_ptr<DiskWriter> DiskOutput ::
  makeWriter (const std::string& sampleName,
              const std::string& segmentName,
              const std::string& suffix) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (!sampleName.empty());
    std::unique_ptr<DiskWriter> result
      = doMakeWriter (sampleName, segmentName, suffix);
    RCU_PROVIDE (result != nullptr);
    return result;
  }
}
