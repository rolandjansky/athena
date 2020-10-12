/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/RetrieveManager.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Driver.h>
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
    std::pair<Detail::ManagerOrder,std::string> RetrieveManager ::
    getManagerOrder () const noexcept
    {
      return std::make_pair (ManagerOrder::OPERATION, "");
    }



    ::StatusCode RetrieveManager ::
    doManagerStep (Detail::ManagerData& data) const
    {
      switch (data.step)
      {
      case Detail::ManagerStep::initial:
        {
          data.nextStep = Detail::ManagerStep::initialRetrieve;
        }
        break;

      case Detail::ManagerStep::initialRetrieve:
        {
          if (gSystem->AccessPathName ((data.submitDir + "/submitted").c_str()) != 0)
          {
            ANA_MSG_ERROR ("job submission was unsuccessful");
            return ::StatusCode::FAILURE;
          }
        }
        break;

      case Detail::ManagerStep::finalRetrieve:
        {
          if (!data.retrieved)
          {
            ANA_MSG_FATAL ("Driver::retrieve not implemented in class " << typeid(*data.driver).name());
            std::abort ();
          }

          data.nextStep = Detail::ManagerStep::final;
        }
        break;

      default:
        (void) true; // safe to do nothing
      }
      return ::StatusCode::SUCCESS;
    }
  }
}
