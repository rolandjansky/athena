/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/AuxDataTestDecor.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test adding decorations to a @c DataVector
 *        with auxiliary data.
 */


#include "AuxDataTestDecor.h"
#include "DataModelTestDataCommon/BAux.h"
#include "DataModelTestDataCommon/BAuxVec.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
AuxDataTestDecor::AuxDataTestDecor (const std::string &name,
                                    ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
  declareProperty ("DecorName",   m_decorName = "dInt1");
  declareProperty ("Offset",      m_offset = 100);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode AuxDataTestDecor::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode AuxDataTestDecor::execute()
{
  BAux::Decorator<int> dInt1 (m_decorName);

  const BAuxVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, m_readPrefix + "bauxvec") );
  for (const BAux* belt : *vec)
    dInt1(*belt) = m_offset + belt->m_x;

  const BAux* b = 0;
  CHECK( evtStore()->retrieve (b, m_readPrefix + "b") );
  dInt1(*b) = 400 + m_offset + b->m_x;

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode AuxDataTestDecor::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

