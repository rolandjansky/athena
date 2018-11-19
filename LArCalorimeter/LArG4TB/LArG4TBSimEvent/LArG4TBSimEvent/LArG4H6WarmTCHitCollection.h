/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBWARMTCHITCOLLECTION_H
#define LARTBWARMTCHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6WarmTCHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArG4H6WarmTCHit> LArG4H6WarmTCHitCollection;
typedef AthenaHitsVector<LArG4H6WarmTCHit>::iterator LArG4H6WarmTCHitIterator;
typedef AthenaHitsVector<LArG4H6WarmTCHit>::const_iterator LArG4H6WarmTCHitConstIterator;

CLASS_DEF(LArG4H6WarmTCHitCollection, 1319483403 , 1 ) 

#endif 
