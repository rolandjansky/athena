/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AsgToolWrapper.h>

#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void AsgToolWrapper ::
  testInvariant () const
  {
    RCU_INVARIANT (!m_config.name().empty());
  }



  AsgToolWrapper ::
  AsgToolWrapper (asg::AsgToolConfig val_config)
    : m_config (std::move (val_config))
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string_view AsgToolWrapper ::
  getName () const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name();
  }



  bool AsgToolWrapper ::
  hasName (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_config.name() == name;
  }



  std::unique_ptr<IAlgorithmWrapper> AsgToolWrapper ::
  makeClone() const
  {
    RCU_READ_INVARIANT (this);
    return std::make_unique<AsgToolWrapper> (m_config);
  }



  StatusCode AsgToolWrapper ::
  initialize (const AlgorithmWorkerData& /*workerData*/)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);
    if (m_config.makeTool (m_tool, m_cleanup).isFailure())
    {
      ANA_MSG_ERROR ("failed to create AsgTool: " << m_config.name());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AsgToolWrapper ::
  execute ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AsgToolWrapper ::
  finalize ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AsgToolWrapper ::
  fileExecute ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  StatusCode AsgToolWrapper ::
  beginInputFile ()
  {
    RCU_READ_INVARIANT (this);
    return StatusCode::SUCCESS;
  }



  ::StatusCode AsgToolWrapper ::
  endInputFile ()
  {
    // no-op
    return StatusCode::SUCCESS;
  }
}
