/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <EventLoopTest/UnitTestAlg5.h>

//
// method implementations
//

namespace EL
{
  UnitTestAlg5 ::
  UnitTestAlg5 (const std::string& name, ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("property", m_property, "test property");
    declareProperty ("string_property", m_string_property, "test string property");
    declareProperty ("toolHandle", m_toolHandle, "ToolHandle property");
  }



  ::StatusCode UnitTestAlg5 ::
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



  ::StatusCode UnitTestAlg5 ::
  execute ()
  {
    if (!m_wasInitialized)
    {
      ANA_MSG_FATAL ("never initialized");
      return StatusCode::FAILURE;
    }
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode UnitTestAlg5 ::
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
