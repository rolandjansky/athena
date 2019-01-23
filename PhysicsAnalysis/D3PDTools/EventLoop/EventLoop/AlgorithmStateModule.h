/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__ALGORITHM_STATE_MODULE_H
#define EVENT_LOOP__ALGORITHM_STATE_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module managing the state of the algorithms on
    /// the workers
    ///
    /// \warn This module ought to be the last module loaded.  That
    /// ensures that the algorithms get to do their thing after the
    /// modules.  If a module needs to act after the algorithms, a new
    /// callback should be added for that.
    ///
    /// The algorithms require actually a fairly complicated state
    /// machine to maintain.  Mostly because the functions of the \ref
    /// Algorithm class have to be called in a specific order, and
    /// each needs to have a specific set of pre-conditions fulfilled.
    /// This logic used to be (before 19 Jan 19) in the \ref Worker
    /// class itself, but I decided it is better to factor it out into
    /// a separate module.

    struct AlgorithmStateModule final : public Module
    {
      //
      // public interface
      //

    public:
      virtual ::StatusCode onInitialize (ModuleData& data) override;

    public:
      virtual ::StatusCode onFinalize (ModuleData& data) override;

    public:
      virtual ::StatusCode onCloseInputFile (ModuleData& data) override;

    public:
      virtual ::StatusCode onNewInputFile (ModuleData& data) override;

    public:
      virtual ::StatusCode onFileExecute (ModuleData& data) override;



      //
      // private interface
      //

      /// \brief whether \ref Algorithm::histInitialize has been called
    private:
      bool m_histInitialized {false};

      /// \brief whether \ref Algorithm::initialize has been called
    private:
      bool m_initialized {false};

      /// \brief whether we are still waiting to connect to the first file
    private:
      bool m_firstFile {true};
    };
  }
}

#endif
