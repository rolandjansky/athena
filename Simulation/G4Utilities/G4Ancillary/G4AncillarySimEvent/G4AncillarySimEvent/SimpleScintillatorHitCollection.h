/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLESCINTILLATORHITCOLLECTION_H
#define SIMPLESCINTILLATORHITCOLLECTION_H

#include "G4AncillarySimEvent/SimpleScintillatorHit.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<SimpleScintillatorHit> SimpleScintillatorHitCollection;
typedef AthenaHitsVector<SimpleScintillatorHit>::iterator SimpleScintillatorHitIterator;
typedef AthenaHitsVector<SimpleScintillatorHit>::const_iterator SimpleScintillatorHitConstIterator;

CLASS_DEF(SimpleScintillatorHitCollection, 917117188 , 1 ) 

#endif // SIMPLESCINTILLATORHITCOLLECTION_H
