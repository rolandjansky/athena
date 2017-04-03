/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelTestDataRead/src/xAODTestReadCView.cxx
 * @author snyder@bnl.gov
 * @date Oct 2016
 * @brief Algorithm to test reading xAOD data (CView).
 */


#include "xAODTestReadCView.h"
#include "DataModelTestDataCommon/CView.h"
#include "DataModelTestDataCommon/C.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include <memory>
#include <sstream>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestReadCView::xAODTestReadCView (const std::string &name,
                                      ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cviewKey ("cview"),
    m_writeKey ("")
{
  declareProperty ("CViewKey", m_cviewKey);
  declareProperty ("WriteKey", m_writeKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadCView::initialize()
{
  ATH_CHECK( m_cviewKey.initialize() );
  if (!m_writeKey.key().empty())
    ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadCView::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<DMTest::CView> cview (m_cviewKey, ctx);

  static C::Accessor<int> anInt10 ("anInt10");
  std::cout << m_cviewKey.key() << ":";
  for (const C* c : *cview) {
    std::cout << " " << c->anInt();
    if (anInt10.isAvailable(*c))
      std::cout << "(" << anInt10(*c) << ")";
  }
  std::cout << "\n";

  if (!m_writeKey.key().empty()) {
    SG::WriteHandle<DMTest::CView> writeview (m_writeKey, ctx);
    ATH_CHECK( writeview.record(std::make_unique<DMTest::CView> (*cview)) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestReadCView::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

