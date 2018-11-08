/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitCollection_H
#define RPCSimHitCollection_H

#include "MuonSimEvent/RPCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<RPCSimHit> RPCOldSimHitCollection;

typedef AtlasHitsVector<RPCSimHit> RPCSimHitCollection;
typedef AtlasHitsVector<RPCSimHit>::iterator RPCSimHitIterator;
typedef AtlasHitsVector<RPCSimHit>::const_iterator RPCSimHitConstIterator;

CLASS_DEF (RPCSimHitCollection, 4051, 1)
CLASS_DEF (RPCOldSimHitCollection, 1182697430, 1)

#endif
