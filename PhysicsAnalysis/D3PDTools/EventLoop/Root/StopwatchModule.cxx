/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/StopwatchModule.h>

#include <EventLoop/ModuleData.h>
#include <RootCoreUtils/Assert.h>
#include <TList.h>
#include <TStopwatch.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode StopwatchModule ::
    preInitialize (ModuleData& /*data*/)
    {
      m_stopwatch = std::make_unique<TStopwatch> ();

      m_stopwatch->Start ();

      m_runTime = std::make_unique<TH1D> ("EventLoop_RunTime", "worker run-time summary", 5, 0, 5);
      m_runTime->SetDirectory (nullptr);
      m_runTime->Fill (0);
      m_runTime->GetXaxis()->SetBinLabel (1, "jobs");
      m_runTime->GetXaxis()->SetBinLabel (2, "files");
      m_runTime->GetXaxis()->SetBinLabel (3, "events");
      m_runTime->GetXaxis()->SetBinLabel (4, "cpu time");
      m_runTime->GetXaxis()->SetBinLabel (5, "real time");
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode StopwatchModule ::
    onFileExecute (ModuleData& /*data*/)
    {
      m_runTime->Fill (1);
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode StopwatchModule ::
    postFinalize (ModuleData& data)
    {
      m_stopwatch->Stop ();
      m_runTime->Fill (2, data.m_eventsProcessed);
      m_runTime->Fill (3, m_stopwatch->CpuTime());
      m_runTime->Fill (4, m_stopwatch->RealTime());
      m_stopwatch->Continue ();
      data.m_output->Add (m_runTime.release());
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode StopwatchModule ::
    onWorkerEnd (ModuleData& /*data*/)
    {
      m_stopwatch->Stop ();
      m_stopwatch->Print ();
      return ::StatusCode::SUCCESS;
    }
  }
}
