/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_SIDSimHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCnv_p1.h"

typedef T_AtlasHitsVectorCnv< AFP_SIDSimHitCollection, AFP_SIDSimHitCollection_p1, AFP_SIDSimHitCnv_p1 > AFP_SIDSimHitCollectionCnv_p1;
//typedef T_AthenaPoolTPPtrVectorCnv< AFP_HitCollection, AFP_HitCollection_p1, AFP_HitCnv_p1 > AFP_HitCollectionCnv_p1;
