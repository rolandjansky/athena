/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTORCNV_H
#define JETEVENT_JETKEYDESCRIPTORCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include <string>
#include <vector>

#include "JetEvent/JetKeyDescriptor.h"
#include "JetEventTPCnv/JetKeyDescriptor_p1.h"

// the latest persistent representation type of JetKeyDescriptor_p1
typedef  JetKeyDescriptor_p1  JetKeyDescriptor_PERS;
typedef  T_AthenaPoolCustomCnv<JetKeyDescriptor, JetKeyDescriptor_PERS>  JetKeyDescriptorCnvBase;

class JetKeyDescriptorCnv : public JetKeyDescriptorCnvBase
{
  friend class CnvFactory<JetKeyDescriptorCnv>;
  
 protected:

  JetKeyDescriptorCnv (ISvcLocator* svcloc) : JetKeyDescriptorCnvBase(svcloc) {};
  virtual JetKeyDescriptor_PERS*   createPersistent (JetKeyDescriptor* transObj);
  virtual JetKeyDescriptor*        createTransient ();
};

#endif
