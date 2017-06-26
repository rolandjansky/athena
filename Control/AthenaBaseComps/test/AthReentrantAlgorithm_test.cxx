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
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
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


IProxyDict* pdict = nullptr;


class MyAlg
  : public AthReentrantAlgorithm
{
public:
  MyAlg (const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize() override;
  virtual StatusCode execute_r (const EventContext& ctx) const override;

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
};


MyAlg::MyAlg  (const std::string& name, ISvcLocator* svcLoc)
  : AthReentrantAlgorithm (name, svcLoc)
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


StatusCode MyAlg::execute_r (const EventContext& ctx) const
{
  pdict = ctx.getExtension<Atlas::ExtendedEventContext>()->proxy();
  return StatusCode::SUCCESS;
}


void MyAlg::declare(Gaudi::DataHandle& hnd) {
  if (hnd.mode() & Gaudi::DataHandle::Reader) 
    inputs.push_back( &hnd );
  if (hnd.mode() & Gaudi::DataHandle::Writer) 
    outputs.push_back( &hnd );
  AthReentrantAlgorithm::declare (hnd);
}


void MyAlg::addDependency (const DataObjID& obj, const Gaudi::DataHandle::Mode& mode)
{
  if (mode & Gaudi::DataHandle::Reader) 
    extra_inputs.push_back( obj );
  if (mode & Gaudi::DataHandle::Writer) 
    extra_outputs.push_back( obj );
  AthReentrantAlgorithm::addDependency (obj, mode);
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

  assert (alg.rdkey.clid() == 293847295);
  assert (alg.rdkey.key() == "yyy.qqq");
  assert (alg.rdkey.storeHandle().name() == "FooSvc");
  assert (alg.rdkey.mode() == Gaudi::DataHandle::Reader);

  assert (alg.wdkey.clid() == 293847295);
  assert (alg.wdkey.key() == "zzz.rrr");
  assert (alg.wdkey.storeHandle().name() == "StoreGateSvc");
  assert (alg.wdkey.mode() == Gaudi::DataHandle::Writer);
  assert (alg.wdkey.contHandleKey().clid() == 293847295);
  assert (alg.wdkey.contHandleKey().key() == "zzz");
  assert (alg.wdkey.contHandleKey().storeHandle().name() == "StoreGateSvc");
  assert (alg.wdkey.contHandleKey().mode() == Gaudi::DataHandle::Reader);

  std::vector<std::string> inputKeys { "aaa", "yyy.qqq" };
  assert (alg.inputs.size() == inputKeys.size());
  for (size_t i = 0; i < alg.inputs.size(); i++) {
    //std::cout << "inp " << alg.inputs[i]->objKey() << "\n";
    assert (alg.inputs[i]->objKey() == inputKeys[i]);
  }

  std::vector<std::string> outputKeys { "eee", "zzz.rrr" };
  assert (alg.outputs.size() == outputKeys.size());
  for (size_t i = 0; i < alg.outputs.size(); i++) {
    //std::cout << "out " << alg.outputs[i]->objKey() << "\n";
    assert (alg.outputs[i]->objKey() == outputKeys[i]);
  }

  std::vector<std::string> extraInputKeys { "zzz" };
  assert (alg.extra_inputs.size() == extraInputKeys.size());
  for (size_t i = 0; i < alg.extra_inputs.size(); i++) {
    //std::cout << "extra inp " << alg.extra_inputs[i].key() << "\n";
    assert (alg.extra_inputs[i].key() == extraInputKeys[i]);
  }

  assert (alg.extra_outputs.empty() );

  IProxyDict* xdict = &*alg.evtStore();
  xdict = alg.evtStore()->hiveProxyDict();
  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );

  Gaudi::Hive::setCurrentContext (ctx);

  assert (alg.execute().isSuccess());
  assert (pdict == xdict);

  DataObjIDColl exp = {
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "eee" },
    { ClassID_traits<AthenaBaseCompsTest::MyBase>::ID(), "eee" },
    { ClassID_traits<AthenaBaseCompsTest::MyObj>::ID(), "zzz.rrr" },
    { ClassID_traits<AthenaBaseCompsTest::MyBase>::ID(), "zzz.rrr" },
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
