/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/DiskOutput.h>

#include <RootCoreUtils/Assert.h>

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



  DiskWriter *DiskOutput ::
  makeWriter (const std::string& sample, const std::string& name,
	      int index, const std::string& suffix) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (!sample.empty() || !name.empty());
    RCU_REQUIRE (index >= -1);
    DiskWriter *const result = doMakeWriter (sample, name, index, suffix);
    RCU_PROVIDE (result != 0);
    return result;
  }



  DiskWriter *DiskOutput ::
  doMakeWriter (const std::string& /*sample*/, const std::string& /*name*/,
		int /*index*/, const std::string& /*suffix*/) const
  {
    RCU_ASSERT0_NOIMP ((std::string ("DiskOutput::doMakeWriter not overridden for ") + typeid(*this).name()).c_str());
    return 0; // compiler dummy
  }
}
