/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__DRIVER_MANAGER_H
#define EVENT_LOOP__DRIVER_MANAGER_H

#include <EventLoop/Global.h>

#include <EventLoop/Manager.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Manager that passes callbacks through to the
    /// \ref Driver
    ///
    /// Originally I was thinking of having the `Driver` class derive
    /// from the `Manager` class, but that gave me dictionary errors I
    /// was unable to resolve.  So now I have this class that passes
    /// callbacks straight through.  There are three ways to go from
    /// here in the future:
    ///
    /// * Move all the functionality from the driver classes into
    ///   separate manager classes, and reduce the driver
    ///   implementations to installing the managers as required.
    ///
    /// * Resolve the dictionary issues, and derive the drivers from
    ///   the manager class.
    ///
    /// * Decide that this is actually a reasonably nice solution, and
    ///   stick with it.

    class DriverManager final : public Manager
    {
    public:
      virtual std::pair<Detail::ManagerOrder,std::string>
      getManagerOrder () const noexcept override;

    public:
      virtual ::StatusCode
      doManagerStep (Detail::ManagerData& data) const override;
    };
  }
}

#endif
