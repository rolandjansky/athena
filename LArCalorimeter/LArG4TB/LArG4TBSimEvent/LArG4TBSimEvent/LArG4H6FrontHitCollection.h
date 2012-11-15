/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBFRONTHITCOLLECTION_H
#define LARTBFRONTHITCOLLECTION_H

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<LArG4H6FrontHit> LArG4H6FrontHitCollection;
typedef AthenaHitsVector<LArG4H6FrontHit>::iterator LArG4H6FrontHitIterator;
typedef AthenaHitsVector<LArG4H6FrontHit>::const_iterator LArG4H6FrontHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(LArG4H6FrontHitCollection, 1250573243 , 1 ) 
#endif

#endif 
