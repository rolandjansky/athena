/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolCnvSvc/test/T_AthenaPoolAuxContainerCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for T_AthenaPoolAuxContainerCnv_test.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "PersistentDataModel/TokenAddress.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/ubsan_suppress.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ClassID_traits.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TestCnvSvcBase.icc"
#include "TSystem.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <vector>


using namespace AthenaPoolCnvSvcTest;


const std::string YAuxCont_v1_guid = "BEE3C14E-149E-4366-9E24-8A32C419A3B4";
const std::string YAuxCont_v2_guid = "170BFEE4-F6B2-4AF4-919B-7EB3986656FA";


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
      obj = new YAuxCont_v2 (*m_pers2);
    }
    else if (m_pers1) {
      obj = new YAuxCont_v1 (*m_pers1);
    }
    else
      std::abort();
  }

  std::string m_name;
  YAuxCont_v1* m_pers1;
  YAuxCont_v2* m_pers2;
};


class YAuxContCnv_v1
{
public:
  typedef YAuxCont_v2 Trans_t;
  typedef YAuxCont_v1 Pers_t;
  
  Trans_t* createTransient (const Pers_t* pers, MsgStream& msg)
  {
    auto trans = std::make_unique<Trans_t>();
    persToTrans (pers, trans.get(), msg);
    return trans.release();
  }

  void persToTrans (const Pers_t* pers, Trans_t* trans, MsgStream&)
  {
    typedef ElementLink<DataVector<Y_v2> > Link_t;
    SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
    SG::auxid_t x_auxid = reg.getAuxID<int> ("x");
    SG::auxid_t l_auxid = reg.getAuxID<Link_t> ("l");

    size_t sz = pers->size();

    int* xt = reinterpret_cast<int*> (trans->getData (x_auxid, sz, sz));
    Link_t* lt = reinterpret_cast<Link_t*> (trans->getData (l_auxid, sz, sz));

    const int* xp = reinterpret_cast<const int*> (pers->getData (x_auxid));
    const Link_t* lp = reinterpret_cast<const Link_t*> (pers->getData (l_auxid));

    for (size_t i = 0; i < sz; i++) {
      lt[i] = lp[i];
      xt[i] = xp[i]*3;
    }
  }
};


void test1_checkThinned (const YAuxCont_v2* pers,
                         const size_t N,
                         const SG::auxid_t x_auxid,
                         const SG::auxid_t l_auxid)
{
  typedef ElementLink<DataVector<Y_v2> > Link_t;

  assert (pers->size() == N-1);
  const int* x = reinterpret_cast<const int*> (pers->getData (x_auxid));
  const Link_t* l = reinterpret_cast<const Link_t*> (pers->getData (l_auxid));
  for (size_t i = 0; i < N-1; i++) {
    size_t ii = i;
    if (i >= 7) ii = i+1;
    assert (x[i] == static_cast<int>(ii)*2);
    if (ii == 5) {
      assert (l[i].key() == 0);
      assert (l[i].index() == 0);
    }
    else {
      assert (l[i].dataID() == "vec");
      if (i < 5)
        assert (l[i].index() == ii);
      else
        assert (l[i].index() == ii-1);
    }
  }
}


// Writing
void test1 (ISvcLocator* svcloc,
            SGTest::TestStore& store,
            TestCnvSvc& /*testsvc*/)
{
  std::cout << "test1\n";
  const size_t N = 10;

  DataVector<Y_v2>* vec = new DataVector<Y_v2>;
  for (size_t i = 0; i < N; i++)
    vec->push_back (new Y_v2(i));
  store.record (vec, "vec");

  T_AthenaPoolAuxContainerCnv<YAuxCont_v2, YAuxContCnv_v1> cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  typedef ElementLink<DataVector<Y_v2> > Link_t;
  SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  SG::auxid_t x_auxid = reg.getAuxID<int> ("x");
  SG::auxid_t l_auxid = reg.getAuxID<Link_t> ("l");
  YAuxCont_v2* trans2 = new YAuxCont_v2;
  int* x2 = reinterpret_cast<int*> (trans2->getData (x_auxid, N, N));
  Link_t* l2 = reinterpret_cast<Link_t*> (trans2->getData (l_auxid, N, N));
  for (size_t i = 0; i < N; i++) {
    x2[i] = i*2;
    l2[i] = Link_t ("vec", i);
  }
  store.record (trans2, "store");

  YAuxCont_v2* pers2a = cnv.createPersistentWithKey (trans2, "store");
  assert (pers2a->size() == trans2->size());
  const int* x2a = reinterpret_cast<const int*> (pers2a->getData (x_auxid));
  const Link_t* l2a = reinterpret_cast<const Link_t*> (pers2a->getData (l_auxid));
  for (size_t i = 0; i < N; i++) {
    assert (x2a[i] == static_cast<int>(i)*2);
    assert (l2a[i].dataID() == "vec");
    assert (l2a[i].index() == i);
  }

  SG::sgkey_t sgkey_store =
    store.stringToKey ("store",
                       ClassID_traits<YAuxCont_v2>::ID());
  SG::sgkey_t sgkey_vec =
    store.stringToKey ("vec",
                       ClassID_traits<DataVector<Y_v2> >::ID());

  SG::ThinningCache cache;
  SG::ThinningDecisionBase dec_store;
  dec_store.resize (N);
  dec_store.thin (7);
  dec_store.buildIndexMap();
  cache.addThinning ("store",
                     std::vector<SG::sgkey_t> { sgkey_store },
                     &dec_store);

  SG::ThinningDecisionBase dec_vec;
  dec_vec.resize (N);
  dec_vec.thin (5);
  dec_vec.buildIndexMap();
  cache.addThinning ("vec",
                     std::vector<SG::sgkey_t> { sgkey_vec },
                     &dec_vec);
  EventContext ctx;
  Atlas::ExtendedEventContext ectx;
  ectx.setThinningCache (&cache);
  Atlas::setExtendedEventContext (ctx, std::move (ectx));
  Gaudi::Hive::setCurrentContext (ctx);

  YAuxCont_v2* pers2c = cnv.createPersistentWithKey (trans2, "store");
  test1_checkThinned (pers2c, N, x_auxid, l_auxid);

  ectx.setThinningCache (nullptr);
  Atlas::setExtendedEventContext (ctx, std::move (ectx));
  Gaudi::Hive::setCurrentContext (ctx);

  delete trans2;
  delete pers2a;
  delete pers2c;
}


