/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/DriverManager.h>

#include <AsgTools/StatusCode.h>
#include <EventLoop/Driver.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerOrder.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    Detail::ManagerOrder DriverManager ::
    getManagerOrder () const noexcept
    {
      return ManagerOrder::DRIVER;
    }



    ::StatusCode DriverManager ::
    doManagerStep (Detail::ManagerData& data) const
    {
      return data.driver->doManagerStep (data);
    }
  }
}
