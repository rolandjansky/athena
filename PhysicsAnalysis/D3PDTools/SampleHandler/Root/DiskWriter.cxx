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
