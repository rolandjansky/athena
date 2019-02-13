/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef EVENT_LOOP_TEVENT_SVC_H
#define EVENT_LOOP_TEVENT_SVC_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>
#include <memory>

namespace xAOD
{
  class TEvent;
  class TStore;
}

namespace EL
{
  namespace Detail
  {
    class TEventModule : public Module
    {
      //
      // public interface
      //

      /// effects: standard constructor.
      /// guarantee: no-fail
    public:
      TEventModule ();


      /// effects: standard destructor.
      /// guarantee: no-fail
    public:
      ~TEventModule ();



      //
      // interface inherited from Algorithm
      //

    public:
      virtual ::StatusCode onInitialize (ModuleData& data) override;

    public:
      virtual ::StatusCode postFinalize (ModuleData& data) override;

    public:
      virtual ::StatusCode onNewInputFile (ModuleData& data) override;

    public:
      virtual ::StatusCode postCloseInputFile (ModuleData& data) override;

    public:
      virtual ::StatusCode onExecute (ModuleData& data) override;



      //
      // private interface
      //

      /// description: the event structure used
    private:
      std::unique_ptr<xAOD::TEvent> m_event; //!
    private:
      std::unique_ptr<xAOD::TStore> m_store; //!

      /// description: whether we collect D3PDPerfStats statistics
    private:
      bool m_useStats; //!
    };
  }
}

#endif
