/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTILLATORHITCOLLECTION_H
#define SCINTILLATORHITCOLLECTION_H

#include "G4AncillarySimEvent/ScintillatorHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<ScintillatorHit> ScintillatorHitCollection;
typedef AthenaHitsVector<ScintillatorHit>::iterator ScintillatorHitIterator;
typedef AthenaHitsVector<ScintillatorHit>::const_iterator ScintillatorHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(ScintillatorHitCollection, 917117117 , 1 ) 
#endif

#endif // SCINTILLATORHITCOLLECTION_H
