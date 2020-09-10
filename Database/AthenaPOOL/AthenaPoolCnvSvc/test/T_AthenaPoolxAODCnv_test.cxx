/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolCnvSvc/test/T_AthenaPoolxAODCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for T_AthenaPoolxAODCnv_test.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "PersistentDataModel/TokenAddress.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "TestCnvSvcBase.icc"
#include "TSystem.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <vector>


using namespace AthenaPoolCnvSvcTest;


const std::string YCont_v1_guid = "05309E49-5567-4790-BE56-2E541E0B4B24";
const std::string YCont_v2_guid = "0CC6B32E-6C95-4B0E-B97A-9B9040A8A9BE";


class TestCnvSvc
  : public TestCnvSvcBase
{
public:
  TestCnvSvc (const std::string& name, ISvcLocator* svcloc)
    : TestCnvSvcBase (name, svcloc),
      m_pers1 (nullptr),
      m_pers2 (nullptr)
  {}

  virtual void setObjPtr(void*& obj, const Token* /*token*/) const override
  {
    if (m_pers2) {
      DataVector<Y_v2>* v = new DataVector<Y_v2>;
      for (const Y_v2* y : *m_pers2)
        v->push_back (new Y_v2(*y));
      obj = v;
    }
    else if (m_pers1) {
      DataVector<Y_v1>* v = new DataVector<Y_v1>;
      for (const Y_v1* y : *m_pers1)
        v->push_back (new Y_v1(*y));
      obj = v;
    }
    else
      std::abort();
  }

  std::string m_name;
  DataVector<Y_v1>* m_pers1;
  DataVector<Y_v2>* m_pers2;
};


class YCnv_v1
{
public:
  typedef DataVector<Y_v2> Trans_t;
  typedef DataVector<Y_v1> Pers_t;
  
  DataVector<Y_v2>* createTransient (const DataVector<Y_v1>* pers, MsgStream& msg)
  {
    auto trans = std::make_unique<DataVector<Y_v2> >();
    persToTrans (pers, trans.get(), msg);
    return trans.release();
  }

  void persToTrans (const DataVector<Y_v1>* pers, DataVector<Y_v2>* trans, MsgStream&)
  {
    for (const Y_v1* y : *pers)
      trans->push_back (new Y_v2 (y->m_a * 3));
  }
};


void test1 (ISvcLocator* svcloc, TestCnvSvc& testsvc)
{
  std::cout << "test1\n";
  const size_t N = 10;

  T_AthenaPoolxAODCnv<DataVector<Y_v2>, YCnv_v1> cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  DataVector<Y_v2> trans1;
  for (size_t i=0; i < N; i++)
    trans1.push_back (new Y_v2(i));

  DataVector<Y_v2>* pers1 = cnv.createPersistentWithKey (&trans1, "");
  assert (pers1->size() == trans1.size());
  for (size_t i = 0; i < N; i++)
    assert ((*pers1)[i] == trans1[i]);

  testsvc.m_pers2 = pers1;
  Token* token = new Token;
  token->setClassID (Guid (YCont_v2_guid));
  TokenAddress taddr (0, 0, "xyz", "key", 0, token);

  DataObject* pObj = nullptr;
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans2 = SG::Storable_cast<DataVector<Y_v2> > (pObj);
  assert (trans2->size() == 10);
  for (size_t i = 0; i < 10; i++)
    assert ((*trans2)[i]->m_a == (int)i);
  assert (trans2->getConstStoreLink().dataID() == "keyAux.");
  delete pObj;
  delete pers1;
  testsvc.m_pers2 = nullptr;

  DataVector<Y_v1> pers_old;
  for (size_t i=0; i < N; i++)
    pers_old.push_back (new Y_v1(i));
  testsvc.m_pers1 = &pers_old;
  token->setClassID (Guid (YCont_v1_guid));
  pObj = nullptr;
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans3 = SG::Storable_cast<DataVector<Y_v2> > (pObj);
  assert (trans3->size() == 10);
  for (size_t i = 0; i < 10; i++)
    assert ((*trans3)[i]->m_a == (int)i*3);
  assert (trans3->getConstStoreLink().dataID() == "keyAux.");
  delete pObj;

  token->setClassID (Guid ("8ACD1C53-D3C7-4FE5-9BC0-E388701DB8FA"));
  pObj = nullptr;
  assert (cnv.createObj (&taddr, pObj).isFailure());
}


// Test writing with thinning.
void test2 (ISvcLocator* svcloc, TestCnvSvc& /*testsvc*/)
{
  std::cout << "test2\n";
  const size_t N = 10;

  T_AthenaPoolxAODCnv<DataVector<Y_v2>, YCnv_v1> cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  DataVector<Y_v2> trans1;
  for (size_t i=0; i < N; i++)
    trans1.push_back (new Y_v2(i));

  DataVector<Y_v2>* pers1 = cnv.createPersistentWithKey (&trans1, "Y");
  assert (pers1->size() == trans1.size());
  for (size_t i = 0; i < N; i++)
    assert ((*pers1)[i] == trans1[i]);
  delete pers1;

  SG::ThinningCache cache;
  SG::ThinningDecisionBase dec;
  dec.resize (N);
  dec.thin (5);
  dec.buildIndexMap();
  cache.addThinning ("Y",
                     std::vector<SG::sgkey_t> { },
                     &dec);

  EventContext ctx;
  Atlas::ExtendedEventContext ectx;
  ectx.setThinningCache (&cache);
  Atlas::setExtendedEventContext (ctx, std::move (ectx));
  Gaudi::Hive::setCurrentContext (ctx);

  DataVector<Y_v2>* pers2 = cnv.createPersistentWithKey (&trans1, "Y");
  assert (pers2->size() == trans1.size()-1);
  size_t ii = 0;
  for (size_t i = 0; i < N-1; i++) {
    if (i == 5) ++ii;
    assert ((*pers2)[i] == trans1[ii]);
    ++ii;
  }
  delete pers2;

  ectx.setThinningCache (nullptr);
  Atlas::setExtendedEventContext (ctx, std::move (ectx));
  Gaudi::Hive::setCurrentContext (ctx);
}


int main()
{
  CxxUtils::ubsan_suppress ([]() {TInterpreter::Instance(); });
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi("AthenaPoolCnvSvc/test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();

  TestCnvSvc* svc = new TestCnvSvc ("AthenaPoolCnvSvc", pSvcLoc);
  ISvcManager* mgr = dynamic_cast<ISvcManager*> (pSvcLoc);
  if (!mgr) std::abort();
  if (mgr->addService (svc).isFailure())
    std::abort();
  
  gSystem->Load("libAthenaPoolCnvSvcTestDict");

  test1 (pSvcLoc, *svc);
  test2 (pSvcLoc, *svc);
  return 0;
}
