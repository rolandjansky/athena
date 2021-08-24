/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITGCCABLINGGETTERSVC_H
#define ITGCCABLINGGETTERSVC_H

#include "AthenaKernel/IOVSvcDefs.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_TGCcablingServerSvc("TGCcablingServerSvc", 1 , 0);

class ITGCcablingServerSvc : virtual public IInterface 
{
 public:

  // / Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_TGCcablingServerSvc; }

  // serve the cabling implementation
  virtual StatusCode giveCabling(const ITGCcablingSvc*&) const=0;  
  virtual bool isAtlas(void) const=0;
};
 
#endif  //ITGCCABLINGSVC_H
