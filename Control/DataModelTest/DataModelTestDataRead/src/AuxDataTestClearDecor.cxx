/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/AuxDataTestClearDecor.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test clearing @c DataVector decorations.
 */


#include "AuxDataTestClearDecor.h"
#include "DataModelTestDataCommon/BAux.h"
#include "DataModelTestDataCommon/BAuxVec.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
AuxDataTestClearDecor::AuxDataTestClearDecor (const std::string &name,
                                              ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode AuxDataTestClearDecor::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode AuxDataTestClearDecor::execute()
{
  const BAuxVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, m_readPrefix + "bauxvec") );
  vec->clearDecorations();

  const BAux* b = 0;
  CHECK( evtStore()->retrieve (b, m_readPrefix + "b") );
  b->clearDecorations();

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode AuxDataTestClearDecor::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

