/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/DiskOutputXRD.h>

#include <sstream>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskWriterXRD.h>

//
// method implementations
//

ClassImp (SH::DiskOutputXRD)

namespace SH
{
  void DiskOutputXRD :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    RCU_INVARIANT (m_prefix.find ("root://") == 0);
  }



  DiskOutputXRD :: 
  DiskOutputXRD ()
    : m_prefix ("root://ThisIsANonExistantPath")
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskOutputXRD :: 
  DiskOutputXRD (const std::string& val_prefix)
    : m_prefix (val_prefix)
  {
    RCU_REQUIRE (val_prefix.find ("root://") == 0);

    RCU_NEW_INVARIANT (this);
  }



  DiskWriter *DiskOutputXRD ::
  doMakeWriter (const std::string& sample, const std::string& name,
		int index, const std::string& suffix) const
  {
    RCU_READ_INVARIANT (this);
    std::ostringstream file;
    file << m_prefix << "/";
    file << sample;
    if (!sample.empty() && !name.empty())
      file << "-";
    file << name;
    if (index >= 0)
      file << "-" << index;
    file << suffix;
    return new DiskWriterXRD (file.str());
  }
}
