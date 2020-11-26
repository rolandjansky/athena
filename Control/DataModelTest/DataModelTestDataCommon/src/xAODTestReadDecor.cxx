/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
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
  declareProperty ("CInfoBaseDecorKey", m_cinfoBaseDecorKey);
  declareProperty ("ObjDecorKey",   m_objDecorKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadDecor::initialize()
{
  if (!m_cvecName.empty()) {
    m_cvecDecorKey  = m_readPrefix + m_cvecName +  "."  + m_decorName;
    ATH_CHECK( m_cvecDecorKey.initialize() );
  }

  if (!m_cinfoName.empty()) {
    m_cinfoDecorKey = m_readPrefix + m_cinfoName + "." + m_decorName;
    m_cinfoBaseDecorKey = m_readPrefix + m_cinfoName + "." + m_decorName + "Base";
    m_objDecorKey = m_readPrefix + m_cinfoName + "." + m_decorName;
    ATH_CHECK( m_cinfoDecorKey.initialize() );
    ATH_CHECK( m_cinfoBaseDecorKey.initialize() );
    ATH_CHECK( m_objDecorKey.initialize() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadDecor::execute (const EventContext& ctx) const
{
  if (!m_cvecDecorKey.empty()) {
    std::ostringstream ss;
    SG::ReadDecorHandle<CVec, int> cvecDecor (m_cvecDecorKey, ctx);
    if (!cvecDecor.isPresent()) return StatusCode::FAILURE;
    for (const C* celt : *cvecDecor) {
      ss << " " << cvecDecor(*celt);
    }
    ATH_MSG_INFO (m_cvecDecorKey.key() << ":" << ss.str());
  }

  if (!m_cinfoDecorKey.empty()) {
    SG::ReadDecorHandle<C, int> cinfoDecor (m_cinfoDecorKey, ctx);
    if (!cinfoDecor.isPresent()) return StatusCode::FAILURE;
    ATH_MSG_INFO (m_cinfoDecorKey.key() << ": " << cinfoDecor(0));
  }

  if (!m_cinfoBaseDecorKey.empty()) {
    SG::ReadDecorHandle<C, int> cinfoBaseDecor (m_cinfoBaseDecorKey, ctx);
    if (!cinfoBaseDecor.isPresent()) return StatusCode::FAILURE;
    ATH_MSG_INFO (m_cinfoBaseDecorKey.key() << ": " << cinfoBaseDecor(0));
  }

  if (!m_objDecorKey.empty()) {
    SG::ReadDecorHandle<SG::AuxElement, int> objDecor (m_objDecorKey, ctx);
    if (!objDecor.isPresent()) return StatusCode::FAILURE;
    ATH_MSG_INFO (m_objDecorKey.key() << ": " << objDecor(0));
  }

  if (!m_cinfoDecorKey.empty()) {
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

