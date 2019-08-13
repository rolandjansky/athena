/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/BaseManager.h>

#include <AsgTools/StatusCode.h>
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
    Detail::ManagerOrder BaseManager ::
    getManagerOrder () const noexcept
    {
      return ManagerOrder::BASE;
    }



    ::StatusCode BaseManager ::
    doManagerStep (Detail::ManagerData& data) const
    {
      switch (data.step)
      {
      case Detail::ManagerStep::updateSubmitDir:
        {
          if (data.submitDir[0] != '/')
            data.submitDir = gSystem->WorkingDirectory () + ("/" + data.submitDir);
          if (data.submitDir.find ("/pnfs/") == 0)
          {
            ANA_MSG_ERROR ("can not place submit directory on pnfs: " + data.submitDir);
            return ::StatusCode::FAILURE;
          }
        }
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
