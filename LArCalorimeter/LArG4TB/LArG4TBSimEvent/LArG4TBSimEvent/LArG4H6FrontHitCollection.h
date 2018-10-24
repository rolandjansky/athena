/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBFRONTHITCOLLECTION_H
#define LARTBFRONTHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<LArG4H6FrontHit> LArG4H6FrontHitCollection;
typedef AthenaHitsVector<LArG4H6FrontHit>::iterator LArG4H6FrontHitIterator;
typedef AthenaHitsVector<LArG4H6FrontHit>::const_iterator LArG4H6FrontHitConstIterator;

CLASS_DEF(LArG4H6FrontHitCollection, 1250573243 , 1 ) 

#endif 
