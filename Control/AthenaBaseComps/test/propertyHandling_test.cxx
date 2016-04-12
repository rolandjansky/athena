/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaBaseComps/test/propertyHandling_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Test property handling for VarHandle / VarHandleKey.
 */


#undef NDEBUG
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "TestTools/initGaudi.h"
#include <vector>
#include <cassert>
#include <iostream>


namespace AthenaBaseCompsTest {
class MyObj {};
}
CLASS_DEF (AthenaBaseCompsTest::MyObj, 293847295, 1)
using AthenaBaseCompsTest::MyObj;


class MyAthAlgorithm
  : public AthAlgorithm
{
public:
  MyAthAlgorithm (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode execute() override;

  virtual void declareInput(Gaudi::DataHandle* im) override;
  virtual void declareOutput(Gaudi::DataHandle* im) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::UpdateHandleKey<MyObj> ukey;

  SG::ReadHandle<MyObj> rhandle;
  SG::WriteHandle<MyObj> whandle;
  SG::UpdateHandle<MyObj> uhandle;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAthAlgorithm::MyAthAlgorithm  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc)
{
  declareProperty ("rkey", rkey);
  declareProperty ("wkey", wkey);
  declareProperty ("ukey", ukey);
  declareProperty ("rhandle", rhandle);
  declareProperty ("whandle", whandle);
  declareProperty ("uhandle", uhandle);
}


StatusCode MyAthAlgorithm::execute()
{
  return StatusCode::SUCCESS;
}


void MyAthAlgorithm::declareInput(Gaudi::DataHandle* im)
{
  inputs.push_back (im);
}


void MyAthAlgorithm::declareOutput(Gaudi::DataHandle* im)
{
  outputs.push_back (im);
}


class MyAthAlgTool
  : public AthAlgTool
{
public:
  MyAthAlgTool (const std::string& type,
                const std::string& name,
                IInterface* parent);

  virtual void declareInput(Gaudi::DataHandle* im) override;
  virtual void declareOutput(Gaudi::DataHandle* im) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::UpdateHandleKey<MyObj> ukey;

  SG::ReadHandle<MyObj> rhandle;
  SG::WriteHandle<MyObj> whandle;
  SG::UpdateHandle<MyObj> uhandle;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAthAlgTool::MyAthAlgTool  (const std::string& type,
                             const std::string& name,
                             IInterface* parent)
  : AthAlgTool (type, name, parent)
{
  declareProperty ("rkey", rkey);
  declareProperty ("wkey", wkey);
  declareProperty ("ukey", ukey);
  declareProperty ("rhandle", rhandle);
  declareProperty ("whandle", whandle);
  declareProperty ("uhandle", uhandle);
}


void MyAthAlgTool::declareInput(Gaudi::DataHandle* im)
{
  inputs.push_back (im);
}


void MyAthAlgTool::declareOutput(Gaudi::DataHandle* im)
{
  outputs.push_back (im);
}


void test1 (ISvcLocator* svcLoc)
{
  std::cout << "test1\n";

  MyAthAlgorithm alg ("alg", svcLoc);  alg.addRef();
  assert (alg.setProperties().isSuccess());

  assert (alg.rkey.clid() == 293847295);
  assert (alg.rkey.key() == "aaa");
  assert (alg.rkey.storeHandle().name() == "FooSvc");
  assert (alg.rkey.mode() == Gaudi::DataHandle::Reader);

  assert (alg.wkey.clid() == 293847295);
  assert (alg.wkey.key() == "bbb");
  assert (alg.wkey.storeHandle().name() == "BarSvc");
  assert (alg.wkey.mode() == Gaudi::DataHandle::Writer);

  assert (alg.ukey.clid() == 293847295);
  assert (alg.ukey.key() == "ccc");
  assert (alg.ukey.storeHandle().name() == "StoreGateSvc");
  assert (alg.ukey.mode() == Gaudi::DataHandle::Updater);

  assert (alg.rhandle.clid() == 293847295);
  assert (alg.rhandle.key() == "ddd");
  assert (alg.rhandle.storeHandle().name() == "FooSvc");
  assert (alg.rhandle.mode() == Gaudi::DataHandle::Reader);

  assert (alg.whandle.clid() == 293847295);
  assert (alg.whandle.key() == "eee");
  assert (alg.whandle.storeHandle().name() == "BarSvc");
  assert (alg.whandle.mode() == Gaudi::DataHandle::Writer);

  assert (alg.uhandle.clid() == 293847295);
  assert (alg.uhandle.key() == "fff");
  assert (alg.uhandle.storeHandle().name() == "StoreGateSvc");
  assert (alg.uhandle.mode() == Gaudi::DataHandle::Updater);

  std::vector<std::string> inputKeys { "aaa", "ccc", "ddd", "fff" };
  assert (alg.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < inputKeys.size(); i++)
    assert (alg.inputs[i]->objKey() == inputKeys[i]);

  std::vector<std::string> outputKeys { "bbb", "ccc", "eee", "fff" };
  assert (alg.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < outputKeys.size(); i++)
    assert (alg.outputs[i]->objKey() == outputKeys[i]);
}


void test2 (ISvcLocator* svcLoc)
{
  std::cout << "test2\n";

  MyAthAlgorithm alg ("alg", svcLoc);  alg.addRef();
  MyAthAlgTool tool ("MyAthAlgTool", "tool", &alg);   tool.addRef();
  assert (tool.setProperties().isSuccess());

  assert (tool.rkey.clid() == 293847295);
  assert (tool.rkey.key() == "taa");
  assert (tool.rkey.storeHandle().name() == "FooSvc");
  assert (tool.rkey.mode() == Gaudi::DataHandle::Reader);

  assert (tool.wkey.clid() == 293847295);
  assert (tool.wkey.key() == "tbb");
  assert (tool.wkey.storeHandle().name() == "BarSvc");
  assert (tool.wkey.mode() == Gaudi::DataHandle::Writer);

  assert (tool.ukey.clid() == 293847295);
  assert (tool.ukey.key() == "tcc");
  assert (tool.ukey.storeHandle().name() == "StoreGateSvc");
  assert (tool.ukey.mode() == Gaudi::DataHandle::Updater);


  assert (tool.rhandle.clid() == 293847295);
  assert (tool.rhandle.key() == "tdd");
  assert (tool.rhandle.storeHandle().name() == "FooSvc");
  assert (tool.rhandle.mode() == Gaudi::DataHandle::Reader);

  assert (tool.whandle.clid() == 293847295);
  assert (tool.whandle.key() == "tee");
  assert (tool.whandle.storeHandle().name() == "BarSvc");
  assert (tool.whandle.mode() == Gaudi::DataHandle::Writer);

  assert (tool.uhandle.clid() == 293847295);
  assert (tool.uhandle.key() == "tff");
  assert (tool.uhandle.storeHandle().name() == "StoreGateSvc");
  assert (tool.uhandle.mode() == Gaudi::DataHandle::Updater);

  std::vector<std::string> inputKeys { "taa", "tcc", "tdd", "tff" };
  assert (tool.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < inputKeys.size(); i++)
    assert (tool.inputs[i]->objKey() == inputKeys[i]);

  std::vector<std::string> outputKeys { "tbb", "tcc", "tee", "tff" };
  assert (tool.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < outputKeys.size(); i++)
    assert (tool.outputs[i]->objKey() == outputKeys[i]);
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc);

  test1 (svcLoc);
  test2 (svcLoc);
  return 0;
}
