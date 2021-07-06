/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_SimHitCollection_p2.h"
#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
#include "LUCID_SimHitCnv_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

typedef T_AtlasHitsVectorCnv< LUCID_SimHitCollection, LUCID_SimHitCollection_p2, LUCID_SimHitCnv_p2 >  LUCID_SimHitCollectionCnv_p2;
