/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericMuonSimHitCollection_H
#define GenericMuonSimHitCollection_H

#include "MuonSimEvent/GenericMuonSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<GenericMuonSimHit> OldGenericMuonSimHitCollection;

typedef AtlasHitsVector<GenericMuonSimHit> GenericMuonSimHitCollection;
typedef AtlasHitsVector<GenericMuonSimHit>::iterator GenericMuonSimHitIterator;
typedef AtlasHitsVector<GenericMuonSimHit>::const_iterator GenericMuonSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF( GenericMuonSimHitCollection , 1155807089 , 1 )
CLASS_DEF( OldGenericMuonSimHitCollection , 97056587 , 1 )
#endif

#endif

