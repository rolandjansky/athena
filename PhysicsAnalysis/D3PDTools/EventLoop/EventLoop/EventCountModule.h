/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__EVENT_COUNT_MODULE_H
#define EVENT_LOOP__EVENT_COUNT_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module tracking how many events get processed by
    /// each algorithm

    class EventCountModule final : public Module
    {
      //
      // public interface
      //

    public:
      virtual ::StatusCode postFinalize (ModuleData& data) override;
    };
  }
}

#endif
