/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  : AthAlgorithm (name, pSvcLocator),
    m_count(0),
    m_cvecKey ("cvec")
    //m_hvecKey ("hvec")
{
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWrite::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  //ATH_CHECK( m_hvecKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWrite::execute()
{
  ++m_count;

  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey);

  DMTest::CVec* trig_coll = new DMTest::CVec;
  DMTest::CTrigAuxContainer* trig_store = new DMTest::CTrigAuxContainer;
  trig_coll->setStore (trig_store);

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Decorator<int> dInt1 ("dInt1");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  //static C::Decorator<SG::PackedElement<unsigned int> > dpInt1 ("dpInt1");
  //static C::Decorator<SG::PackedElement<std::vector<float> > > dpvFloat ("dpvFloat");
  static C::Decorator<unsigned int> dpInt1 ("dpInt1");
  static C::Decorator<std::vector<float> > dpvFloat ("dpvFloat");

  for (int i=0; i < 8; i++) {
    trig_coll->push_back (new DMTest::C);
    C& c = *trig_coll->back();
    c.setAnInt (m_count * 500 + i+1);
    c.setAFloat (m_count * 600 + (float)i*0.1);

    anInt2(c) = m_count*700 + i+1;
    dInt1(c) = m_count*480 + i+1;
  }

  CHECK( evtStore()->record (trig_coll, "ctrig") );
  CHECK( evtStore()->record (trig_store, "ctrigAux.") );
  CHECK( evtStore()->setConst (trig_coll) );
  CHECK( evtStore()->setConst (trig_store) );

  DMTest::GVec* gvec = new DMTest::GVec;
  DMTest::GAuxContainer* gstore = new DMTest::GAuxContainer;
  gvec->setStore (gstore);

  for (int i=0; i < 10; i++) {
    gvec->push_back (new DMTest::G);
    G& g = *gvec->back();
    g.setAnInt (m_count * 700 + i+1);
  }

  CHECK( evtStore()->record (gvec, "gvec") );
  CHECK( evtStore()->record (gstore, "gvecAux.") );
  CHECK( evtStore()->setConst (gvec) );
  CHECK( evtStore()->setConst (gstore) );

  CHECK( write_cvec_with_data() );
  //CHECK( write_htest() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Test writing container with additional data.
 */
StatusCode xAODTestWrite::write_cvec_with_data()
{
  DMTest::CVecWithData* coll = new DMTest::CVecWithData;
  DMTest::CAuxContainer* store = new DMTest::CAuxContainer;
  coll->setStore (store);

  coll->meta1 = m_count + 1000;
  for (int i=0; i < 10; i++) {
    coll->push_back (new DMTest::C);
    C& c = *coll->back();
    c.setAnInt (m_count * 200 + i+1);
  }

  CHECK( evtStore()->record (coll, "cvecWD") );
  CHECK( evtStore()->record (store, "cvecWDAux.") );
  CHECK( evtStore()->setConst (coll) );
  CHECK( evtStore()->setConst (store) );

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
    hvec->back()->setAnInt (i+1 + m_count * 400);
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

