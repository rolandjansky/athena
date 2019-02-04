//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
//
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/MessageCheck.h>

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
  ANA_MSG_SOURCE (msgProperty, "AsgTools_Property")
  ANA_MSG_SOURCE (msgToolHandle, "AsgTools_AnaToolHandle")
  ANA_MSG_SOURCE (msgUserCode, "AsgTools_UserCode")

  namespace detail
  {
#ifndef XAOD_STANDALONE
    // Get the Athena message service
    IMessageSvc* getMessageSvcAthena()
    {
      static IMessageSvc* msgSvc
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
