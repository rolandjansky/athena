/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestTools/initGaudi.h"
#include "StoreGate/SegMemSvc.h"

#include <cassert>
#include <iostream>

namespace Athena_test {

  void segmem_test (ISvcLocator *psvc) {

    SegMemSvc* p_sms(0);
    assert((psvc->service("SegMemSvc", p_sms, true)).isSuccess());

    int* p_int = new ( p_sms->allocate<int>(SegMemSvc::EVENT) ) int(1001);

    // cppcheck doesn't seem to understand placement new.
    // cppcheck-suppress uninitdata
    assert ( *p_int == 1001 );

  }


}


using namespace Athena_test;

//#include "Reflex/PluginService.h"

int main() { 
  //  ROOT::Reflex::PluginService::SetDebug(8);
  ISvcLocator* pDum;
  initGaudi(pDum); //need MessageSvc
  segmem_test(pDum);
  std::cout << "*** SegMemSvc_test OK ***" << std::endl;
  return 0; 
}
