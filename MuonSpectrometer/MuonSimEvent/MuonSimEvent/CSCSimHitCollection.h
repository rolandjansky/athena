/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCollection_H
#define CSCSimHitCollection_H

#include "MuonSimEvent/CSCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"
typedef AthenaHitsVector<CSCSimHit> CSCOldSimHitCollection;

typedef AtlasHitsVector<CSCSimHit> CSCSimHitCollection;
typedef AtlasHitsVector<CSCSimHit>::iterator CSCSimHitIterator;
typedef AtlasHitsVector<CSCSimHit>::const_iterator CSCSimHitConstIterator;

CLASS_DEF (CSCSimHitCollection, 4053, 1)
CLASS_DEF (CSCOldSimHitCollection, 1333371890, 1)

#endif
