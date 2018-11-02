/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSimHitCollection_H
#define sTGCSimHitCollection_H

#include "MuonSimEvent/sTGCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<sTGCSimHit> OldsTGCSimHitCollection;

typedef AtlasHitsVector<sTGCSimHit> sTGCSimHitCollection;
typedef AtlasHitsVector<sTGCSimHit>::iterator sTGCSimHitIterator;
typedef AtlasHitsVector<sTGCSimHit>::const_iterator sTGCSimHitConstIterator;

CLASS_DEF( sTGCSimHitCollection , 20180530 , 1 )
CLASS_DEF( OldsTGCSimHitCollection , 20180529 , 1 )

#endif

