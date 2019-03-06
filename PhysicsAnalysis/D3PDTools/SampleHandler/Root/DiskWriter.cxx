/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/DiskWriter.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace SH
{
  void DiskWriter :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DiskWriter :: 
  DiskWriter ()
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskWriter ::
  ~DiskWriter ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  std::string DiskWriter ::
  path () const
  {
    RCU_READ_INVARIANT (this);
    return getPath ();
  }



  TFile *DiskWriter ::
  file ()
  {
    RCU_CHANGE_INVARIANT (this);

    TFile *result = getFile ();
    RCU_REQUIRE (result != 0);
    return result;
  }



  void DiskWriter ::
  close ()
  {
    RCU_CHANGE_INVARIANT (this);

    doClose ();
  }
}
