/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBWARMTCHITCOLLECTION_H
#define LARTBWARMTCHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBWarmTCHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArTBWarmTCHit> LArTBWarmTCHitCollection;
typedef AthenaHitsVector<LArTBWarmTCHit>::iterator LArTBWarmTCHitIterator;
typedef AthenaHitsVector<LArTBWarmTCHit>::const_iterator LArTBWarmTCHitConstIterator;

CLASS_DEF(LArTBWarmTCHitCollection, 96211162 , 1 ) 

#endif 
