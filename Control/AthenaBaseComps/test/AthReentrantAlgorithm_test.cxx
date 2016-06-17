/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaBaseComps/test/AthReentrantAlgorithm_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Test property handling for AthReentrantAlgorithm.
 */


#undef NDEBUG
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <cassert>
#include <iostream>


namespace AthenaBaseCompsTest {
class MyObj {};
}
CLASS_DEF (AthenaBaseCompsTest::MyObj, 293847295, 1)
using AthenaBaseCompsTest::MyObj;


IProxyDict* pdict = nullptr;


class MyAlg
  : public AthReentrantAlgorithm
{
public:
  MyAlg (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode execute_r (const EventContext& ctx) const override;

  virtual void declareInput(Gaudi::DataHandle* im) override;
  virtual void declareOutput(Gaudi::DataHandle* im) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandle<MyObj> whandle;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAlg::MyAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthReentrantAlgorithm (name, svcLoc)
{
  declareProperty ("rkey", rkey);
  declareProperty ("whandle", whandle);
}


StatusCode MyAlg::execute_r (const EventContext& ctx) const
{
  pdict = ctx.proxy();
  return StatusCode::SUCCESS;
}


void MyAlg::declareInput(Gaudi::DataHandle* im)
{
  inputs.push_back (im);
}


void MyAlg::declareOutput(Gaudi::DataHandle* im)
{
  outputs.push_back (im);
}


void test1 (ISvcLocator* svcLoc)
{
  std::cout << "test1\n";

  MyAlg alg ("ralg", svcLoc);  alg.addRef();
  assert (alg.setProperties().isSuccess());

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
#ifdef ATHENAHIVE
  xdict = alg.evtStore()->hiveProxyDict();
#endif
  EventContext ctx;
  ctx.setProxy (xdict);
  Gaudi::Hive::setCurrentContext (ctx);

  assert (alg.execute().isSuccess());
  assert (pdict == xdict);
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc);

  test1 (svcLoc);
  return 0;
}
