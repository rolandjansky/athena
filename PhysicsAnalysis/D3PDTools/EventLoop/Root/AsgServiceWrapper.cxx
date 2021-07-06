/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AsgServiceWrapper.h>

#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void AsgServiceWrapper ::
  testInvariant () const
  {
    RCU_INVARIANT (!m_config.name().empty());
  }



  AsgServiceWrapper ::
  AsgServiceWrapper (asg::AsgServiceConfig val_config)
    : m_config (std::move (val_config))
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string_view AsgServiceWrapper ::
  getName () const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name();
  }



  bool AsgServiceWrapper ::
  hasName (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name() == name;
  }



  std::unique_ptr<IAlgorithmWrapper> AsgServiceWrapper ::
  makeClone() const
  {
    RCU_READ_INVARIANT (this);
    return std::make_unique<AsgServiceWrapper> (m_config);
  }



  StatusCode AsgServiceWrapper ::
  initialize (const AlgorithmWorkerData& /*workerData*/)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);
    if (m_config.makeService (m_service).isFailure())
    {
      ANA_MSG_ERROR ("failed to create AsgService: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AsgServiceWrapper ::
  execute ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AsgServiceWrapper ::
  finalize ()
  {
    using namespace msgEventLoop;
    RCU_READ_INVARIANT (this);
    if (m_service->finalize().isFailure())
    {
      ANA_MSG_ERROR ("failed to call finalize() on service: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AsgServiceWrapper ::
  fileExecute ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AsgServiceWrapper ::
  beginInputFile ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  ::StatusCode AsgServiceWrapper ::
  endInputFile ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }
}
