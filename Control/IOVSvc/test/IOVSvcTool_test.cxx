/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "IOVSvc/IIOVSvcTool.h"
#include "StoreGate/StoreGateSvc.h"

#include <cassert>
#include <iostream>

#include "SGTools/CLASS_DEF.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
using namespace SG;
using namespace std;

struct Dooo {
  Dooo(int dy=0) : Daaa(dy) {}
  int Daaa;
};
CLASS_DEF(Dooo, 6666, 1)

namespace Athena_test {

  void IOVSvcTool_test (IIOVSvcTool *pTool) {
    assert(pTool);
    DataProxy* pOld(new DataProxy(StoreGateSvc::asStorable(new Dooo(1)),
				  new TransientAddress(CLID(6666), "old")));
    assert((pTool->regProxy(pOld, "old").isSuccess()));
    assert(pTool->holdsProxy(pOld));
    DataProxy* pNew(new DataProxy(StoreGateSvc::asStorable(new Dooo(2)),
				  new TransientAddress(CLID(6666), "new")));
    assert((pTool->replaceProxy(pOld, pNew).isSuccess()));
    assert(pTool->holdsProxy(pNew) );
    assert(!pTool->holdsProxy(pOld) );
  }


}


using namespace Athena_test;

//#include "Reflex/PluginService.h"

int main() { 
  //  ROOT::Reflex::PluginService::SetDebug(8);
  std::cout << "*** IOVSvcTool_test starts ***" << std::endl;
  ISvcLocator* pSvc;
  if (!initGaudi("IOVSvcTool_test.txt", pSvc))
    return 1;
  //initGaudi(pSvc); 
  IToolSvc* pTS(0);
  assert((pSvc->service("ToolSvc", pTS, true)).isSuccess());
  IAlgTool* pTool(0);
  assert((pTS->retrieve("IOVSvcTool", IIOVSvcTool::interfaceID(), pTool, 0, true)).isSuccess());
  IOVSvcTool_test(dynamic_cast<IIOVSvcTool*>(pTool));
  std::cout << "*** IOVSvcTool_test OK ***" << std::endl;
  return 0; 
}
