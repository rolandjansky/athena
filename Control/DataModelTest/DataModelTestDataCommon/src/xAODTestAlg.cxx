/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Algorithm used to test SG access from tools.
 */


#include "xAODTestAlg.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestAlg::xAODTestAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_tools (this)
{
  declareProperty ("Tools", m_tools);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestAlg::initialize()
{
  ATH_CHECK( m_tools.retrieve() );
  return StatusCode::SUCCESS;
}



/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestAlg::execute_r (const EventContext& ctx) const
{
  for (const auto& t : m_tools)
    ATH_CHECK( t->doit (ctx) );
  return StatusCode::SUCCESS;
}


} // namespace DMTest
