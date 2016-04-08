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
#include "DataModelTestDataCommon/CView.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "DataModelTestDataWrite/GVec.h"
#include "DataModelTestDataWrite/G.h"
#include "DataModelTestDataWrite/GAuxContainer.h"
#include "DataModelTestDataWrite/H.h"
#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataWrite/HView.h"
#include "DataModelTestDataWrite/HAuxContainer.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"


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
    m_hvecKey ("hvec")
{
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWrite::initialize()
{
  ATH_CHECK( m_hvecKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWrite::execute()
{
  ++m_count;

  DMTest::CVec* coll = new DMTest::CVec;
  DMTest::CAuxContainer* store = new DMTest::CAuxContainer;
  coll->setStore (store);

  DMTest::CVec* trig_coll = new DMTest::CVec;
  DMTest::CTrigAuxContainer* trig_store = new DMTest::CTrigAuxContainer;
  trig_coll->setStore (trig_store);

  DMTest::C* cinfo = new DMTest::C;
  DMTest::CInfoAuxContainer* info_store = new DMTest::CInfoAuxContainer;
  cinfo->setStore (info_store);

  CHECK( evtStore()->record (coll, "cvec") );
  CHECK( evtStore()->record (store, "cvecAux.") );

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Decorator<int> dInt1 ("dInt1");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  //static C::Decorator<SG::PackedElement<unsigned int> > dpInt1 ("dpInt1");
  //static C::Decorator<SG::PackedElement<std::vector<float> > > dpvFloat ("dpvFloat");
  static C::Decorator<unsigned int> dpInt1 ("dpInt1");
  static C::Decorator<std::vector<float> > dpvFloat ("dpvFloat");

  for (int i=0; i < 10; i++) {
    coll->push_back (new DMTest::C);
    C& c = *coll->back();
    c.setAnInt (m_count * 100 + i+1);
    c.setAFloat (m_count * 200 + (float)i*0.1);
    c.setPInt (m_count * 500 + i+1);
    c.setPFloat (i + (float)m_count * 0.01);

    std::vector<int> pvi;
    for (int j=0; j<i; j++)
      pvi.push_back (j + i*10 + m_count*100 - 500);
    c.setPVInt (pvi);

    std::vector<float> pvf;
    for (int j=0; j<i; j++)
      pvf.push_back ((float)j*0.1 + (float)i*0.01 + (float)m_count*0.001 - 0.5);
    c.setPVFloat (std::move (pvf));

    anInt2(c) = m_count*300 + i+1;
    dInt1(c) = m_count*400 + i+1;
    dpInt1(c) = m_count*50 + i+1;
    cEL(c).toIndexedElement (*coll, 9-i);

    pvf.clear();
    for (int j=0; j<i; j++)
      pvf.push_back ((float)i*0.1 + (float)m_count*0.01 + (float)j*0.001);
    dpvFloat(c) = std::move(pvf);
  }

  CHECK_OPTION( coll->setOption ("dpInt1", SG::AuxDataOption ("nbits", 13)) );
  CHECK_OPTION( coll->setOption ("dpvFloat", SG::AuxDataOption ("nbits", 13)) );
  CHECK_OPTION( coll->setOption ("dpvFloat", SG::AuxDataOption ("signed", 0)));
  CHECK_OPTION( coll->setOption ("dpvFloat", SG::AuxDataOption ("nmantissa", 13)) );

  for (int i=0; i < 8; i++) {
    trig_coll->push_back (new DMTest::C);
    C& c = *trig_coll->back();
    c.setAnInt (m_count * 500 + i+1);
    c.setAFloat (m_count * 600 + (float)i*0.1);

    anInt2(c) = m_count*700 + i+1;
    dInt1(c) = m_count*480 + i+1;
  }

  cinfo->setAnInt (m_count * 1000);
  cinfo->setAFloat ((float)m_count * 0.1);
  anInt2(*cinfo) = m_count * 2000;
  dInt1(*cinfo) = m_count * 3000;

  CHECK( evtStore()->setConst (coll) );
  CHECK( evtStore()->setConst (store) );

  CHECK( evtStore()->record (trig_coll, "ctrig") );
  CHECK( evtStore()->record (trig_store, "ctrigAux.") );
  CHECK( evtStore()->setConst (trig_coll) );
  CHECK( evtStore()->setConst (trig_store) );

  cEL(*cinfo).toIndexedElement (*coll, m_count % coll->size());

  CHECK( evtStore()->record (cinfo, "cinfo") );
  CHECK( evtStore()->record (info_store, "cinfoAux.") );
  CHECK( evtStore()->setConst (cinfo) );
  CHECK( evtStore()->setConst (info_store) );

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
  CHECK( write_cview (*coll) );
  CHECK( write_htest() );

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


/**
 * @brief Test writing view container.
 */
StatusCode xAODTestWrite::write_cview (DMTest::CVec& coll)
{
  auto cview = CxxUtils::make_unique<DMTest::CView>();
  for (int i = coll.size()-1; i >= 0; --i)
    cview->push_back (coll[i]);

  CHECK( DMTest::recordView1 (&*evtStore(), std::move(cview), "cview") );

  return StatusCode::SUCCESS;
}


/**
 * @brief Test schema evolution involving view container.
 */
StatusCode xAODTestWrite::write_htest()
{
  auto hvec = CxxUtils::make_unique<DMTest::HVec>();
  auto store = CxxUtils::make_unique<DMTest::HAuxContainer>();
  hvec->setStore (store.get());
  auto hview = CxxUtils::make_unique<DMTest::HView>();

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


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWrite::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

