/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
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
#include "SGTools/StorableConversions.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include "TSystem.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "TestThinningSvc.icc"
#include "TestCnvSvcBase.icc"
#include <iostream>
#include <cassert>
#include <vector>


using namespace AthenaPoolCnvSvcTest;


std::string YCont_v1_guid = "7E1826B9-3666-42B3-A2E7-C916BD10A5B8";
std::string YCont_v2_guid = "80C19103-FE9B-4227-8E48-8C0DB468F892";


class TestCnvSvc
  : public TestCnvSvcBase
{
public:
  TestCnvSvc (const std::string& name, ISvcLocator* svcloc)
    : TestCnvSvcBase (name, svcloc),
      m_pers (nullptr)
  {}

  virtual void setObjPtr(void*& obj, const Token* /*token*/) const override
  {
    obj = new YCont_v2_pers (*m_pers);
  }

  std::string m_name;
  YCont_v2_pers* m_pers;
};


void test1 (ISvcLocator* svcloc, TestCnvSvc& testsvc, DataVector<Y_v2>& vec)
{
  std::cout << "test1\n";

  T_AthenaPoolCnv<ViewVector<DataVector<Y_v2> > > cnv (svcloc);
  assert (cnv.initialize().isSuccess());

  ViewVector<DataVector<Y_v2> > view (SG::VIEW_ELEMENTS);
  size_t sz = vec.size();
  for (size_t i = 0; i < sz; i++)
    view.push_back (vec[sz-i-1]);

  YCont_v2_pers* pers = cnv.createPersistent (&view);
  assert (pers->size() == sz);
  for (size_t i = 0; i < sz; i++) {
    assert ((*pers)[i].dataID() == "vec");
    assert ((*pers)[i].index() == sz-i-1);
  }

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
  delete pObj;
  pObj = nullptr;
  delete pers;
  testsvc.m_pers = nullptr;

  YCont_v2_pers pers1;
  size_t ii = 0;
  for (size_t i = 0; i < sz; i++, ii+=2) {
    if (ii >= sz) ii = 1;
    pers1.emplace_back ("vec", ii);
  }
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
  delete trans2;

  token->setClassID (Guid ("79E2478D-C17F-45E9-848D-278240C2FED3"));
  assert (cnv.createObj (&taddr, pObj).isFailure());
}


// Test with thinning.
void test2 (ISvcLocator* svcloc, TestCnvSvc& /*testsvc*/, DataVector<Y_v2>& vec)
{
  std::cout << "test2\n";
  size_t sz = vec.size();

  TestThinningSvc thinsvc;
  TestThinningSvc::instance (&thinsvc, true);
  thinsvc.remap (&vec, 5, IThinningSvc::RemovedIdx);
  for (size_t i = 6; i < sz; i++)
    thinsvc.remap (&vec, i, i-1);

  T_AthenaPoolCnv<ViewVector<DataVector<Y_v2> > > cnv (svcloc);
  assert (cnv.initialize().isSuccess());

  ViewVector<DataVector<Y_v2> > view (SG::VIEW_ELEMENTS);
  for (size_t i = 0; i < sz; i++)
    view.push_back (vec[i]);

  YCont_v2_pers* pers = cnv.createPersistent (&view);
  assert (pers->size() == sz);
  for (size_t i = 0; i < sz; i++) {
    const ElementLink<DataVector<Y_v2> >& l = (*pers)[i];
    if (i == 5) {
      assert (l.key() == 0);
      assert (l.index() == 0);
    }
    else {
      assert (l.dataID() == "vec");
      if (i < 5)
        assert (l.index() == i);
      else
        assert (l.index() == i-1);
    }
  }
  delete pers;
}


DataVector<Y_v2>& makeVecs()
{
  auto vec = CxxUtils::make_unique<DataVector<Y_v2> >();
  for (size_t i = 0; i < 10; i++)
    vec->push_back (new Y_v2(i));
  DataVector<Y_v2>& ret = *vec.get();
  SGTest::store.record (vec.release(), "vec");
  return ret;
}


int main()
{
  SGTest::initTestStore();
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi("test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  TestCnvSvc* svc = new TestCnvSvc ("AthenaPoolCnvSvc", pSvcLoc);
  ISvcManager* mgr = dynamic_cast<ISvcManager*> (pSvcLoc);
  if (mgr->addService (svc).isFailure())
    std::abort();
  
  gSystem->Load("libAthenaPoolCnvSvcTestDict");
  DataVector<Y_v2>& vec = makeVecs();
  test1 (pSvcLoc, *svc, vec);
  test2 (pSvcLoc, *svc, vec);
}

