/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHitCollection_h
#define AFP_TDSimHitCollection_h


#include "AFP_SimEv/AFP_TDSimHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif //__CINT__

typedef AtlasHitsVector<AFP_TDSimHit> AFP_TDSimHitCollection;
typedef AtlasHitsVector<AFP_TDSimHit>::iterator AFP_TDSimHitIter;
typedef AtlasHitsVector<AFP_TDSimHit>::const_iterator AFP_TDSimHitConstIter;

#ifndef __CINT__
CLASS_DEF(AFP_TDSimHitCollection, 1183496046, 1) // from where the number is coming? CLID <class name>
#endif //__CINT__

#endif //AFP_TDSimHitCollection_h
