/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteHVec.cxx
 * @author snyder@bnl.gov
 * @date Oct, 2016
 * @brief Algorithm to test writing xAOD classes for schema evolution (hvec/hview).
 */


#include "xAODTestWriteHVec.h"
#include "xAODTestWriteHelper.h"
#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataWrite/HView.h"
#include "DataModelTestDataWrite/H.h"
#include "DataModelTestDataWrite/HAuxContainer.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWriteHVec::xAODTestWriteHVec (const std::string &name,
                                      ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_hvecKey ("hvec"),
    m_hviewKey ("hview")
{
  declareProperty ("HVecKey", m_hvecKey);
  declareProperty ("HViewKey", m_hviewKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteHVec::initialize()
{
  ATH_CHECK( m_hvecKey.initialize() );
  ATH_CHECK( m_hviewKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteHVec::execute (const EventContext& ctx) const
{
  unsigned int count = ctx.eventID().event_number() + 1;

  auto hvec = std::make_unique<DMTest::HVec>();
  auto store = std::make_unique<DMTest::HAuxContainer>();
  hvec->setStore (store.get());
  auto hview = std::make_unique<DMTest::HView>();

  for (int i = 0; i < 20; i++) {
    hvec->push_back (new DMTest::H);
    hvec->back()->setAnInt (i+1 + count * 400);
  }

  for (int i = 0; i < 20; i++) {
    hview->push_back (hvec->at (19-i));
  }

  ATH_CHECK( SG::makeHandle(m_hvecKey).record (std::move(hvec),
                                               std::move(store)) );

  SG::WriteHandle<DMTest::HView> hviewH (m_hviewKey, ctx);
  ATH_CHECK( DMTest::recordView2 (hviewH, std::move(hview)) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWriteHVec::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

