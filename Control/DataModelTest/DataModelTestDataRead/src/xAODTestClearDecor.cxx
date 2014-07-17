/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/xAODTestClearDecor.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test clearing xAOD decorations.
 */


#include "xAODTestClearDecor.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestClearDecor::xAODTestClearDecor (const std::string &name,
                                        ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestClearDecor::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestClearDecor::execute()
{
  const CVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, m_readPrefix + "cvec") );
  vec->clearDecorations();

  const C* c = 0;
  CHECK( evtStore()->retrieve (c, m_readPrefix + "cinfo") );
  c->clearDecorations();

  const CVec* ctrig = 0;
  CHECK( evtStore()->retrieve (ctrig, m_readPrefix + "ctrig") );
  ctrig->clearDecorations();

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestClearDecor::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

