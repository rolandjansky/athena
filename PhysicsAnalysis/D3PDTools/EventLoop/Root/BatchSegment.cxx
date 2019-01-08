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

#include <EventLoop/BatchSegment.h>

//
// method implementations
//

ClassImp(EL::BatchSegment)

namespace EL
{
  BatchSegment ::
  BatchSegment ()
    : sample (0), job_id (0),
      begin_file (0), begin_event (0),
      end_file (0), end_event (0)
  {
  }



  BatchSegment ::
  ~BatchSegment ()
  {
  }
}
