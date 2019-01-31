/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__STOPWATCH_MODULE_H
#define EVENT_LOOP__STOPWATCH_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>
#include <TH1.h>
#include <TStopwatch.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module wrapping the \ref TStopwatch class

    class StopwatchModule final : public Module
    {
      //
      // public interfacce
      //

    public:
      virtual ::StatusCode preInitialize (ModuleData& data) override;

    public:
      virtual ::StatusCode onFileExecute (ModuleData& data) override;

    public:
      virtual ::StatusCode postFinalize (ModuleData& data) override;

    public:
      virtual ::StatusCode onWorkerEnd (ModuleData& data) override;

      //
      // private interface
      //

      /// \brief the run time summary histogram
    private:
      std::unique_ptr<TH1> m_runTime {nullptr}; //!

      /// \brief the stop watch we use for measuring total time spend
    private:
      std::unique_ptr<TStopwatch> m_stopwatch; //!
    };
  }
}

#endif
