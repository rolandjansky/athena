/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/test/T_AthenaPoolTPCnvCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for T_AthenaPoolTPCnvCnv_test.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "PersistentDataModel/TokenAddress.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/ubsan_suppress.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "TestCnvSvcBase.icc"
#include "TSystem.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <vector>


using namespace AthenaPoolCnvSvcTest;


const std::string XCont_guid    = "6AEA6831-8777-4571-8595-95FFF40B171F";
const std::string XCont_p1_guid = "421C967C-CB57-4234-A33F-EBE4928036C2";
const std::string XCont_p2_guid = "F151C81D-869A-4585-8086-5F8835AB12E1";


class TestCnvSvc
  : public TestCnvSvcBase
{
public:
  TestCnvSvc (const std::string& name, ISvcLocator* svcloc)
    : TestCnvSvcBase (name, svcloc),
      m_pers0 (nullptr),
      m_pers1 (nullptr),
      m_pers2 (nullptr)
  {}

  virtual void setObjPtr(void*& obj, const Token* /*token*/) const override
  {
    if (m_pers2) {
      obj = new XCont_p2 (*m_pers2);
    }
    else if (m_pers1) {
      obj = new XCont_p1 (*m_pers1);
    }
    else if (m_pers0) {
      obj = new XCont (*m_pers0);
    }
    else
      std::abort();
  }

  std::string m_name;
  XCont* m_pers0;
  XCont_p1* m_pers1;
  XCont_p2* m_pers2;
};


class XContCnv_p1
  : public T_AthenaPoolTPCnvBase<XCont, XCont_p1>
{
public:
  virtual
  void persToTrans (const Pers_t* pers, Trans_t* trans, MsgStream&) override
  {
    trans->clear();
    for (const X_p1& x : pers->m_v)
      trans->push_back (new X (x.m_a / 20));
  }

  virtual
  void transToPers (const Trans_t* trans, Pers_t* pers, MsgStream&) override
  {
    pers->m_v.clear();
    for (const X* x : *trans)
      pers->m_v.emplace_back (x->m_a * 20);
  }
};


class XContCnv_p2
  : public T_AthenaPoolTPCnvBase<XCont, XCont_p2>
{
public:
  virtual
  void persToTrans (const Pers_t* pers, Trans_t* trans, MsgStream&) override
  {
    trans->clear();
    for (const X_p2& x : pers->m_v)
      trans->push_back (new X (x.m_a / 10));
  }

  virtual
  void transToPers (const Trans_t* trans, Pers_t* pers, MsgStream&) override
  {
    pers->m_v.clear();
    for (const X* x : *trans)
      pers->m_v.emplace_back (x->m_a * 10);
  }
};


// Writing
void test1 (ISvcLocator* svcloc, TestCnvSvc& /*testsvc*/)
{
  std::cout << "test1\n";
  const size_t N = 10;

  T_AthenaPoolTPCnvCnv<XCont, XContCnv_p2, XContCnv_p1, T_TPCnvNull<XCont> >
    cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  XCont trans;
  for (size_t i = 0; i < N; i++)
    trans.push_back (new X(i));

  {
    XCont_p2* pers2 = cnv.createPersistentWithKey (&trans, "key");
    assert (pers2->m_v.size() == N);
    for (size_t i = 0; i < N; i++)
      assert (pers2->m_v[i].m_a == static_cast<int>(i*10));
    delete pers2;
  }
}


// Reading
void test2 (ISvcLocator* svcloc, TestCnvSvc& testsvc)
{
  std::cout << "test2\n";
  const size_t N = 10;
  T_AthenaPoolTPCnvCnv<XCont, XContCnv_p2, XContCnv_p1, T_TPCnvNull<XCont> >
    cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  XCont_p2 pers2;
  for (size_t i = 0; i < N; i++)
    pers2.m_v.push_back (X_p2 (i*50));

  testsvc.m_pers2 = &pers2;
  Token* token = new Token;
  token->setClassID (Guid (XCont_p2_guid));
  TokenAddress taddr (0, 0, "", "", 0, token);

  {
    DataObject* pObj = nullptr;
    assert (cnv.createObj (&taddr, pObj).isSuccess());
    auto* trans = SG::Storable_cast<XCont> (pObj);
    assert (trans->size() == N);
    for (size_t i = 0; i < N; i++)
      assert ((*trans)[i]->m_a == static_cast<int>(i*5));
    delete pObj;
  }
  testsvc.m_pers2 = nullptr;

  XCont_p1 pers1;
  for (size_t i = 0; i < N; i++)
    pers1.m_v.push_back (X_p1 (i*80));

  testsvc.m_pers1 = &pers1;
  token->setClassID (Guid (XCont_p1_guid));
  {
    DataObject* pObj = nullptr;
    assert (cnv.createObj (&taddr, pObj).isSuccess());
    auto* trans = SG::Storable_cast<XCont> (pObj);
    assert (trans->size() == N);
    for (size_t i = 0; i < N; i++)
      assert ((*trans)[i]->m_a == static_cast<int>(i*4));
    delete pObj;
  }
  testsvc.m_pers1 = nullptr;

  XCont pers0;
  for (size_t i = 0; i < N; i++)
    pers0.push_back (new X (i*11));

  testsvc.m_pers0 = &pers0;
  token->setClassID (Guid (XCont_guid));
  {
    DataObject* pObj = nullptr;
    assert (cnv.createObj (&taddr, pObj).isSuccess());
    auto* trans = SG::Storable_cast<XCont> (pObj);
    assert (trans->size() == N);
    for (size_t i = 0; i < N; i++)
      assert ((*trans)[i]->m_a == static_cast<int>(i*11));
    delete pObj;
  }

  token->setClassID (Guid ("8ACD1C53-D3C7-4FE5-9BC0-E388701DB8FA"));
  DataObject* pObj = nullptr;
  assert (cnv.createObj (&taddr, pObj).isFailure());

  testsvc.m_pers1 = nullptr;
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
