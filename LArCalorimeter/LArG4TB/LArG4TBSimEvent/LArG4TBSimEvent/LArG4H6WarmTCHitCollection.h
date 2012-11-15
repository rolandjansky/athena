/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBWARMTCHITCOLLECTION_H
#define LARTBWARMTCHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6WarmTCHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArG4H6WarmTCHit> LArG4H6WarmTCHitCollection;
typedef AthenaHitsVector<LArG4H6WarmTCHit>::iterator LArG4H6WarmTCHitIterator;
typedef AthenaHitsVector<LArG4H6WarmTCHit>::const_iterator LArG4H6WarmTCHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArG4H6WarmTCHitCollection, 1319483403 , 1 ) 
#endif

#endif 
