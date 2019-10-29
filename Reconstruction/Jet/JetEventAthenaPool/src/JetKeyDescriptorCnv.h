/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTORCNV_H
#define JETEVENT_JETKEYDESCRIPTORCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

#include <string>
#include <vector>

#include "JetEvent/JetKeyDescriptor.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

// the latest persistent representation type of JetKeyDescriptor_p1
typedef T_AthenaPoolTPCnvCnv<JetKeyDescriptor,
                             JetKeyDescriptorCnv_p1>
  JetKeyDescriptorCnv;


#endif
