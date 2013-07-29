/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HitCollection_h
#define ALFA_HitCollection_h


#include "ALFA_SimEv/ALFA_Hit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AtlasHitsVector<ALFA_Hit> ALFA_HitCollection;
typedef AtlasHitsVector<ALFA_Hit>::iterator ALFA_HitIter;
typedef AtlasHitsVector<ALFA_Hit>::const_iterator ALFA_HitConstIter;

#ifndef __CINT__
CLASS_DEF(ALFA_HitCollection, 1088734340, 1) // from where the number is coming? CLID
#endif

#endif
