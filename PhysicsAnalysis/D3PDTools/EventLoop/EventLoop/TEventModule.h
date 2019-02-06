/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_TEVENT_SVC_H
#define EVENT_LOOP_TEVENT_SVC_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the services interfacing with TEvent.
/// The interface provided in this module is intended for the general
/// user.  The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <EventLoop/Module.h>

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
      virtual ::StatusCode onCloseInputFile (ModuleData& data) override;

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
