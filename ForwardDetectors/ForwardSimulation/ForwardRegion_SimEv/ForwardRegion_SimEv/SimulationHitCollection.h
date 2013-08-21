/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHitCollection_h
#define SimulationHitCollection_h


#include "ForwardRegion_SimEv/SimulationHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif //__CINT__

typedef AtlasHitsVector<SimulationHit> SimulationHitCollection;
typedef AtlasHitsVector<SimulationHit>::iterator SimulationHitIter;
typedef AtlasHitsVector<SimulationHit>::const_iterator SimulationHitConstIter;

#ifndef __CINT__
CLASS_DEF(SimulationHitCollection, 1245677875, 1) // from where the number is coming? CLID <class name>
#endif //__CINT__

#endif 
