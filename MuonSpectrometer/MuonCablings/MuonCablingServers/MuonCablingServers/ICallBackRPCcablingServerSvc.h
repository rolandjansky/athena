/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICallBackRPCcablingServerSvc_H
#define ICallBackRPCcablingServerSvc_H

#ifndef LVL1_STANDALONE

#include "AthenaKernel/IOVSvcDefs.h"

class ICallBackRPCcablingServerSvc : virtual public IInterface 
{
    public:
    virtual bool isConfigured(void) const=0;  
    virtual StatusCode compareTags(IOVSVC_CALLBACK_ARGS)=0;

};
 
#endif

#endif  //ICallBackRPCcablingServerSvc_H
