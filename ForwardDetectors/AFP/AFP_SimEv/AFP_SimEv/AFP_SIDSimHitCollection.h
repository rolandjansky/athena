/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDSimHitCollection_h
#define AFP_SIDSimHitCollection_h

#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif //__CINT__

typedef AtlasHitsVector<AFP_SIDSimHit> AFP_SIDSimHitCollection;
typedef AtlasHitsVector<AFP_SIDSimHit>::iterator AFP_SIDSimHitIter;
typedef AtlasHitsVector<AFP_SIDSimHit>::const_iterator AFP_SIDSimHitConstIter;

#ifndef __CINT__
CLASS_DEF(AFP_SIDSimHitCollection, 211112378, 1) // from where the number is coming? CLID <class name>
#endif //__CINT__

#endif //AFP_SIDSimHitCollection_h

