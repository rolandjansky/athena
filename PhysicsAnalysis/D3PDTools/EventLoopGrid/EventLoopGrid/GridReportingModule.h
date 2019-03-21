/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_GRID__GRID_REPORTING_MODULE_H
#define EVENT_LOOP_GRID__GRID_REPORTING_MODULE_H

#include <EventLoop/Module.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module that handles the reporting from the job
    /// on the grid

    class GridReportingModule final : public Module
    {
      /// the panda error code for bad input files
    public:
      static constexpr int EC_BADINPUT = 223;

    public:
      virtual void reportInputFailure (ModuleData& data);
    };
  }
}

#endif
