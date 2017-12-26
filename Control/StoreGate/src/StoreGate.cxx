/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGate.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/tools/hash_functions.h"

#include <exception>
#include <iostream>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"

using namespace std;

ActiveStoreSvc* 
getActiveStoreSvc() {
  ActiveStoreSvc* pASG(0);
  static const bool CREATEIF(true);
  if ((Gaudi::svcLocator()->service("ActiveStoreSvc", pASG, CREATEIF)).isSuccess()) {
    pASG->addRef(); //FIXME WHO RELEASES?
    return pASG; 
  } else {
#ifndef NDEBUG
    cerr << __FILE__ << ':' << __LINE__ << ":   "
         << "ERROR Could not locate ActiveStoreSvc " <<endl;
#endif
    return 0;
  }
}

StoreGateSvc* 
getStore() {
  ActiveStoreSvc* pASG(0);
  StoreGateSvc* pSG(0);
  static const bool CREATEIF(true);
  if ((Gaudi::svcLocator()->service("ActiveStoreSvc", pASG, CREATEIF)).isSuccess() &&
      0 != (pSG = pASG->operator->())) {
    pSG->addRef(); //FIXME WHO RELEASES?
    return pSG; 
  } else {
#ifndef NDEBUG
    cerr << __FILE__ << ':' << __LINE__ << ":   "
         << "ERROR Could not locate active StoreGate " <<endl;
#endif
    return 0;
  }
}

StoreGateSvc* 
getStore(std::string name) {
  StoreGateSvc* pSGService(0);
  static const bool CREATEIF(true);
  if ((Gaudi::svcLocator()->service(name, pSGService, CREATEIF)).isSuccess()) {
    pSGService->addRef();
    return pSGService;
  } else {
#ifndef NDEBUG
    cerr << __FILE__ << ':' << __LINE__ << ":   "
         << "ERROR Could not locate StoreGate "
         << "instance named " << name << endl;
#endif
    return 0;
  }
}

StoreGateSvc*
StoreGate::pointer() {
  return getStore();
}


StoreGateSvc& 
StoreGate::instance() { 
  StoreGateSvc* ptr(pointer());
  if (0 == ptr) {
    throw std::runtime_error("Could not locate active StoreGate ");
  }
  return *ptr;
}

ActiveStoreSvc*
StoreGate::activeStoreSvc() {
  return getActiveStoreSvc();
}

StoreGateSvc*
StoreGate::pointer(std::string sgID) {
  return getStore(sgID);
}


StoreGateSvc& 
StoreGate::instance(std::string sgID) {
  StoreGateSvc* ptr(pointer(sgID));
  if (0 == ptr) {
    throw std::runtime_error("Could not locate required StoreGate instance");
  }
  return *ptr;
}












