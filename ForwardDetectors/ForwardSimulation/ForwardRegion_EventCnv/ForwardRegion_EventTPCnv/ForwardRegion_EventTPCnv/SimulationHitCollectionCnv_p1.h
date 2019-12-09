/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHitCollectionCnv_p1_h
#define SimulationHitCollectionCnv_p1_h
#include "ForwardRegion_EventTPCnv/SimulationHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ForwardRegion_SimEv/SimulationHitCollection.h"
#include "ForwardRegion_EventTPCnv/SimulationHitCnv_p1.h"

typedef T_AtlasHitsVectorCnv< SimulationHitCollection, SimulationHitCollection_p1, SimulationHitCnv_p1 > SimulationHitCollectionCnv_p1;
#endif
