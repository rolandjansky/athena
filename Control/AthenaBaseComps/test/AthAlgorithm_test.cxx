/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthenaBaseComps/test/AthAlgorithm_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Test property handling for AthAlgorithm.
 */


#undef NDEBUG
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <cassert>
#include <iostream>


namespace AthenaBaseCompsTest {
class MyBase {};
class MyObj : public MyBase {};
}
CLASS_DEF (AthenaBaseCompsTest::MyBase, 293847296, 1)
CLASS_DEF (AthenaBaseCompsTest::MyObj, 293847295, 1)
SG_BASE (AthenaBaseCompsTest::MyObj, AthenaBaseCompsTest::MyBase);
using AthenaBaseCompsTest::MyObj;


class MyAlg
  : public AthAlgorithm
{
public:
  MyAlg (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

  virtual void declare(Gaudi::DataHandle& hnd) override;
  virtual void addDependency (const DataObjID& obj, const Gaudi::DataHandle::Mode& mode) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandle<MyObj> whandle;

  SG::ReadDecorHandleKey<MyObj> rdkey;
  SG::WriteDecorHandleKey<MyObj> wdkey;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
  std::vector<DataObjID> extra_inputs;
  std::vector<DataObjID> extra_outputs;

  Gaudi::Property< int > gp_int {this, "gp_int", 3, "doc for gp_int"};
  Gaudi::Property< SG::ReadHandleKey<MyObj> > 
     gp_rkey {this, "gp_rkey", "aaa_gp", "doc for gp_rkey"};

};


MyAlg::MyAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc)
{
  declareProperty ("rkey", rkey);
  declareProperty ("whandle", whandle);
  declareProperty ("rdkey", rdkey);
  declareProperty ("wdkey", wdkey);
}


StatusCode MyAlg::initialize()
{
  //ATH_CHECK( rkey.initialize() );
  //ATH_CHECK( whandle.initialize() );
  //ATH_CHECK( rdkey.initialize() );
  ATH_CHECK( wdkey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode MyAlg::execute()
{
  return StatusCode::SUCCESS;
}


void MyAlg::declare(Gaudi::DataHandle& hnd)
{
  if (hnd.mode() & Gaudi::DataHandle::Reader) 
    inputs.push_back( &hnd );
  if (hnd.mode() & Gaudi::DataHandle::Writer) 
    outputs.push_back( &hnd );
  AthAlgorithm::declare (hnd);
}


void MyAlg::addDependency (const DataObjID& obj, const Gaudi::DataHandle::Mode& mode)
{
  if (mode & Gaudi::DataHandle::Reader) 
    extra_inputs.push_back( obj );
  if (mode & Gaudi::DataHandle::Writer) 
    extra_outputs.push_back( obj );
  AthAlgorithm::addDependency (obj, mode);
}


void test1 (ISvcLocator* svcLoc)
{
  std::cout << "test1\n";

  MyAlg alg ("ralg", svcLoc);  alg.addRef();
  //assert (alg.setProperties().isSuccess());
  assert (alg.sysInitialize().isSuccess());

  assert (alg.rkey.clid() == 293847295);
  assert (alg.rkey.key() == "aaa");
  assert (alg.rkey.storeHandle().name() == "FooSvc");
  assert (alg.rkey.mode() == Gaudi::DataHandle::Reader);

  assert (alg.whandle.clid() == 293847295);
  assert (alg.whandle.key() == "eee");
  assert (alg.whandle.storeHandle().name() == "BarSvc");
  assert (alg.whandle.mode() == Gaudi::DataHandle::Writer);

  std::vector<std::string> inputKeys { "StoreGateSvc+aaa_gp", "FooSvc+aaa", "FooSvc+yyy.qqq" };
  assert (alg.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < alg.inputs.size(); i++) {
    // std::cout << "inp " << alg.inputs[i]->objKey() << " =?= " 
    //           << inputKeys[i] << "\n";
    assert (alg.inputs[i]->objKey() == inputKeys[i]);
  }

  std::vector<std::string> outputKeys { "BarSvc+eee", "StoreGateSvc+zzz.rrr" };
  assert (alg.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < alg.outputs.size(); i++) {
    // std::cout << "out " << alg.outputs[i]->objKey() << " =?= "
    //           << outputKeys[i] << "\n";
    assert (alg.outputs[i]->objKey() == outputKeys[i]);
  }

  std::vector<std::string> extraInputKeys { "StoreGateSvc+zzz" };
  assert (alg.extra_inputs.size() == extraInputKeys.size());
  for (size_t i = 0; i < alg.extra_inputs.size(); i++) {
    // std::cout << "extra inp " << alg.extra_inputs[i].key() << " =?= " 
    //           << extraInputKeys[i] << "\n";
    assert (alg.extra_inputs[i].key() == extraInputKeys[i]);
  }

  assert (alg.extra_outputs.empty() );

  IProxyDict* xdict = &*alg.evtStore();
  xdict = alg.evtStore()->hiveProxyDict();
  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);

  assert (alg.execute().isSuccess());

  DataObjIDColl exp = {
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "BarSvc+eee" },
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "StoreGateSvc+zzz.rrr" },
    { ClassID_traits<AthenaBaseCompsTest::MyBase>::ID(), "StoreGateSvc+zzz.rrr" },
    { ClassID_traits<AthenaBaseCompsTest::MyBase>::ID(), "BarSvc+eee" },
  };
  if (exp != alg.outputDataObjs()) {
    for (const DataObjID& o : alg.outputDataObjs()) {
      std::cout << "obj " << o.clid() << " " << o.key() << "\n";
    }
  }
}


