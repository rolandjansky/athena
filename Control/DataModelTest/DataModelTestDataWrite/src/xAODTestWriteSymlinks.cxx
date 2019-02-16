/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteSymlinks.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Testing making symlinks/aliases through handles.
 */


#include "xAODTestWriteSymlinks.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWriteSymlinks::xAODTestWriteSymlinks (const std::string &name,
                                              ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator)
{
  declareProperty ("S2Key", m_s2Key = "S2");
  declareProperty ("S1Key", m_s1Key = "S2");
  declareProperty ("AliasKey", m_aliasKey = "S2alias");
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteSymlinks::initialize()
{
  m_s1Key = m_s2Key.key();

  ATH_CHECK( m_s2Key.initialize() );
  ATH_CHECK( m_s1Key.initialize() );
  ATH_CHECK( m_aliasKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteSymlinks::execute (const EventContext& ctx) const
{
  SG::WriteHandle<DMTest::S2> s2 (m_s2Key, ctx);
  unsigned int eventNumber = ctx.eventID().event_number();
  ATH_CHECK( s2.record (std::make_unique<DMTest::S2> (eventNumber*100)) );
  ATH_CHECK( s2.alias (m_aliasKey) );
  ATH_CHECK( s2.symLink (m_s1Key) );
  return StatusCode::SUCCESS;
}


} // namespace DMTest

