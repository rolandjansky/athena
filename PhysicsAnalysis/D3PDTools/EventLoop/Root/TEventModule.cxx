/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/TEventModule.h>

#include <memory>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/tools/TFileAccessTracer.h>
#include <xAODRootAccess/TStore.h>
// #include <xAODRootAccess/D3PDPerfStats.h>
#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaObject.h>
#include <xAODCore/tools/ReadStats.h>
#include <xAODCore/tools/PerfStats.h>
#include <xAODCore/tools/IOStats.h>
#include <AsgTools/MessageCheck.h>
#include <TList.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    TEventModule ::
    TEventModule ()
    {}



    TEventModule ::
    ~TEventModule ()
    {}



    ::StatusCode TEventModule ::
    onInitialize (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (m_event != nullptr || m_store != nullptr)
      {
        ANA_MSG_ERROR ("module initialized twice");
        return ::StatusCode::FAILURE;
      }
      if (data.m_tevent != nullptr || data.m_tstore != nullptr)
      {
        ANA_MSG_ERROR ("duplicate TEventModule??");
        return ::StatusCode::FAILURE;
      }

      std::string modeStr = data.m_metaData->castString
        (Job::optXaodAccessMode);
      if (!modeStr.empty())
      {
        xAOD::TEvent::EAuxMode mode = xAOD::TEvent::kClassAccess; //compiler dummy
        if (modeStr == Job::optXaodAccessMode_class)
          mode = xAOD::TEvent::kClassAccess;
        else if (modeStr == Job::optXaodAccessMode_branch)
          mode = xAOD::TEvent::kBranchAccess;
        else if (modeStr == Job::optXaodAccessMode_athena)
          mode = xAOD::TEvent::kAthenaAccess;
        else
        {
          ANA_MSG_ERROR ("unknown XAOD access mode: " << modeStr);
          return ::StatusCode::FAILURE;
        }
        m_event.reset (new xAOD::TEvent (mode));
      } else
      {
        m_event.reset (new xAOD::TEvent);
      }
      if (data.m_metaData->castDouble (Job::optXAODSummaryReport, 1) == 0)
        xAOD::TFileAccessTracer::enableDataSubmission (false);

      m_store.reset (new xAOD::TStore);

      m_useStats = data.m_metaData->castBool (Job::optXAODPerfStats, false);
      if (m_useStats)
        xAOD::PerfStats::instance().start();

      data.m_tevent = m_event.get();
      data.m_tstore = m_store.get();

      return StatusCode::SUCCESS;
    }



    ::StatusCode TEventModule ::
    postFinalize (ModuleData& data)
    {
      if (m_useStats)
      {
        xAOD::PerfStats::instance().stop();
        std::unique_ptr<xAOD::ReadStats> stats
          (new xAOD::ReadStats (xAOD::IOStats::instance().stats()));
        stats->SetName (Job::optXAODReadStats.c_str());
        stats->Print ();
        data.addOutput (std::move (stats));
      }
      data.m_tevent = nullptr;
      data.m_tstore = nullptr;
      m_event.reset ();
      m_store.reset ();
      return StatusCode::SUCCESS;
    }



    ::StatusCode TEventModule ::
    onNewInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (m_event == nullptr || m_store == nullptr)
      {
        ANA_MSG_ERROR ("module not inititalized");
        return ::StatusCode::FAILURE;
      }
      ANA_CHECK (m_event->readFrom (data.m_inputFile.get()));
      m_store->clear ();
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode TEventModule ::
    postCloseInputFile (ModuleData& /*data*/)
    {
      using namespace msgEventLoop;
      if (m_event == nullptr || m_store == nullptr)
      {
        ANA_MSG_ERROR ("module not inititalized");
        return ::StatusCode::FAILURE;
      }
      ANA_CHECK (m_event->readFrom ((TFile *)nullptr));
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode TEventModule ::
    onExecute (ModuleData& data)
    {
      using namespace msgEventLoop;
      m_store->clear ();
      if (m_event->getEntry (data.m_inputTreeEntry) < 0)
        RCU_THROW_MSG ("failed to read from xAOD");
      return StatusCode::SUCCESS;
    }
  }
}
