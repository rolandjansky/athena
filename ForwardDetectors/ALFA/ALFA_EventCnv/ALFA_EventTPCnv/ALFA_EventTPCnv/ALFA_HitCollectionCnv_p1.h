/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_HitCollection_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_EventTPCnv/ALFA_HitCnv_p1.h"
 
 
typedef T_AtlasHitsVectorCnv< ALFA_HitCollection, ALFA_HitCollection_p1, ALFA_HitCnv_p1 > ALFA_HitCollectionCnv_p1;
//typedef T_AthenaPoolTPPtrVectorCnv< ALFA_HitCollection, ALFA_HitCollection_p1, ALFA_HitCnv_p1 > ALFA_HitCollectionCnv_p1;
