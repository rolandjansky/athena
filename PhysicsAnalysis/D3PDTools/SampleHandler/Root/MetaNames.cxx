/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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
