/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITGCCABLINGGETTERSVC_H
#define ITGCCABLINGGETTERSVC_H

#ifndef LVL1_STANDALONE


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
  virtual bool isConfigured(void) const=0;

  virtual StatusCode compareTags(IOVSVC_CALLBACK_ARGS)=0;

};
 
#endif

#endif  //ITGCCABLINGSVC_H
