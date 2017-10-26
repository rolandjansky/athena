/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthenaBaseComps/test/AthAlgTool_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Test property handling for AthAlgTool.
 */


#undef NDEBUG
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TestTools/initGaudi.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
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
};


MyAlg::MyAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm (name, svcLoc)
{
}


StatusCode MyAlg::execute()
{
  return StatusCode::SUCCESS;
}


class MyAlgTool
  : public AthAlgTool
{
public:
  MyAlgTool (const std::string& type,
             const std::string& name,
             const IInterface* parent);

  virtual StatusCode initialize() override;

  virtual void addDependency (const DataObjID& obj, const Gaudi::DataHandle::Mode& mode) override;

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandle<MyObj> whandle;

  SG::ReadDecorHandleKey<MyObj> rdkey;
  SG::WriteDecorHandleKey<MyObj> wdkey;

  std::vector<DataObjID> extra_inputs;
  std::vector<DataObjID> extra_outputs;
};


MyAlgTool::MyAlgTool  (const std::string& type,
                       const std::string& name,
                       const IInterface* parent)
  : AthAlgTool (type, name, parent)
{
  declareProperty ("rkey", rkey);
  declareProperty ("whandle", whandle);
  declareProperty ("rdkey", rdkey);
  declareProperty ("wdkey", wdkey);
}


StatusCode MyAlgTool::initialize()
{
  ATH_CHECK( wdkey.initialize() );
  return StatusCode::SUCCESS;
}


void MyAlgTool::addDependency (const DataObjID& obj, const Gaudi::DataHandle::Mode& mode)
{
  if (mode & Gaudi::DataHandle::Reader) 
    extra_inputs.push_back( obj );
  if (mode & Gaudi::DataHandle::Writer) 
    extra_outputs.push_back( obj );
  AthAlgTool::addDependency (obj, mode);
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


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  MyAlg alg ("toolalg", svcloc);  alg.addRef();
  MyAlgTool tool ("MyAlgTool", "tool2", &alg);  tool.addRef();
  assert (tool.setProperties().isSuccess());
  assert (tool.sysInitialize().isSuccess());

  assert (tool.rkey.clid() == 293847295);
  assert (tool.rkey.key() == "taa");
  assert (tool.rkey.storeHandle().name() == "StoreGateSvc");
  assert (tool.rkey.mode() == Gaudi::DataHandle::Reader);

  assert (tool.whandle.clid() == 293847295);
  assert (tool.whandle.key() == "tee");
  assert (tool.whandle.storeHandle().name() == "StoreGateSvc");
  assert (tool.whandle.mode() == Gaudi::DataHandle::Writer);

  comphandles (tool.inputHandles(), { "StoreGateSvc+taa", "StoreGateSvc+tgg.qqq" });
  comphandles (tool.outputHandles(), { "StoreGateSvc+tee", "StoreGateSvc+thh.rrr" });

  std::vector<std::string> extraInputKeys { "StoreGateSvc+thh" };
  assert (tool.extra_inputs.size() == extraInputKeys.size());
  for (size_t i = 0; i < tool.extra_inputs.size(); i++) {
    // std::cout << "extra inp " << tool.extra_inputs[i].key() << "\n";
    assert (tool.extra_inputs[i].key() == extraInputKeys[i]);
  }

  assert (tool.extra_outputs.empty() );

  DataObjIDColl exp = {
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "tee" },
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "thh.rrr" },
  };
  if (exp != tool.outputDataObjs()) {
    for (const DataObjID& o : tool.outputDataObjs()) {
      std::cout << "obj " << o.clid() << " " << o.key() << "\n";
    }
  }
}


class MyArrAlgTool : public AthAlgTool
{
public:
  MyArrAlgTool (const std::string& type,
                const std::string& name,
                const IInterface* parent);

  SG::ReadHandleKey<MyObj> rkey;
  SG::WriteHandleKey<MyObj> wkey;
  SG::ReadHandleKeyArray<MyObj> rkeyarr;
  SG::WriteHandleKeyArray<MyObj> wkeyarr;
};


MyArrAlgTool::MyArrAlgTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : AthAlgTool (type, name, parent)
{
  declareProperty ("rkey",    rkey);
  declareProperty ("wkey",    wkey);
  declareProperty ("rkeyarr", rkeyarr);
  declareProperty ("wkeyarr", wkeyarr);

  rkeyarr.emplace_back ("r1");
  wkeyarr.emplace_back ("w1");
}


// Testing handle arrays.
void test2 (ISvcLocator* svcLoc)
{
  std::cout << "test2\n";

  MyAlg alg ("arralg", svcLoc);  alg.addRef();
  MyArrAlgTool tool ("MyAlgTool", "arrtool", &alg);  tool.addRef();
  assert (tool.setProperties().isSuccess());
  assert (tool.sysInitialize().isSuccess());
  comphandles (tool.inputHandles(),{"StoreGateSvc+raa", "StoreGateSvc+rbb", "StoreGateSvc+rcc", "StoreGateSvc+rdd", "StoreGateSvc+ree", "StoreGateSvc+rff", "StoreGateSvc+rrr"});
  comphandles (tool.outputHandles(),{"StoreGateSvc+waa", "StoreGateSvc+wbb", "StoreGateSvc+wcc", "StoreGateSvc+wdd", "StoreGateSvc+wee", "StoreGateSvc+wff", "StoreGateSvc+www"});

  // Test that circular dependency detection worksd.
  MyArrAlgTool tool2 ("MyArrAlgTool", "arrtool2", &alg);  tool2.addRef();
  assert (tool2.setProperties().isSuccess());
  assert (tool2.sysInitialize().isFailure());
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
