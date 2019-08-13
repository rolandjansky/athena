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

    enum class ManagerOrder
    {
      /// \brief the most basic operations, common for all tasks
      BASE,

      /// \brief the base-operation performed (e.g. submit, retrieve)
      OPERATION,

      /// \brief the driver itself
      DRIVER
    };
  }
}

#endif
