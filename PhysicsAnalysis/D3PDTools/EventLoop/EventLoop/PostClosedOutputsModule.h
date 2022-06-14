/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Romain Bouquet



#ifndef EVENT_LOOP__POSTCLOSEDOUTPUTS_MODULE_H
#define EVENT_LOOP__POSTCLOSEDOUTPUTS_MODULE_H

#include <EventLoop/Global.h>
#include <EventLoop/Module.h>

namespace EL
{
  namespace Detail
  {
    /// \brief Module enabling to perform checks/actions on outputs once files are all closed

    class PostClosedOutputsModule final : public Module
    {
      //
      // public interfacce
      //
    public:
      virtual ::StatusCode postFileClose   (ModuleData& data) override;
    };
  }
}

#endif