class MyArrAlg : public AthAlgorithm
{
public:
  MyArrAlg (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode execute() override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::ReadHandleKeyArray<MyObj> rkeyarr;
  SG::WriteHandleKeyArray<MyObj> wkeyarr;
};


MyArrAlg::MyArrAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc)
{
  declareProperty ("rkey",    rkey);
  declareProperty ("wkey",    wkey);
  declareProperty ("rkeyarr", rkeyarr);
  declareProperty ("wkeyarr", wkeyarr);

  rkeyarr.emplace_back ("r1");
  wkeyarr.emplace_back ("w1");
}


StatusCode MyArrAlg::execute()
{
  return StatusCode::SUCCESS;
}


void comphandles (const std::vector<Gaudi::DataHandle*>& hvec,
                  std::vector<std::string> keys)
{
  std::vector<std::string> hkeys;
  for (const Gaudi::DataHandle* h : hvec) {
    if (h) hkeys.push_back (h->objKey());
  }

  std::sort (hkeys.begin(), hkeys.end());
  std::sort (keys.begin(), keys.end());
  if (keys != hkeys) {
    std::cout << "Handle list mismatch.\n";
    std::cout << "Expected: ";
    for (const std::string& s : keys) std::cout << s << " ";
    std::cout << "\n:";
    std::cout << "Got: ";
    for (const std::string& s : hkeys) std::cout << s << " ";
    std::cout << "\n:";
    std::abort();
  }
}


// Testing handle arrays.
void test2 (ISvcLocator* svcLoc)
{
  std::cout << "test2\n";

  MyArrAlg alg ("arralg", svcLoc);  alg.addRef();

  assert (alg.sysInitialize().isSuccess());

  comphandles (alg.inputHandles(),{"StoreGateSvc+raa", "StoreGateSvc+rbb", "StoreGateSvc+rcc", "StoreGateSvc+rdd", "StoreGateSvc+ree", "StoreGateSvc+rff", "StoreGateSvc+rrr"});
  comphandles (alg.outputHandles(),{"StoreGateSvc+waa", "StoreGateSvc+wbb", "StoreGateSvc+wcc", "StoreGateSvc+wdd", "StoreGateSvc+wee", "StoreGateSvc+wff", "StoreGateSvc+www"});

  // Test that circular dependency detection worksd.
  MyArrAlg alg2 ("arralg2", svcLoc);  alg2.addRef();
  assert (alg2.sysInitialize().isFailure());
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc)) {
    return 1;
  }

  test1 (svcLoc);
  test2 (svcLoc);
  return 0;
}
