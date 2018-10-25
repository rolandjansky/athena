/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBLEAKHITCOLLECTION_H
#define LARTBLEAKHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6LeakHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArG4H6LeakHit> LArG4H6LeakHitCollection;
typedef AthenaHitsVector<LArG4H6LeakHit>::iterator LArG4H6LeakHitIterator;
typedef AthenaHitsVector<LArG4H6LeakHit>::const_iterator LArG4H6LeakHitConstIterator;

CLASS_DEF(LArG4H6LeakHitCollection, 1259234314 , 1 ) 

#endif 
