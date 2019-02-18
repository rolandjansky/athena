/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCVec.cxx
 * @author snyder@bnl.gov
 * @date Apr 2016
 * @brief Algorithm to test writing xAOD classes with auxiliary data (cvec).
 */


#include "xAODTestWriteCVec.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "StoreGate/WriteDecorHandle.h"
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
xAODTestWriteCVec::xAODTestWriteCVec (const std::string &name,
                                      ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cvecKey ("cvec")
{
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("CVecDecorKey", m_cvecDecorKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCVec::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );

  if (m_cvecDecorKey.key().empty())
    m_cvecDecorKey = m_cvecKey.key() + ".dtest";
  ATH_CHECK( m_cvecDecorKey.initialize() );
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteCVec::execute (const EventContext& ctx) const
{
  unsigned int count = ctx.eventID().event_number() + 1;

  auto coll = CxxUtils::make_unique<DMTest::CVec>();
  auto store = CxxUtils::make_unique<DMTest::CAuxContainer>();
  coll->setStore (store.get());

  const static C::Accessor<int> anInt2 ("anInt2");
  const static C::Decorator<int> dVar1 ("dVar1");
  const static C::Decorator<int> dVar2 ("dVar2");
  const static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  const static C::Decorator<unsigned int> dpInt1 ("dpInt1");
  const static C::Decorator<std::vector<float> > dpvFloat ("dpvFloat");

  int nent = 10;
  if (count == 5) nent = 0;
  for (int i=0; i < nent; i++) {
    coll->push_back (new DMTest::C);
    C& c = *coll->back();
    c.setAnInt (count * 100 + i+1);
    c.setAFloat (count * 200 + (float)i*0.1);
    c.setPInt (count * 500 + i+1);
    c.setPFloat (i + (float)count * 0.01);

    std::vector<int> pvi;
    for (int j=0; j<i; j++)
      pvi.push_back (j + i*10 + count*100 - 500);
    c.setPVInt (pvi);

    std::vector<float> pvf;
    for (int j=0; j<i; j++)
      pvf.push_back ((float)j*0.1 + (float)i*0.01 + (float)count*0.001 - 0.5);
    c.setPVFloat (std::move (pvf));

    anInt2(c) = count*300 + i+1;
    dVar1(c) = count*450 + i+1;
    dVar2(c) = count*750 + i+1;
    dpInt1(c) = count*50 + i+1;
    cEL(c).toIndexedElement (*coll, 9-i);

    pvf.clear();
    for (int j=0; j<i; j++)
      pvf.push_back ((float)i*0.1 + (float)count*0.01 + (float)j*0.001);
    dpvFloat(c) = std::move(pvf);
  }

  SG::WriteHandle<DMTest::CVec> cvec (m_cvecKey, ctx);
  CHECK( cvec.record (std::move(coll), std::move(store)) );

  SG::WriteDecorHandle<DMTest::CVec, int> dtest (m_cvecDecorKey, ctx);
  for (int i=0; i < nent; i++) {
    dtest (*(*dtest)[i]) = i+123;
  }

  CHECK_OPTION( cvec->setOption ("dpInt1", SG::AuxDataOption ("nbits", 13)) );
  CHECK_OPTION( cvec->setOption ("dpvFloat", SG::AuxDataOption ("nbits", 13)) );
  CHECK_OPTION( cvec->setOption ("dpvFloat", SG::AuxDataOption ("signed", 0)));
  CHECK_OPTION( cvec->setOption ("dpvFloat", SG::AuxDataOption ("nmantissa", 13)) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWriteCVec::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

