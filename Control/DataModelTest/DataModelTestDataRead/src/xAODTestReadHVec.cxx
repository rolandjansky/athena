/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelTestDataRead/src/xAODTestReadHVec.cxx
 * @author snyder@bnl.gov
 * @date Oct, 2016
 * @brief Algorithm to test reading xAOD data with schema evolution (HVec/HView)
 */


#include "xAODTestReadHVec.h"
#include "DataModelTestDataRead/HVec.h"
#include "DataModelTestDataRead/HView.h"
#include "DataModelTestDataRead/H.h"
#include "DataModelTestDataRead/HAuxContainer.h"
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
xAODTestReadHVec::xAODTestReadHVec (const std::string &name,
                                    ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_hvecKey ("hvec"),
    m_hviewKey ("hview"),
    m_vecWriteKey (""),
    m_viewWriteKey ("")
{
  declareProperty ("HVecKey", m_hvecKey);
  declareProperty ("HViewKey", m_hviewKey);
  declareProperty ("VecWriteKey", m_vecWriteKey);
  declareProperty ("ViewWriteKey", m_viewWriteKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadHVec::initialize()
{
  ATH_CHECK( m_hvecKey.initialize() );
  if (!m_hviewKey.key().empty())
    ATH_CHECK( m_hviewKey.initialize() );
  if (!m_vecWriteKey.key().empty())
    ATH_CHECK( m_vecWriteKey.initialize() );
  if (!m_viewWriteKey.key().empty())
    ATH_CHECK( m_viewWriteKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadHVec::execute_r (const EventContext& ctx) const
{
  const DMTest::HVec* hvec = SG::get (m_hvecKey, ctx);
  std::cout << m_hvecKey.key() << ":";
  for (const H* h : *hvec)
    std::cout << " " << h->aFloat();
  std::cout << "\n";

  if (const DMTest::HVec* hview = SG::get (m_hviewKey, ctx)) {
    std::cout << m_hviewKey.key() << ":";
    for (const H* h : *hview)
      std::cout << " " << h->aFloat();
    std::cout << "\n";


    if (!m_vecWriteKey.key().empty()) {
      auto vecnew = std::make_unique<HVec>();
      auto store = std::make_unique<HAuxContainer>();
      vecnew->setStore (store.get());
      for (size_t i = 0; i < hview->size(); i++) {
        vecnew->push_back (new H);
        *vecnew->back() = *(*hview)[i];
      }

      auto viewnew = std::make_unique<HView>();
      for (size_t i = 0; i < vecnew->size(); i++)
        viewnew->push_back (vecnew->at(vecnew->size()-1-i));

      SG::WriteHandle<DMTest::HVec> writevec (m_vecWriteKey, ctx);
      SG::WriteHandle<DMTest::HView> writeview (m_viewWriteKey, ctx);
      ATH_CHECK( writevec.record (std::move(vecnew), std::move(store)) );
      ATH_CHECK( writeview.record (std::move(viewnew)) );
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestReadHVec::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

