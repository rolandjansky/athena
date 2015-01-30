/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETMOMENTMAPCOLLECTIONCNV_P6_H
#define JETEVENT_JETMOMENTMAPCOLLECTIONCNV_P6_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "JetEvent/JetMomentMapCollection.h"
#include "JetEventTPCnv/JetMomentMapCollection_p6.h"
#include "JetEventTPCnv/JetMomentMapCnv_p6.h"

typedef T_AthenaPoolTPCnvVector< JetMomentMapCollection, JetMomentMapCollection_p6, JetMomentMapCnv_p6 > JetMomentMapCollectionCnv_p6;

#endif
