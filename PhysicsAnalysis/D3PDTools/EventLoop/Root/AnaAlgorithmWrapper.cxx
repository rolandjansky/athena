/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AnaAlgorithmWrapper.h>

#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

ClassImp (EL::AnaAlgorithmWrapper)

namespace EL
{
  void AnaAlgorithmWrapper :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  AnaAlgorithmWrapper :: 
  AnaAlgorithmWrapper (const AnaAlgorithmConfig& val_config)
    : m_config (val_config)
  {
    SetName (m_config.name().c_str());

    RCU_NEW_INVARIANT (this);
  }



  StatusCode AnaAlgorithmWrapper ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    if (m_config.useXAODs())
      job.useXAOD ();
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  histInitialize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_config.makeAlgorithm (m_algorithm));
    m_algorithm->setHistogramWorker (wk ());
    m_algorithm->setTreeWorker (wk ());
    m_algorithm->setFilterWorker (wk ());
    m_algorithm->setWk (wk ());
    if (m_config.useXAODs())
      m_algorithm->setEvtStore (evtStore ());
    ANA_CHECK (m_algorithm->sysInitialize());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  initialize ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  fileExecute ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    if (m_algorithm->hasFileExecute())
      ANA_CHECK (m_algorithm->sysFileExecute());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  changeInput (bool)
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    if (m_algorithm->hasBeginInputFile())
      ANA_CHECK (m_algorithm->sysBeginInputFile());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  execute ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_algorithm->sysExecute());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  histFinalize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_algorithm->sysFinalize());
    return StatusCode::SUCCESS;
  }
}
