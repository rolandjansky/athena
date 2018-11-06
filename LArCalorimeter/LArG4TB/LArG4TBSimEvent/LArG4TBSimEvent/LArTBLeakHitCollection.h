/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBLEAKHITCOLLECTION_H
#define LARTBLEAKHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBLeakHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArTBLeakHit> LArTBLeakHitCollection;
typedef AthenaHitsVector<LArTBLeakHit>::iterator LArTBLeakHitIterator;
typedef AthenaHitsVector<LArTBLeakHit>::const_iterator LArTBLeakHitConstIterator;

CLASS_DEF(LArTBLeakHitCollection, 196013649 , 1 ) 

#endif 
