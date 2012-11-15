/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBLEAKHITCOLLECTION_H
#define LARTBLEAKHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6LeakHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArG4H6LeakHit> LArG4H6LeakHitCollection;
typedef AthenaHitsVector<LArG4H6LeakHit>::iterator LArG4H6LeakHitIterator;
typedef AthenaHitsVector<LArG4H6LeakHit>::const_iterator LArG4H6LeakHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArG4H6LeakHitCollection, 1259234314 , 1 ) 
#endif

#endif 
