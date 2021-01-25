/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <EventLoopTest/UnitTestAlg6.h>

//
// method implementations
//

namespace EL
{
  UnitTestAlg6 ::
  UnitTestAlg6 (const std::string& name, ISvcLocator* pSvcLocator)
    : AnaReentrantAlgorithm (name, pSvcLocator)
  {
    declareProperty ("property", m_property, "test property");
    declareProperty ("string_property", m_string_property, "test string property");
    declareProperty ("toolHandle", m_toolHandle, "ToolHandle property");
  }



  ::StatusCode UnitTestAlg6 ::
  initialize ()
  {
    if (m_wasInitialized)
    {
      ANA_MSG_FATAL ("initialized twice");
      return StatusCode::FAILURE;
    }

    if (!m_toolHandle.empty())
      ANA_CHECK (m_toolHandle.retrieve());

    m_wasInitialized = true;
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode UnitTestAlg6 ::
  execute (const EventContext& /*ctx*/)
  {
    if (!m_wasInitialized)
    {
      ANA_MSG_FATAL ("never initialized");
      return StatusCode::FAILURE;
    }
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode UnitTestAlg6 ::
  finalize ()
  {
    if (!m_wasInitialized)
    {
      ANA_MSG_FATAL ("never initialized");
      return StatusCode::FAILURE;
    }
    return ::StatusCode::SUCCESS;
  }
}
