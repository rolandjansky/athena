/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/BaseManager.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerOrder.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <TSystem.h>

using namespace EL::msgEventLoop;

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    std::pair<Detail::ManagerOrder,std::string> BaseManager ::
    getManagerOrder () const noexcept
    {
      return std::make_pair (ManagerOrder::BASE, "");
    }



    ::StatusCode BaseManager ::
    doManagerStep (Detail::ManagerData& data) const
    {
      switch (data.step)
      {
      case Detail::ManagerStep::initial:
        // normally this should get overriden by the operation
        // manager, but this is a safety fallback in case the
        // operation doesn't do that, leaving us with nothing
        // happening
        data.nextStep = ManagerStep::final;
        break;

      case Detail::ManagerStep::final:
        ANA_MSG_INFO ("done");
        break;

      default:
        break;
      }
      return ::StatusCode::SUCCESS;
    }
  }
}
