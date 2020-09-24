/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/test/T_AthenaPoolViewVectorCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for T_AthenaPoolViewVectorCnv_test.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "PersistentDataModel/TokenAddress.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/StorableConversions.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "TestCnvSvcBase.icc"
#include "TSystem.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <vector>


using namespace AthenaPoolCnvSvcTest;


const std::string YCont_v1_guid2 = "7E1826B9-3666-42B3-A2E7-C916BD10A5B8";
const std::string YCont_v2_guid2 = "80C19103-FE9B-4227-8E48-8C0DB468F892";
const std::string YCont_v1_guid = "C5454482-92F6-4EE5-BBAB-44B36D571942";
const std::string YCont_v2_guid = "CA993B46-C358-4F5E-9EE2-3693A8F3CDAF";


class ViewVectorBaseTest
{
public:
  static void check (const SG::ViewVectorBase& vvb, SG::sgkey_t sgkey);
  static void checkPersThinned (const SG::ViewVectorBase& vvb, SG::sgkey_t sgkey);
  static void checkPersEmpty (const SG::ViewVectorBase& vvb);
};


void ViewVectorBaseTest::check (const SG::ViewVectorBase& vvb,
                                SG::sgkey_t sgkey)
{
  assert (vvb.m_persKey.size() == 10);
  assert (vvb.m_persIndex.size() == 10);
  for (unsigned int i = 0; i < 10; i++) {
    assert  (vvb.m_persKey[i] == sgkey);
    assert  (vvb.m_persIndex[i] == 9-i);
  }
}


void ViewVectorBaseTest::checkPersThinned (const SG::ViewVectorBase& vvb,
                                           SG::sgkey_t sgkey)
{
  size_t sz = 10;
  assert (vvb.m_persKey.size() == sz);
  assert (vvb.m_persIndex.size() == sz);
  for (size_t i = 0; i < sz; i++) {
    SG::sgkey_t pkey = vvb.m_persKey[i];
    size_t pindex = vvb.m_persIndex[i];
    if (i == 5) {
      assert (pkey == 0);
      assert (pindex == 0);
    }
    else {
      assert (pkey == sgkey);
      if (i < 5)
        assert (pindex == i);
      else
        assert (pindex == i-1);
    }
  }
}


void ViewVectorBaseTest::checkPersEmpty (const SG::ViewVectorBase& vvb)
{
  assert (vvb.m_persKey.empty());
  assert (vvb.m_persIndex.empty());
}


class TestCnvSvc
  : public TestCnvSvcBase
{
public:
  TestCnvSvc (const std::string& name, ISvcLocator* svcloc)
    : TestCnvSvcBase (name, svcloc),
      m_pers (nullptr),
      m_pers_old (nullptr)
  {}

  virtual void setObjPtr(void*& obj, const Token* token) const override
  {
    if (m_pers) {
      auto vvb = new ViewVector<DataVector<Y_v2> > (*m_pers);
      if (token->classID() == Guid(YCont_v2_guid))
        vvb->toTransient();
      obj = vvb;
    }
    else if (m_pers_old) {
      obj = new YCont_v2_pers2 (*m_pers_old);
    }
  }

  std::string m_name;
  ViewVector<DataVector<Y_v2> >* m_pers;
  YCont_v2_pers2* m_pers_old;
};


