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
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "TestTools/initGaudi.h"
#include <vector>
#include <cassert>
#include <iostream>


namespace AthenaBaseCompsTest {
class MyObj
{
public:
  typedef std::true_type thread_safe;
};
}
CLASS_DEF (AthenaBaseCompsTest::MyObj, 293847295, 1)
using AthenaBaseCompsTest::MyObj;


class MyAthAlgorithm
  : public AthAlgorithm
{
public:
  MyAthAlgorithm (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode execute() override;

  virtual void declare(Gaudi::DataHandle& hnd) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::UpdateHandleKey<MyObj> ukey;

  SG::ReadHandle<MyObj> rhandle;
  SG::WriteHandle<MyObj> whandle;
  SG::UpdateHandle<MyObj> uhandle;

  SG::ReadDecorHandleKey<MyObj> rdkey;
  SG::WriteDecorHandleKey<MyObj> wdkey;

  SG::ReadCondHandleKey<MyObj> rckey;
  SG::WriteCondHandleKey<MyObj> wckey;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAthAlgorithm::MyAthAlgorithm  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc),
    rckey ("rc"),
    wckey ("wc", "dbkey")
{
  declareProperty ("rkey", rkey);
  declareProperty ("wkey", wkey);
  declareProperty ("ukey", ukey);
  declareProperty ("rhandle", rhandle);
  declareProperty ("whandle", whandle);
  declareProperty ("uhandle", uhandle);
  declareProperty ("rdkey", rdkey);
  declareProperty ("wdkey", wdkey);
  declareProperty ("rckey", rckey);
  declareProperty ("wckey", wckey);
}


StatusCode MyAthAlgorithm::execute()
{
  return StatusCode::SUCCESS;
}

void MyAthAlgorithm::declare(Gaudi::DataHandle& hnd) {
  if (hnd.mode() & Gaudi::DataHandle::Reader) 
    inputs.push_back( &hnd );
  if (hnd.mode() & Gaudi::DataHandle::Writer) 
    outputs.push_back( &hnd );
}


class MyAthAlgTool
  : public AthAlgTool
{
public:
  MyAthAlgTool (const std::string& type,
                const std::string& name,
                IInterface* parent);

  virtual void declare(Gaudi::DataHandle& hnd) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::UpdateHandleKey<MyObj> ukey;

  SG::ReadHandle<MyObj> rhandle;
  SG::WriteHandle<MyObj> whandle;
  SG::UpdateHandle<MyObj> uhandle;

  SG::ReadDecorHandleKey<MyObj> rdkey;
  SG::WriteDecorHandleKey<MyObj> wdkey;

  SG::ReadCondHandleKey<MyObj> rckey;
  SG::WriteCondHandleKey<MyObj> wckey;

  std::vector<Gaudi::DataHandle*> inputs;
  std::vector<Gaudi::DataHandle*> outputs;
};


MyAthAlgTool::MyAthAlgTool  (const std::string& type,
                             const std::string& name,
                             IInterface* parent)
  : AthAlgTool (type, name, parent),
    rckey ("rc"),
    wckey ("wc", "dbkey")
{
  declareProperty ("rkey", rkey);
  declareProperty ("wkey", wkey);
  declareProperty ("ukey", ukey);
  declareProperty ("rhandle", rhandle);
  declareProperty ("whandle", whandle);
  declareProperty ("uhandle", uhandle);
  declareProperty ("rdkey", rdkey = "aaa.bbb");
  declareProperty ("wdkey", wdkey);
  declareProperty ("rckey", rckey);
  declareProperty ("wckey", wckey);
}


void MyAthAlgTool::declare(Gaudi::DataHandle& hnd) {
  if (hnd.mode() & Gaudi::DataHandle::Reader) 
    inputs.push_back( &hnd );
  if (hnd.mode() & Gaudi::DataHandle::Writer) 
    outputs.push_back( &hnd );
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

  assert (alg.rdkey.clid() == 293847295);
  assert (alg.rdkey.key() == "ggg.qqq");
  assert (alg.rdkey.storeHandle().name() == "FooSvc");
  assert (alg.rdkey.mode() == Gaudi::DataHandle::Reader);

  assert (alg.wdkey.clid() == 293847295);
  assert (alg.wdkey.key() == "hhh.rrr");
  assert (alg.wdkey.storeHandle().name() == "BarSvc");
  assert (alg.wdkey.mode() == Gaudi::DataHandle::Writer);
  assert (alg.wdkey.contHandleKey().clid() == 293847295);
  assert (alg.wdkey.contHandleKey().key() == "hhh");
  assert (alg.wdkey.contHandleKey().storeHandle().name() == "BarSvc");
  assert (alg.wdkey.contHandleKey().mode() == Gaudi::DataHandle::Reader);

  assert (alg.rckey.clid() == 293847295);
  assert (alg.rckey.key() == "iii");
  assert (alg.rckey.mode() == Gaudi::DataHandle::Reader);

  assert (alg.wckey.clid() == 293847295);
  assert (alg.wckey.key() == "jjj");
  assert (alg.wckey.mode() == Gaudi::DataHandle::Writer);

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

  std::vector<std::string> inputKeys { "aaa", "ccc", "ddd", "fff",
                                       "ggg.qqq", "hhh", "iii" };
  assert (alg.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < alg.inputs.size(); i++) {
    //std::cout << "inp " << alg.inputs[i]->objKey() << "\n";
    assert (alg.inputs[i]->objKey() == inputKeys[i]);
  }

  std::vector<std::string> outputKeys { "bbb", "ccc", "eee", "fff",
                                        "hhh.rrr", "jjj" };
  assert (alg.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < alg.outputs.size(); i++) {
    //std::cout << "out " << alg.outputs[i]->objKey() << "\n";
    assert (alg.outputs[i]->objKey() == outputKeys[i]);
  }
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

  assert (tool.rdkey.clid() == 293847295);
  assert (tool.rdkey.key() == "tgg.qqq");
  assert (tool.rdkey.storeHandle().name() == "FooSvc");
  assert (tool.rdkey.mode() == Gaudi::DataHandle::Reader);

  assert (tool.wdkey.clid() == 293847295);
  assert (tool.wdkey.key() == "thh.rrr");
  assert (tool.wdkey.storeHandle().name() == "BarSvc");
  assert (tool.wdkey.mode() == Gaudi::DataHandle::Writer);
  assert (tool.wdkey.contHandleKey().clid() == 293847295);
  assert (tool.wdkey.contHandleKey().key() == "thh");
  assert (tool.wdkey.contHandleKey().storeHandle().name() == "BarSvc");
  assert (tool.wdkey.contHandleKey().mode() == Gaudi::DataHandle::Reader);

  assert (tool.rckey.clid() == 293847295);
  assert (tool.rckey.key() == "tii");
  assert (tool.rckey.mode() == Gaudi::DataHandle::Reader);

  assert (tool.wckey.clid() == 293847295);
  assert (tool.wckey.key() == "tjj");
  assert (tool.wckey.mode() == Gaudi::DataHandle::Writer);

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

  std::vector<std::string> inputKeys { "taa", "tcc", "tdd", "tff",
                                       "tgg.qqq", "thh", "tii" };
  assert (tool.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < tool.inputs.size(); i++) {
    //std::cout << "inp " << tool.inputs[i]->objKey() << "\n";
    assert (tool.inputs[i]->objKey() == inputKeys[i]);
  }

  std::vector<std::string> outputKeys { "tbb", "tcc", "tee", "tff",
                                        "thh.rrr", "tjj" };
  assert (tool.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < tool.outputs.size(); i++) {
    //std::cout << "out " << tool.outputs[i]->objKey() << "\n";
    assert (tool.outputs[i]->objKey() == outputKeys[i]);
  }
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc);

  test1 (svcLoc);
  test2 (svcLoc);
  return 0;
}
