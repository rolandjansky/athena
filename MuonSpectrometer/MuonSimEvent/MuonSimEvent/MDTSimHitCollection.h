/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCollection_H
#define MDTSimHitCollection_H

#include "MuonSimEvent/MDTSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif
typedef AthenaHitsVector<MDTSimHit> MDTOldSimHitCollection;

typedef AtlasHitsVector<MDTSimHit> MDTSimHitCollection;
typedef AtlasHitsVector<MDTSimHit>::iterator MDTSimHitIterator;
typedef AtlasHitsVector<MDTSimHit>::const_iterator MDTSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF (MDTSimHitCollection, 4054, 1)
CLASS_DEF (MDTOldSimHitCollection, 1138135938, 1)
#endif

#endif
