/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTILLATORHITCOLLECTION_H
#define SCINTILLATORHITCOLLECTION_H

#include "G4AncillarySimEvent/ScintillatorHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<ScintillatorHit> ScintillatorHitCollection;
typedef AthenaHitsVector<ScintillatorHit>::iterator ScintillatorHitIterator;
typedef AthenaHitsVector<ScintillatorHit>::const_iterator ScintillatorHitConstIterator;

CLASS_DEF(ScintillatorHitCollection, 917117117 , 1 ) 

#endif // SCINTILLATORHITCOLLECTION_H
