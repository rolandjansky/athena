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

#include <EventLoop/BatchSample.h>

//
// method implementations
//

ClassImp(EL::BatchSample)

namespace EL
{
  BatchSample ::
  BatchSample ()
    : begin_segments (0), end_segments (0)
  {
  }



  BatchSample ::
  ~BatchSample ()
  {
  }
}
