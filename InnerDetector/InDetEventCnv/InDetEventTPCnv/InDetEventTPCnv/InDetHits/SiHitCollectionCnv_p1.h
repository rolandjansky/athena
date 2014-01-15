/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "SiHitCollection_p1.h"
#undef private 
#include "InDetSimEvent/SiHitCollection.h"
#include "SiHitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

typedef T_AtlasHitsVectorCnv< SiHitCollection, SiHitCollection_p1, SiHitCnv_p1 >  SiHitCollectionCnv_p1;
