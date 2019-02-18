/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWrite.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test writing xAOD classes with auxiliary data.
 */


#include "xAODTestWrite.h"
#include "xAODTestWriteHelper.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataWrite/GVec.h"
#include "DataModelTestDataWrite/G.h"
#include "DataModelTestDataWrite/GAuxContainer.h"
//#include "DataModelTestDataWrite/H.h"
//#include "DataModelTestDataWrite/HVec.h"
//#include "DataModelTestDataWrite/HView.h"
//#include "DataModelTestDataWrite/HAuxContainer.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"


#define CHECK_OPTION(ret)                       \
  do {                                          \
    if (!ret) {                                 \
      ATH_MSG_ERROR("setOption failed");        \
      return StatusCode::FAILURE;               \
    }                                           \
  } while(0)


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWrite::xAODTestWrite (const std::string &name,
                              ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator)
{
  declareProperty ("CVecKey", m_cvecKey = "cvec");
  declareProperty ("CTrigKey", m_ctrigKey = "ctrig");
  declareProperty ("GVecKey", m_gvecKey = "gvec");
  declareProperty ("CVecWDKey", m_cvecWDKey = "cvecWD");
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWrite::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  ATH_CHECK( m_ctrigKey.initialize() );
  ATH_CHECK( m_gvecKey.initialize() );
  ATH_CHECK( m_cvecWDKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWrite::execute (const EventContext& ctx) const
{
  unsigned int count = ctx.eventID().event_number() + 1;

  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey);

  auto trig_coll = std::make_unique<DMTest::CVec>();
  auto trig_store = std::make_unique<DMTest::CTrigAuxContainer>();
  trig_coll->setStore (trig_store.get());

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  //static C::Decorator<SG::PackedElement<unsigned int> > dpInt1 ("dpInt1");
  //static C::Decorator<SG::PackedElement<std::vector<float> > > dpvFloat ("dpvFloat");
  static C::Decorator<unsigned int> dpInt1 ("dpInt1");
  static C::Decorator<std::vector<float> > dpvFloat ("dpvFloat");

  for (int i=0; i < 8; i++) {
    trig_coll->push_back (new DMTest::C);
    C& c = *trig_coll->back();
    c.setAnInt (count * 500 + i+1);
    c.setAFloat (count * 600 + (float)i*0.1);

    anInt2(c) = count*700 + i+1;
  }

  SG::WriteHandle<DMTest::CVec> ctrig (m_ctrigKey, ctx);
  CHECK( ctrig.record (std::move (trig_coll), std::move (trig_store)) );

  auto gcont = std::make_unique<DMTest::GVec>();
  auto gstore = std::make_unique<DMTest::GAuxContainer>();
  gcont->setStore (gstore.get());

  for (int i=0; i < 10; i++) {
    gcont->push_back (new DMTest::G);
    G& g = *gcont->back();
    g.setAnInt (count * 700 + i+1);
  }

  SG::WriteHandle<DMTest::GVec> gvec (m_gvecKey, ctx);
  CHECK( gvec.record (std::move (gcont), std::move (gstore)) );

  CHECK( write_cvec_with_data (count, ctx) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Test writing container with additional data.
 */
StatusCode xAODTestWrite::write_cvec_with_data (unsigned int count,
                                                const EventContext& ctx) const
{
  auto coll = std::make_unique<DMTest::CVecWithData>();
  auto store = std::make_unique<DMTest::CAuxContainer>();
  coll->setStore (store.get());

  coll->meta1 = count + 1000;
  for (int i=0; i < 10; i++) {
    coll->push_back (new DMTest::C);
    C& c = *coll->back();
    c.setAnInt (count * 200 + i+1);
  }

  SG::WriteHandle<DMTest::CVecWithData> cvecWD (m_cvecWDKey, ctx);
  CHECK( cvecWD.record (std::move (coll), std::move (store)) );

  return StatusCode::SUCCESS;
}


#if 0
/**
 * @brief Test schema evolution involving view container.
 */
StatusCode xAODTestWrite::write_htest()
{
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
  CHECK( DMTest::recordView2 (&*evtStore(), std::move(hview), "hview") );

  return StatusCode::SUCCESS;
}
#endif


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWrite::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

