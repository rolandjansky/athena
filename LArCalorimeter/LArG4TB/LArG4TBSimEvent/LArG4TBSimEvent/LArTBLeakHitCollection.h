/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBLEAKHITCOLLECTION_H
#define LARTBLEAKHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBLeakHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArTBLeakHit> LArTBLeakHitCollection;
typedef AthenaHitsVector<LArTBLeakHit>::iterator LArTBLeakHitIterator;
typedef AthenaHitsVector<LArTBLeakHit>::const_iterator LArTBLeakHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArTBLeakHitCollection, 196013649 , 1 ) 
#endif

#endif 
