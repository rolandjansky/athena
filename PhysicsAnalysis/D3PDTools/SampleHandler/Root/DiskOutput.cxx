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
