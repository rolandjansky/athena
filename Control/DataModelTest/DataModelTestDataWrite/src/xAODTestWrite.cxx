/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/xAODTestWrite.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test writing xAOD classes with auxiliary data.
 */


#include "xAODTestWrite.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWrite::xAODTestWrite (const std::string &name,
                              ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWrite::initialize()
{
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

  DMTest::C* c = new DMTest::C;
  DMTest::CInfoAuxContainer* info_store = new DMTest::CInfoAuxContainer;
  c->setStore (info_store);

  CHECK( evtStore()->record (coll, "cvec") );
  CHECK( evtStore()->record (store, "cvecAux.") );

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Decorator<int> dInt1 ("dInt1");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  for (int i=0; i < 10; i++) {
    coll->push_back (new DMTest::C);
    C& c = *coll->back();
    c.setAnInt (m_count * 100 + i+1);
    c.setAFloat (m_count * 200 + (float)i/10);

    anInt2(c) = m_count*300 + i+1;
    dInt1(c) = m_count*400 + i+1;
    cEL(c).toIndexedElement (*coll, 9-i);
  }

  for (int i=0; i < 8; i++) {
    trig_coll->push_back (new DMTest::C);
    C& c = *trig_coll->back();
    c.setAnInt (m_count * 500 + i+1);
    c.setAFloat (m_count * 600 + (float)i/10);

    anInt2(c) = m_count*700 + i+1;
    dInt1(c) = m_count*480 + i+1;
  }

  c->setAnInt (m_count * 1000);
  c->setAFloat ((float)m_count / 10);
  anInt2(*c) = m_count * 2000;
  dInt1(*c) = m_count * 3000;

  CHECK( evtStore()->setConst (coll) );
  CHECK( evtStore()->setConst (store) );

  CHECK( evtStore()->record (trig_coll, "ctrig") );
  CHECK( evtStore()->record (trig_store, "ctrigAux.") );
  CHECK( evtStore()->setConst (trig_coll) );
  CHECK( evtStore()->setConst (trig_store) );

  cEL(*c).toIndexedElement (*coll, m_count % coll->size());

  CHECK( evtStore()->record (c, "cinfo") );
  CHECK( evtStore()->record (info_store, "cinfoAux.") );
  CHECK( evtStore()->setConst (c) );
  CHECK( evtStore()->setConst (info_store) );

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