void test1 (ISvcLocator* svcloc,
            SGTest::TestStore& store,
            TestCnvSvc& testsvc, DataVector<Y_v2>& vec)
{
  std::cout << "test1\n";
  SG::sgkey_t sgkey =
    store.stringToKey ("vec", ClassID_traits<DataVector<Y_v2> >::ID());
  T_AthenaPoolCnv<ViewVector<DataVector<Y_v2> > > cnv (svcloc);
  assert (cnv.initialize().isSuccess());

  ViewVector<DataVector<Y_v2> > view (SG::VIEW_ELEMENTS);
  size_t sz = vec.size();
  for (size_t i = 0; i < sz; i++)
    view.push_back (vec[sz-i-1]);

  ViewVector<DataVector<Y_v2> >* pers = cnv.createPersistent (&view);
  pers->toPersistent();
  ViewVectorBaseTest::check (*pers,  sgkey);
  assert (pers->size() == 0);

  testsvc.m_pers = pers;
  Token* token = new Token;
  token->setClassID (Guid (YCont_v2_guid));
  TokenAddress taddr (0, 0, "", "", 0, token);

  DataObject* pObj;
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans1 = SG::Storable_cast<ViewVector<DataVector<Y_v2> > > (pObj);
  assert (trans1->size() == 10);
  for (size_t i = 0; i < sz; i++)
    assert ((*trans1)[i] == view[i]);
  ViewVectorBaseTest::checkPersEmpty (*trans1);
  delete pObj;
  pObj = nullptr;
  delete pers;
  testsvc.m_pers = nullptr;

  ViewVector<DataVector<Y_v2> > pers1;
  size_t ii = 0;
  for (size_t i = 0; i < sz; i++, ii+=2) {
    if (ii >= sz) ii = 1;
    pers1.push_back (vec[ii]);
  }
  pers1.setClearOnPersistent();
  pers1.toPersistent();
  testsvc.m_pers = &pers1;
  token->setClassID (Guid (YCont_v1_guid));
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans2 = SG::Storable_cast<ViewVector<DataVector<Y_v2> > > (pObj);
  assert (trans2->size() == 10);
  ii = 0;
  for (size_t i = 0; i < sz; i++, ii+=2) {
    if (ii >= sz) ii = 1;
    assert ((*trans2)[i] == vec[ii]);
  }
  ViewVectorBaseTest::checkPersEmpty (*trans2);
  delete trans2;

  token->setClassID (Guid ("79E2478D-C17F-45E9-848D-278240C2FED3"));
  assert (cnv.createObj (&taddr, pObj).isFailure());
}


// Test reading std::vector<EL>
void test2 (ISvcLocator* svcloc, TestCnvSvc& testsvc, DataVector<Y_v2>& vec)
{
  std::cout << "test2\n";
  T_AthenaPoolCnv<ViewVector<DataVector<Y_v2> > > cnv (svcloc);
  assert (cnv.initialize().isSuccess());
  size_t sz = vec.size();

  std::vector<ElementLink<DataVector<Y_v2> > > pers2;
  size_t ii = 0;
  for (size_t i = 0; i < sz; i++, ii+=2) {
    if (ii >= sz) ii = 1;
    pers2.emplace_back ("vec", ii);
  }

  testsvc.m_pers = nullptr;
  testsvc.m_pers_old = &pers2;
  Token* token = new Token;
  token->setClassID (Guid (YCont_v2_guid2));
  TokenAddress taddr (0, 0, "", "", 0, token);

  DataObject* pObj;
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans2 = SG::Storable_cast<ViewVector<DataVector<Y_v2> > > (pObj);
  assert (trans2->size() == 10);
  ii = 0;
  for (size_t i = 0; i < sz; i++, ii+=2) {
    if (ii >= sz) ii = 1;
    assert ((*trans2)[i] == vec[ii]);
  }
  ViewVectorBaseTest::checkPersEmpty (*trans2);
  delete pObj;
  pObj = nullptr;

  std::vector<ElementLink<DataVector<Y_v2> > > pers1;
  for (size_t i = 0; i < sz; i++)
    pers1.emplace_back ("vec", i);
  testsvc.m_pers_old = &pers1;
  token->setClassID (Guid (YCont_v1_guid2));
  assert (cnv.createObj (&taddr, pObj).isSuccess());
  auto* trans1 = SG::Storable_cast<ViewVector<DataVector<Y_v2> > > (pObj);
  assert (trans1->size() == 10);
  for (size_t i = 0; i < sz; i++)
    assert ((*trans1)[i] == vec[i]);
  ViewVectorBaseTest::checkPersEmpty (*trans1);
  delete pObj;
  pObj = nullptr;
  testsvc.m_pers_old = nullptr;
}


DataVector<Y_v2>& makeVecs (SGTest::TestStore& store)
{
  auto vec = std::make_unique<DataVector<Y_v2> >();
  for (size_t i = 0; i < 10; i++)
    vec->push_back (new Y_v2(i));
  DataVector<Y_v2>& ret = *vec.get();
  store.record (vec.release(), "vec");
  return ret;
}


int main()
{
  CxxUtils::ubsan_suppress ([]() {TInterpreter::Instance(); });
  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi("AthenaPoolCnvSvc/test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  TestCnvSvc* svc = new TestCnvSvc ("AthenaPoolCnvSvc", pSvcLoc);
  ISvcManager* mgr = dynamic_cast<ISvcManager*> (pSvcLoc);
  if (!mgr) std::abort();
  if (mgr->addService (svc).isFailure())
    std::abort();
  
  gSystem->Load("libAthenaPoolCnvSvcTestDict");
  DataVector<Y_v2>& vec = makeVecs (*testStore);
  test1 (pSvcLoc, *testStore, *svc, vec);
  test2 (pSvcLoc, *svc, vec);
}

