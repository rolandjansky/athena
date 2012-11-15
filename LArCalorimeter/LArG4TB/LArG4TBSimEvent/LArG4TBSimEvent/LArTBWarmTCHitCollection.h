/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBWARMTCHITCOLLECTION_H
#define LARTBWARMTCHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBWarmTCHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArTBWarmTCHit> LArTBWarmTCHitCollection;
typedef AthenaHitsVector<LArTBWarmTCHit>::iterator LArTBWarmTCHitIterator;
typedef AthenaHitsVector<LArTBWarmTCHit>::const_iterator LArTBWarmTCHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArTBWarmTCHitCollection, 96211162 , 1 ) 
#endif

#endif 
