/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSimHitCollection_H
#define MMSimHitCollection_H

#include "MuonSimEvent/MMSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AthenaHitsVector<MMSimHit> OldMMSimHitCollection;

typedef AtlasHitsVector<MMSimHit> MMSimHitCollection;
typedef AtlasHitsVector<MMSimHit>::iterator MMSimHitIterator;
typedef AtlasHitsVector<MMSimHit>::const_iterator MMSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF( MMSimHitCollection , 20181211 , 1 )
CLASS_DEF( OldMMSimHitCollection , 20181212 , 1 )
#endif

#endif

