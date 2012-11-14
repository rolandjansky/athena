/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SIMHITCOLLECTION_H
#define LUCID_SIMHITCOLLECTION_H 1

#include "LUCID_SimEvent/LUCID_SimHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AtlasHitsVector<LUCID_SimHit>                 LUCID_SimHitCollection;
typedef AtlasHitsVector<LUCID_SimHit>::iterator       LUCID_SimHitIterator;
typedef AtlasHitsVector<LUCID_SimHit>::const_iterator LUCID_SimHitConstIterator;

#ifndef __CINT__
CLASS_DEF(LUCID_SimHitCollection , 4050 , 0)
#endif

#endif
