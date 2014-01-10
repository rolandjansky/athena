/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitCollection_H
#define RPCSimHitCollection_H

#include "MuonSimEvent/RPCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<RPCSimHit> RPCOldSimHitCollection;

typedef AtlasHitsVector<RPCSimHit> RPCSimHitCollection;
typedef AtlasHitsVector<RPCSimHit>::iterator RPCSimHitIterator;
typedef AtlasHitsVector<RPCSimHit>::const_iterator RPCSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF (RPCSimHitCollection, 4051, 1)
CLASS_DEF (RPCOldSimHitCollection, 1182697430, 1)
#endif

#endif
