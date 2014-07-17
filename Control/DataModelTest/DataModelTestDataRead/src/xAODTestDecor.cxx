/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/AuxDataTestDecor.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test adding decorations to xAOD data.
 */


#include "xAODTestDecor.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestDecor::xAODTestDecor (const std::string &name,
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
StatusCode xAODTestDecor::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestDecor::execute()
{
  C::Decorator<int> dInt1 (m_decorName);

  const CVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, m_readPrefix + "cvec") );
  for (const C* celt : *vec)
    dInt1(*celt) = m_offset + celt->anInt();

  const C* c = 0;
  CHECK( evtStore()->retrieve (c, m_readPrefix + "cinfo") );
  dInt1(*c) = 400 + m_offset + c->anInt();

  const CVec* ctrig = 0;
  CHECK( evtStore()->retrieve (ctrig, m_readPrefix + "ctrig") );
  for (const C* celt : *ctrig)
    dInt1(*celt) = 600 + m_offset + celt->anInt();

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestDecor::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

