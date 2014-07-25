/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPCCABLINGGETTERSVC_H
#define IRPCCABLINGGETTERSVC_H

#ifndef LVL1_STANDALONE

#include "RPCcablingInterface/IRPCcablingSvc.h"


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_RPCcablingServerSvc("RPCcablingServerSvc", 1 , 0);

class IRPCcablingServerSvc : virtual public IInterface 
{
 public:

  // / Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_RPCcablingServerSvc; }

  // serve the cabling implementation
  virtual StatusCode giveCabling(const IRPCcablingSvc*&) const=0;  
  virtual bool isAtlas(void) const=0;
};
 
#endif

#endif  //IRPCCABLINGSVC_H
