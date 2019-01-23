/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__TREE_CACHE_MODULE_H
#define EVENT_LOOP__TREE_CACHE_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module for setting the TTreeCache parameters on
    /// the input

    class TreeCacheModule : public Module
    {
      //
      // public interface
      //

    public:
      virtual ::StatusCode onNewInputFile (ModuleData& data) override;

    public:
      virtual ::StatusCode onCloseInputFile (ModuleData& data) override;
    };
  }
}

#endif
