/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCollection_H
#define CSCSimHitCollection_H

#include "MuonSimEvent/CSCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif
typedef AthenaHitsVector<CSCSimHit> CSCOldSimHitCollection;

typedef AtlasHitsVector<CSCSimHit> CSCSimHitCollection;
typedef AtlasHitsVector<CSCSimHit>::iterator CSCSimHitIterator;
typedef AtlasHitsVector<CSCSimHit>::const_iterator CSCSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF (CSCSimHitCollection, 4053, 1)
CLASS_DEF (CSCOldSimHitCollection, 1333371890, 1)
#endif

#endif
