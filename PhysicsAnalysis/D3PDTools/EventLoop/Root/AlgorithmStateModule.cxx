/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AlgorithmStateModule.h>

#include <AnaAlgorithm/AlgorithmWorkerData.h>
#include <AnaAlgorithm/IAlgorithmWrapper.h>
#include <AsgTools/SgTEvent.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/ModuleData.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>
#include <TTree.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    namespace
    {
      template<typename F> ::StatusCode
      forAllAlgorithms (ModuleData& data, const char *funcName, F&& func)
      {
        using namespace msgEventLoop;
        for (AlgorithmData& alg : data.m_algs)
        {
          try
          {
            typedef typename std::decay<decltype(func(alg))>::type scType__;
            if (!::asg::CheckHelper<scType__>::isSuccess (func (alg)))
            {
              ANA_MSG_ERROR ("executing " << funcName << " on algorithm " << alg->getName());
              return StatusCode::FAILURE;
            }
          } catch (...)
          {
            report_exception ();
            ANA_MSG_ERROR ("executing " << funcName << " on algorithm " << alg->getName());
            return StatusCode::FAILURE;
          }
        }
        return StatusCode::SUCCESS;
      }
    }



    ::StatusCode AlgorithmStateModule ::
    onInitialize (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (m_initialized)
      {
        ANA_MSG_ERROR ("getting second initialize call");
        return ::StatusCode::FAILURE;
      }
      m_initialized = true;
      AlgorithmWorkerData workerData;
      workerData.m_histogramWorker = data.m_worker;
      workerData.m_treeWorker = data.m_worker;
      workerData.m_filterWorker = data.m_worker;
      workerData.m_wk = data.m_worker;
      workerData.m_evtStore = data.m_evtStore;
      return forAllAlgorithms (data, "initialize", [&] (AlgorithmData& alg) {
        return alg->initialize (workerData);});
    }



    ::StatusCode AlgorithmStateModule ::
    onFinalize (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (!m_initialized)
        return ::StatusCode::SUCCESS;
      if (forAllAlgorithms (data, "finalize", [&] (AlgorithmData& alg) {
            return alg->finalize ();}).isFailure())
        return StatusCode::FAILURE;
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode AlgorithmStateModule ::
    onCloseInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      return forAllAlgorithms (data, "endInputFile", [&] (AlgorithmData& alg) {
          return alg->endInputFile ();});
    }



    ::StatusCode AlgorithmStateModule ::
    onNewInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (!m_initialized)
      {
        ANA_MSG_ERROR ("algorithms have not been initialized yet");
        return ::StatusCode::FAILURE;
      }

      if (data.m_inputTree == nullptr ||
          data.m_inputTree->GetEntries() == 0)
        return ::StatusCode::SUCCESS;

      if (forAllAlgorithms (data, "changeInput", [&] (AlgorithmData& alg) {
            return alg->beginInputFile ();}).isFailure())
        return ::StatusCode::FAILURE;
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode AlgorithmStateModule ::
    onFileExecute (ModuleData& data)
    {
      using namespace msgEventLoop;
      return forAllAlgorithms (data, "fileExecute", [&] (AlgorithmData& alg) {
          return alg->fileExecute ();});
    }
  }
}
