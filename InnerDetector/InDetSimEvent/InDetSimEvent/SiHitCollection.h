/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SITRACKEREVENT_SIHITCOLLECTION_H
#define SITRACKEREVENT_SIHITCOLLECTION_H

#include "InDetSimEvent/SiHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"// for back compatibility
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif
typedef AthenaHitsVector<SiHit> SiOldHitCollection;

typedef AtlasHitsVector<SiHit> SiHitCollection;
typedef AtlasHitsVector<SiHit>::iterator SiHitIterator;
typedef AtlasHitsVector<SiHit>::const_iterator SiHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(SiHitCollection, 2545 , 1 ) 
  CLASS_DEF(SiOldHitCollection, 1175907564, 1 ) 
#endif

#endif // SITRACKEREVENT_SIHITCOLLECTION_H
