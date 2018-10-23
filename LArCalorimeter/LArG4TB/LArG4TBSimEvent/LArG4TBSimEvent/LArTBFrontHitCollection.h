/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBFRONTHITCOLLECTION_H
#define LARTBFRONTHITCOLLECTION_H

#include "LArG4TBSimEvent/LArTBFrontHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArTBFrontHit> LArTBFrontHitCollection;
typedef AthenaHitsVector<LArTBFrontHit>::iterator LArTBFrontHitIterator;
typedef AthenaHitsVector<LArTBFrontHit>::const_iterator LArTBFrontHitConstIterator;

CLASS_DEF(LArTBFrontHitCollection, 176134262 , 1 ) 

#endif 
