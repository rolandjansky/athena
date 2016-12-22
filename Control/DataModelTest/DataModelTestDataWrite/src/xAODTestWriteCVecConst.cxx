/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCVecConst.cxx
 * @author snyder@bnl.gov
 * @date Jun, 2016
 * @brief Test recording a const aux obj via WriteHandle
 */


#include "xAODTestWriteCVecConst.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWriteCVecConst::xAODTestWriteCVecConst (const std::string &name,
                                                ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cvecKey ("cvecConst")
{
  declareProperty ("CVecKey", m_cvecKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCVecConst::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteCVecConst::execute_r (const EventContext& ctx) const
{
  auto coll = CxxUtils::make_unique<DMTest::CVec>();
  auto store = CxxUtils::make_unique<DMTest::CAuxContainer>();
  coll->setStore (store.get());

  SG::WriteHandle<DMTest::CVec> cvec (m_cvecKey, ctx);
  CHECK( cvec.record (std::move(coll), std::move(store)) );

  size_t count = ctx.evt();
  for (int i=0; i < 10; i++) {
    cvec->push_back (new DMTest::C);
    C& c = *cvec->back();
    c.setAnInt (count * 100 + i+1);
    c.setAFloat (count * 200 + (float)i*0.1);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWriteCVecConst::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

