/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/BatchJob.h>

#include <TChain.h>
#include <EventLoop/Algorithm.h>
#include <EventLoop/BatchDriver.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/OutputStream.h>

//
// method implementations
//

ClassImp(EL::BatchJob)

namespace EL
{
  BatchJob ::
  BatchJob ()
  {
  }



  BatchJob ::
  ~BatchJob ()
  {
  }
}
