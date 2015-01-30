/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTORCOLLECTIONCNV_P1_H
#define JETEVENT_JETKEYDESCRIPTORCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "JetEvent/JetKeyDescriptorCollection.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< JetKeyDescriptorCollection, JetKeyDescriptorCollection_p1, JetKeyDescriptorCnv_p1 > JetKeyDescriptorCollectionCnv_p1;

#endif
