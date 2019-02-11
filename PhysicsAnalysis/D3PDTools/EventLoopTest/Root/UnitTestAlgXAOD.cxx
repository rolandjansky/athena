/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopTest/UnitTestAlgXAOD.h>

#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <AsgTools/MessageCheck.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

#include <AsgTools/MsgStream.h>
#include <AsgTools/MsgStreamMacros.h>
#include <xAODEventInfo/EventInfo.h>

//
// method implementations
//

ClassImp (EL::UnitTestAlgXAOD)

namespace EL
{
  void UnitTestAlgXAOD ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  UnitTestAlgXAOD ::
  UnitTestAlgXAOD ()
  {
    RCU_NEW_INVARIANT (this);
  }



  UnitTestAlgXAOD ::
  ~UnitTestAlgXAOD ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_state != State::START && m_state != State::HIST_FINALIZED)
    {
      ANA_MSG_FATAL ("never got completely finalized");
      RCU_THROW_MSG ("never got completely finalized");
    }
  }



  StatusCode UnitTestAlgXAOD ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);

    job.useXAOD ();

    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  changeInput (bool /*firstFile*/)
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("changeInput");
    ANA_CHECK (testWorkerState (true, {State::HIST_INITIALIZED, State::INITIALIZED, State::INPUT_CHANGED}));
    if (m_state == State::HIST_INITIALIZED)
      m_state = State::INPUT_CHANGED;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  histInitialize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("histInitialize");
    ANA_CHECK (testWorkerState (false, {State::START}));
    m_state = State::HIST_INITIALIZED;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  initialize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("initialize");
    ANA_CHECK (testWorkerState (true, {State::INPUT_CHANGED}));
    m_state = State::INITIALIZED;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  fileExecute ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("fileExecute");
    ANA_CHECK (testWorkerState (false, {State::HIST_INITIALIZED, State::INPUT_CHANGED, State::INITIALIZED}));

    // in principle I should check here that TEvent is connected to a
    // good file, but I'm not sure how, just checking that I can
    // retrieve EventInfo, which is not guaranteed, as the input file
    // may be empty, but none of the test files is

    if (wk()->inputFile() == nullptr)
    {
      ANA_MSG_ERROR ("didn't find input file");
      return StatusCode::FAILURE;
    }

    const xAOD::EventInfo *info {nullptr};
    ANA_CHECK (evtStore()->retrieve (info, "EventInfo"));

    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  execute ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_DEBUG ("execute");
    ANA_CHECK (testWorkerState (true, {State::INITIALIZED}));
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  finalize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("finalize");
    ANA_CHECK (testWorkerState (false, {State::INITIALIZED}));
    m_state = State::FINALIZED;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  histFinalize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_CHANGE_INVARIANT (this);
    ANA_MSG_INFO ("histFinalize");
    ANA_CHECK (testWorkerState (false, {State::FINALIZED, State::HIST_FINALIZED}));
    m_state = State::HIST_FINALIZED;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlgXAOD ::
  testWorkerState (bool validEvent, std::set<State> validStates) const
  {
    if (validStates.find (m_state) == validStates.end())
    {
      ANA_MSG_ERROR ("invalid state: " << unsigned (m_state));
      return StatusCode::FAILURE;
    }

    if (wk()->xaodEvent() == nullptr)
    {
      ANA_MSG_ERROR ("didn't find TEvent");
      return StatusCode::FAILURE;
    }

    if (wk()->xaodStore() == nullptr)
    {
      ANA_MSG_ERROR ("didn't find TStore");
      return StatusCode::FAILURE;
    }

    if (validEvent)
    {
      if (wk()->tree() == nullptr)
      {
        ANA_MSG_ERROR ("didn't find tree");
        return StatusCode::FAILURE;
      }

      if (wk()->treeEntry() < 0 ||
          wk()->treeEntry() >= wk()->tree()->GetEntries())
      {
        ANA_MSG_ERROR ("invalid tree entry");
        return StatusCode::FAILURE;
      }

      const xAOD::EventInfo *info {nullptr};
      ANA_CHECK (evtStore()->retrieve (info, "EventInfo"));
    }
    return StatusCode::SUCCESS;
  }
}
