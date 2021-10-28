/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthCheckedComponent.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TestTools/initGaudi.h"

class MyAlg : public AthCheckedComponent<AthAlgorithm> {
public:
  MyAlg(const std::string& name, ISvcLocator* svcLoc) :
    AthCheckedComponent<AthAlgorithm>(name, svcLoc) {}
  virtual StatusCode execute() override { return StatusCode::SUCCESS; }
};

class MyTool : public AthCheckedComponent<AthAlgTool> {
public:
  MyTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthCheckedComponent<AthAlgTool>(type, name, parent) {}
};

class MyService : public AthCheckedComponent<AthService> {
public:
  MyService(const std::string& name, ISvcLocator* svcLoc) :
    AthCheckedComponent<AthService>(name, svcLoc) {}
};


void testAlg(ISvcLocator* svcLoc)
{
  MyAlg alg1("MyAlg1", svcLoc);
  alg1.addRef();
  MyAlg alg2("MyAlg2", svcLoc);
  alg2.addRef();
  svcLoc->getOptsSvc().set("MyAlg2.OutputLevel", "1");

  assert( alg1.sysInitialize().isFailure() );
  assert( alg2.sysInitialize().isSuccess() );
}

void testTool(ISvcLocator* svcLoc)
{
  MyAlg alg("MyAlg", svcLoc);
  alg.addRef();
  MyTool tool1("MyTool", "MyAlg.MyTool1", &alg);
  MyTool tool2("MyTool", "MyAlg.MyTool2", &alg);
  svcLoc->getOptsSvc().set("MyAlg.MyTool2.OutputLevel", "1");

  assert( tool1.sysInitialize().isFailure() );
  assert( tool2.sysInitialize().isSuccess() );
}

void testService(ISvcLocator* svcLoc)
{
  MyService svc1("MySvc1", svcLoc);
  svc1.addRef();
  MyService svc2("MySvc2", svcLoc);
  svc2.addRef();
  svcLoc->getOptsSvc().set("MySvc2.OutputLevel", "1");

  assert( svc1.sysInitialize().isFailure() );
  assert( svc2.sysInitialize().isSuccess() );
}

int main()
{
  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi(svcLoc)) return 1;

  testAlg(svcLoc);
  testTool(svcLoc);
  testService(svcLoc);

  return 0;
}