// Reading
void test2 (ISvcLocator* svcloc, TestCnvSvc& testsvc)
{
  std::cout << "test2\n";
  const size_t N = 10;

  T_AthenaPoolAuxContainerCnv<YAuxCont_v2, YAuxContCnv_v1> cnv (svcloc);
  assert (static_cast<AthenaPoolConverter&>(cnv).initialize().isSuccess());

  typedef ElementLink<DataVector<Y_v2> > Link_t;
  SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  SG::auxid_t x_auxid = reg.getAuxID<int> ("x");
  SG::auxid_t l_auxid = reg.getAuxID<Link_t> ("l");
  YAuxCont_v2 pers2;
  int* x2 = reinterpret_cast<int*> (pers2.getData (x_auxid, N, N));
  Link_t* l2 = reinterpret_cast<Link_t*> (pers2.getData (l_auxid, N, N));
  for (size_t i = 0; i < N; i++) {
    x2[i] = i*2;
    l2[i] = Link_t ("vec", i);
  }

  testsvc.m_pers2 = &pers2;
  Token* token = new Token;
  token->setClassID (Guid (YAuxCont_v2_guid));
  TokenAddress taddr (0, 0, "", "", 0, token);

  {
    DataObject* pObj = nullptr;
    assert (cnv.createObj (&taddr, pObj).isSuccess());
    auto* trans = SG::Storable_cast<YAuxCont_v2> (pObj);
    assert (trans->size() == pers2.size());
    const int* xt = reinterpret_cast<const int*> (trans->getData (x_auxid));
    const Link_t* lt = reinterpret_cast<const Link_t*> (trans->getData (l_auxid));
    for (size_t i = 0; i < N; i++) {
      assert (xt[i] == static_cast<int>(i)*2);
      assert (lt[i].dataID() == "vec");
      assert (lt[i].index() == i);
    }

    delete pObj;
  }
  testsvc.m_pers2 = nullptr;

  YAuxCont_v1 pers1;
  int* x1 = reinterpret_cast<int*> (pers1.getData (x_auxid, N, N));
  Link_t* l1 = reinterpret_cast<Link_t*> (pers1.getData (l_auxid, N, N));
  for (size_t i = 0; i < N; i++) {
    x1[i] = i*4;
    l1[i] = Link_t ("vec", i);
  }

  testsvc.m_pers1 = &pers1;
  token->setClassID (Guid (YAuxCont_v1_guid));
  {
    DataObject* pObj = nullptr;
    assert (cnv.createObj (&taddr, pObj).isSuccess());
    auto* trans = SG::Storable_cast<YAuxCont_v2> (pObj);
    assert (trans->size() == pers2.size());
    const int* xt = reinterpret_cast<const int*> (trans->getData (x_auxid));
    const Link_t* lt = reinterpret_cast<const Link_t*> (trans->getData (l_auxid));
    for (size_t i = 0; i < N; i++) {
      assert (xt[i] == static_cast<int>(i)*3*4);
      assert (lt[i].dataID() == "vec");
      assert (lt[i].index() == i);
    }

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

  test1 (pSvcLoc, *testStore, *svc);
  test2 (pSvcLoc, *svc);
  return 0;
}
