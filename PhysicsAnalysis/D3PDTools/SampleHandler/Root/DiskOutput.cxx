/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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



  std::string DiskOutput ::
  targetURL (const std::string& sampleName,
             const std::string& segmentName,
             const std::string& suffix) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (!sampleName.empty());
    std::string result = getTargetURL (sampleName, segmentName, suffix);
    RCU_PROVIDE (!result.empty());
    return result;
  }
}
