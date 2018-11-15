/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCollection_H
#define MDTSimHitCollection_H

#include "MuonSimEvent/MDTSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"
typedef AthenaHitsVector<MDTSimHit> MDTOldSimHitCollection;

typedef AtlasHitsVector<MDTSimHit> MDTSimHitCollection;
typedef AtlasHitsVector<MDTSimHit>::iterator MDTSimHitIterator;
typedef AtlasHitsVector<MDTSimHit>::const_iterator MDTSimHitConstIterator;

CLASS_DEF (MDTSimHitCollection, 4054, 1)
CLASS_DEF (MDTOldSimHitCollection, 1138135938, 1)

#endif
