/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLESCINTILLATORHITCOLLECTION_H
#define SIMPLESCINTILLATORHITCOLLECTION_H

#include "G4AncillarySimEvent/SimpleScintillatorHit.h"
#include "HitManagement/AthenaHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<SimpleScintillatorHit> SimpleScintillatorHitCollection;
typedef AthenaHitsVector<SimpleScintillatorHit>::iterator SimpleScintillatorHitIterator;
typedef AthenaHitsVector<SimpleScintillatorHit>::const_iterator SimpleScintillatorHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(SimpleScintillatorHitCollection, 917117188 , 1 ) 
#endif

#endif // SIMPLESCINTILLATORHITCOLLECTION_H
