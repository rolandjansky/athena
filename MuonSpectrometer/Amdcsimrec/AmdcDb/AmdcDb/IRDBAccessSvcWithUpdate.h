/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRDBAccessSvcWithUpdate_H
#define IRDBAccessSvcWithUpdate_H

#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "AthenaKernel/IOVSvcDefs.h"

  /**
   @class AmdcDb

   This class dreived from IRDBAccessSvc adds the control state method
   
  @author samusog@cern.ch
  
  */

class IRDBAccessSvcWithUpdate : virtual public IRDBAccessSvc {
public:

  virtual bool       InitializedSvc() = 0 ;
  virtual bool       UsableSvc() = 0 ;
  virtual StatusCode UpdatedSvc(IOVSVC_CALLBACK_ARGS) = 0 ;
  
};

#endif

