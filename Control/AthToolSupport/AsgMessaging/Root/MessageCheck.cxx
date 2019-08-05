/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgMessaging/MessageCheck.h>

#include <stdexcept>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/Bootstrap.h>
#include <GaudiKernel/ISvcLocator.h>
#endif

//
// method implementations
//

namespace asg
{
  ANA_MSG_SOURCE (msgUserCode, "AsgMessaging_UserCode")

  namespace detail
  {
#ifndef XAOD_STANDALONE
    // Get the Athena message service
    IMessageSvc* getMessageSvcAthena()
    {
      static IMessageSvc* const msgSvc
	= Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc");
      return msgSvc;
    }
#endif


    void throw_check_fail (const std::string& str)
    {
      throw std::runtime_error (str);
    }
  }
}
