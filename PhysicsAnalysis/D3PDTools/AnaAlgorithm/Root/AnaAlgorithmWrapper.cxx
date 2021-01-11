/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithmWrapper.h>

#include <AnaAlgorithm/MessageCheck.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void AnaAlgorithmWrapper ::
  testInvariant () const
  {
    RCU_INVARIANT (!m_config.name().empty());
  }



  AnaAlgorithmWrapper ::
  AnaAlgorithmWrapper (AnaAlgorithmConfig val_config)
    : m_config (std::move (val_config))
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string_view AnaAlgorithmWrapper ::
  getName () const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name();
  }



  bool AnaAlgorithmWrapper ::
  hasName (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name() == name;
  }



  std::unique_ptr<IAlgorithmWrapper> AnaAlgorithmWrapper ::
  makeClone() const
  {
    RCU_READ_INVARIANT (this);
    return std::make_unique<AnaAlgorithmWrapper> (m_config);
  }



  StatusCode AnaAlgorithmWrapper ::
  initialize (const AlgorithmWorkerData& workerData)
  {
    using namespace msgAlgorithmConfig;
    RCU_CHANGE_INVARIANT (this);
    if (m_config.makeAlgorithm (m_algorithm, workerData).isFailure())
    {
      ANA_MSG_ERROR ("failed to create AnaAlgorithm: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  execute ()
  {
    using namespace msgAlgorithmConfig;
    RCU_READ_INVARIANT (this);
    if (m_algorithm->sysExecute().isFailure())
    {
      ANA_MSG_ERROR ("failed to call execute() on algorithm: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  finalize ()
  {
    using namespace msgAlgorithmConfig;
    RCU_READ_INVARIANT (this);
    if (m_algorithm->sysFinalize().isFailure())
    {
      ANA_MSG_ERROR ("failed to call finalize() on algorithm: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  fileExecute ()
  {
    using namespace msgAlgorithmConfig;
    RCU_READ_INVARIANT (this);
    if (m_algorithm->hasFileExecute())
    {
      if (m_algorithm->sysFileExecute().isFailure())
      {
        ANA_MSG_ERROR ("failed to call fileExecute() on algorithm: " << m_config.name());
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  beginInputFile ()
  {
    using namespace msgAlgorithmConfig;
    RCU_READ_INVARIANT (this);
    if (m_algorithm->hasBeginInputFile())
    {
      if (m_algorithm->sysBeginInputFile().isFailure())
      {
        ANA_MSG_ERROR ("failed to call beginInputFile() on algorithm: " << m_config.name());
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaAlgorithmWrapper ::
  endInputFile ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }
}
