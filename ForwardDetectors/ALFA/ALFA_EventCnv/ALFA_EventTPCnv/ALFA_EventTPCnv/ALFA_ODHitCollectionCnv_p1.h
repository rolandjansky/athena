/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_ODHitCollection_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ALFA_SimEv/ALFA_ODHitCollection.h"
#include "ALFA_EventTPCnv/ALFA_ODHitCnv_p1.h"
 
 
typedef T_AtlasHitsVectorCnv< ALFA_ODHitCollection, ALFA_ODHitCollection_p1, ALFA_ODHitCnv_p1 > ALFA_ODHitCollectionCnv_p1;
//typedef T_AthenaPoolTPPtrVectorCnv< ALFA_HitCollection, ALFA_HitCollection_p1, ALFA_HitCnv_p1 > ALFA_HitCollectionCnv_p1;
