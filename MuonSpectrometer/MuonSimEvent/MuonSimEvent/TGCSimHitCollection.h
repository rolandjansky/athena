/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCollection_H
#define TGCSimHitCollection_H

#include "MuonSimEvent/TGCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<TGCSimHit> TGCOldSimHitCollection;

typedef AtlasHitsVector<TGCSimHit> TGCSimHitCollection;
typedef AtlasHitsVector<TGCSimHit>::iterator TGCSimHitIterator;
typedef AtlasHitsVector<TGCSimHit>::const_iterator TGCSimHitConstIterator;

CLASS_DEF (TGCSimHitCollection, 4052, 1)
CLASS_DEF (TGCOldSimHitCollection, 1275808031, 1)

#endif
