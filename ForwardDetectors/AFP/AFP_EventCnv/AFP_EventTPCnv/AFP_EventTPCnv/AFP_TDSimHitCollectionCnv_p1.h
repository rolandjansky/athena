/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_TDSimHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCnv_p1.h"

typedef T_AtlasHitsVectorCnv< AFP_TDSimHitCollection, AFP_TDSimHitCollection_p1, AFP_TDSimHitCnv_p1 > AFP_TDSimHitCollectionCnv_p1;
//typedef T_AthenaPoolTPPtrVectorCnv< AFP_HitCollection, AFP_HitCollection_p1, AFP_HitCnv_p1 > AFP_HitCollectionCnv_p1;
