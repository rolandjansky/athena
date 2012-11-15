/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBFRONTHITCOLLECTION_H
#define LARTBFRONTHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBFrontHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArTBFrontHit> LArTBFrontHitCollection;
typedef AthenaHitsVector<LArTBFrontHit>::iterator LArTBFrontHitIterator;
typedef AthenaHitsVector<LArTBFrontHit>::const_iterator LArTBFrontHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArTBFrontHitCollection, 176134262 , 1 ) 
#endif

#endif 
