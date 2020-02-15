/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AlgorithmStateModule.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/ModuleData.h>
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
              ANA_MSG_ERROR ("executing " << funcName << " on algorithm " << alg->GetName());
              return StatusCode::FAILURE;
            }
          } catch (...)
          {
            report_exception ();
            ANA_MSG_ERROR ("executing " << funcName << " on algorithm " << alg->GetName());
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
      if (m_histInitialized)
      {
        ANA_MSG_ERROR ("getting second initialize call");
        return ::StatusCode::FAILURE;
      }
      m_histInitialized = true;
      return forAllAlgorithms (data, "histInitialize", [&] (AlgorithmData& alg) {
          return alg->histInitialize ();});
    }



    ::StatusCode AlgorithmStateModule ::
    onFinalize (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (m_initialized)
      {
        if (forAllAlgorithms (data, "finalize", [&] (AlgorithmData& alg) {
              return alg->finalize ();}).isFailure())
          return StatusCode::FAILURE;
      }
      if (!m_histInitialized)
        return ::StatusCode::SUCCESS;
      return forAllAlgorithms (data, "histFinalize", [&] (AlgorithmData& alg) {
          return alg->histFinalize ();});
    }



    ::StatusCode AlgorithmStateModule ::
    onCloseInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      return forAllAlgorithms (data, "endOfFile", [&] (AlgorithmData& alg) {
          return alg->endOfFile ();});
    }



    ::StatusCode AlgorithmStateModule ::
    onNewInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (!m_histInitialized)
      {
        ANA_MSG_ERROR ("algorithms have not been initialized yet");
        return ::StatusCode::FAILURE;
      }

      if (data.m_inputTree == nullptr ||
          data.m_inputTree->GetEntries() == 0)
        return ::StatusCode::SUCCESS;

      const bool firstFile = m_firstFile;
      m_firstFile = false;
      if (forAllAlgorithms (data, "changeInput", [&] (AlgorithmData& alg) {
            return alg->changeInput (firstFile);}).isFailure())
        return ::StatusCode::FAILURE;

      if (m_initialized == true)
        return ::StatusCode::SUCCESS;
      m_initialized = true;
      return forAllAlgorithms (data, "initialize", [&] (AlgorithmData& alg) {
          return alg->initialize ();});
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
