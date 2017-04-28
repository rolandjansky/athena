/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/MetaNames.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace SH
{
  const std::string& MetaNames ::
  openRetries ()
  {
    static const std::string result ("nc_open_retries");
    return result;
  }



  unsigned MetaNames ::
  openRetries_default ()
  {
    return 0;
  }



  const std::string& MetaNames ::
  openRetriesWait ()
  {
    static const std::string result ("nc_open_retries_wait");
    return result;
  }



  double MetaNames ::
  openRetriesWait_default ()
  {
    return 300;
  }



  const std::string& MetaNames ::
  sampleName ()
  {
    static const std::string result ("sample_name");
    return result;
  }
}
