/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCollection_H
#define TGCSimHitCollection_H

#include "MuonSimEvent/TGCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<TGCSimHit> TGCOldSimHitCollection;

typedef AtlasHitsVector<TGCSimHit> TGCSimHitCollection;
typedef AtlasHitsVector<TGCSimHit>::iterator TGCSimHitIterator;
typedef AtlasHitsVector<TGCSimHit>::const_iterator TGCSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF (TGCSimHitCollection, 4052, 1)
CLASS_DEF (TGCOldSimHitCollection, 1275808031, 1)
#endif

#endif
