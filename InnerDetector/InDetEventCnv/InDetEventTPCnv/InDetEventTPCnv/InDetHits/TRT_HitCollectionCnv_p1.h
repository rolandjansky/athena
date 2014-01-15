/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TRT_HitCollection_p1.h"
#undef private 
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "TRT_HitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

typedef T_AtlasHitsVectorCnv< TRTUncompressedHitCollection, TRT_HitCollection_p1, TRT_HitCnv_p1 >  TRT_HitCollectionCnv_p1;
