/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/AuxDataTestDecor.cxx
 * @author snyder@bnl.gov
 * @date Apr 2017
 * @brief Algorithm to test reading decorations using handles.
 */


#include "xAODTestReadDecor.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "StoreGate/ReadDecorHandle.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestReadDecor::xAODTestReadDecor (const std::string &name,
                                      ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
  declareProperty ("DecorName",   m_decorName = "dInt1");
  declareProperty ("CVecName",    m_cvecName  = "cvec");
  declareProperty ("CInfoName",   m_cinfoName = "cinfo");

  declareProperty ("CVecDecorKey",  m_cvecDecorKey);
  declareProperty ("CInfoDecorKey", m_cinfoDecorKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadDecor::initialize()
{
  m_cvecDecorKey  = m_readPrefix + m_cvecName +  "."  + m_decorName;
  m_cinfoDecorKey = m_readPrefix + m_cinfoName + "." + m_decorName;

  ATH_CHECK( m_cvecDecorKey.initialize() );
  ATH_CHECK( m_cinfoDecorKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadDecor::execute_r (const EventContext& ctx) const
{
  {
    std::ostringstream ss;
    SG::ReadDecorHandle<CVec, int> cvecDecor (m_cvecDecorKey, ctx);
    if (!cvecDecor.isPresent()) return StatusCode::FAILURE;
    for (const C* celt : *cvecDecor) {
      ss << " " << cvecDecor(*celt);
    }
    ATH_MSG_INFO (m_cvecDecorKey.key() << ":" << ss.str());
  }

  {
    SG::ReadDecorHandle<C, int> cinfoDecor (m_cinfoDecorKey, ctx);
    if (!cinfoDecor.isPresent()) return StatusCode::FAILURE;
    ATH_MSG_INFO (m_cinfoDecorKey.key() << ": " << cinfoDecor(0));
  }

  {
    SG::ReadDecorHandleKey<C> testKey (m_cinfoDecorKey);
    testKey = testKey.key() + "_test";
    SG::ReadDecorHandle<C, int> cinfoDecorTest (testKey, ctx);
    if (!cinfoDecorTest.isPresent()) return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestReadDecor::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

