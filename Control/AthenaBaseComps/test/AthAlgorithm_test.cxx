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
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
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

  virtual StatusCode execute() override;

  virtual void declare(Gaudi::DataHandle& hnd) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandle<MyObj> whandle;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAlg::MyAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc)
{
  declareProperty ("rkey", rkey);
  declareProperty ("whandle", whandle);
}


StatusCode MyAlg::execute()
{
  return StatusCode::SUCCESS;
}


void MyAlg::declare(Gaudi::DataHandle& hnd) {
  if (hnd.mode() & Gaudi::DataHandle::Reader) 
    inputs.push_back( &hnd );
  if (hnd.mode() & Gaudi::DataHandle::Writer) 
    outputs.push_back( &hnd );
  AthAlgorithm::declare (hnd);
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

  std::vector<std::string> inputKeys { "aaa"  };
  assert (alg.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < inputKeys.size(); i++)
    assert (alg.inputs[i]->objKey() == inputKeys[i]);

  std::vector<std::string> outputKeys { "eee" };
  assert (alg.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < outputKeys.size(); i++)
    assert (alg.outputs[i]->objKey() == outputKeys[i]);

  IProxyDict* xdict = &*alg.evtStore();
  xdict = alg.evtStore()->hiveProxyDict();
  EventContext ctx;
  ctx.setProxy (xdict);
  Gaudi::Hive::setCurrentContext (ctx);

  assert (alg.execute().isSuccess());

  DataObjIDColl exp = {
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "eee" },
    { ClassID_traits<AthenaBaseCompsTest::MyBase>::ID(), "eee" },
  };
  if (exp != alg.outputDataObjs()) {
    for (const DataObjID& o : alg.outputDataObjs()) {
      std::cout << "obj " << o.clid() << " " << o.key() << "\n";
    }
  }
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc);

  test1 (svcLoc);
  return 0;
}
