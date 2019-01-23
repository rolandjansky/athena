/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoopGrid/GridReportingModule.h>

#include <EventLoop/MessageCheck.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    void GridReportingModule ::
    reportInputFailure (ModuleData& /*data*/)
    {
      using namespace msgEventLoop;

      ANA_MSG_FATAL ("encountered input error");
      exit (EC_BADINPUT);
    }
  }
}
