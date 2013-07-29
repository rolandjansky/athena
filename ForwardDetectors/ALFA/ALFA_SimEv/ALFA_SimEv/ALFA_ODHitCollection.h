/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODHitCollection_h
#define ALFA_ODHitCollection_h


#include "ALFA_SimEv/ALFA_ODHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AtlasHitsVector<ALFA_ODHit> ALFA_ODHitCollection;
typedef AtlasHitsVector<ALFA_ODHit>::iterator ALFA_ODHitIter;
typedef AtlasHitsVector<ALFA_ODHit>::const_iterator ALFA_ODHitConstIter;

#ifndef __CINT__
CLASS_DEF( ALFA_ODHitCollection , 1133954409 , 1 ) // from where the number is coming? CLID
#endif

#endif // ALFA_ODHitCollection_hh
