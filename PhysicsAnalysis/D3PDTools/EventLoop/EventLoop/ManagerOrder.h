/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MANAGER_ORDER_H
#define EVENT_LOOP__MANAGER_ORDER_H

#include <EventLoop/Global.h>

namespace EL
{
  namespace Detail
  {
    /// \brief an enum to establish an order in which to apply managers
    ///
    /// While there should be very little inter-dependency between
    /// managers, it can matter which order they get applied in,
    /// e.g. the basic manager may set the default value of some
    /// property only to be overridden by a more specialized manager
    /// downstream.

    enum class ManagerOrder
    {
      /// \brief the most basic operations, common for all tasks
      BASE,

      /// \brief the basic operations for the submission directory
      SUBMIT_DIR,

      /// \brief the base-operation performed (e.g. submit, retrieve)
      OPERATION,

      /// \brief the driver itself
      DRIVER
    };
  }
}

#endif
