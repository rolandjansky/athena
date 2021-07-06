/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaReentrantAlgorithmWrapper.h>

#include <AnaAlgorithm/MessageCheck.h>
#include <AsgTools/CurrentContext.h>
#include "RootCoreUtils/Assert.h"



//
// method implementations
//

namespace EL
{
  void AnaReentrantAlgorithmWrapper ::
  testInvariant () const
  {
    RCU_INVARIANT (!m_config.name().empty());
  }



  AnaReentrantAlgorithmWrapper ::
  AnaReentrantAlgorithmWrapper (AnaReentrantAlgorithmConfig val_config)
    : m_config (std::move (val_config))
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string_view AnaReentrantAlgorithmWrapper ::
  getName () const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name();
  }



  bool AnaReentrantAlgorithmWrapper ::
  hasName (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name() == name;
  }



  std::unique_ptr<IAlgorithmWrapper> AnaReentrantAlgorithmWrapper ::
  makeClone() const
  {
    RCU_READ_INVARIANT (this);
    return std::make_unique<AnaReentrantAlgorithmWrapper> (m_config);
  }



  StatusCode AnaReentrantAlgorithmWrapper ::
  initialize (const AlgorithmWorkerData& workerData)
  {
    using namespace msgAlgorithmConfig;
    RCU_CHANGE_INVARIANT (this);
    if (m_config.makeAlgorithm (m_algorithm, workerData).isFailure())
    {
      ANA_MSG_ERROR ("failed to create AnaReentrantAlgorithm: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaReentrantAlgorithmWrapper ::
  execute ()
  {
    using namespace msgAlgorithmConfig;
    RCU_READ_INVARIANT (this);
    if (m_algorithm->sysExecute(Gaudi::Hive::currentContext()).isFailure())
    {
      ANA_MSG_ERROR ("failed to call execute() on algorithm: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaReentrantAlgorithmWrapper ::
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



  StatusCode AnaReentrantAlgorithmWrapper ::
  fileExecute ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }



  StatusCode AnaReentrantAlgorithmWrapper ::
  beginInputFile ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaReentrantAlgorithmWrapper ::
  endInputFile ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }
}
