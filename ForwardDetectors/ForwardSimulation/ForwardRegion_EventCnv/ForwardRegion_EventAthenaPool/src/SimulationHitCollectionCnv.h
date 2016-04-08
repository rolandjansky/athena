/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHitCollectionCnv_h
#define SimulationHitCollectionCnv_h

#include "ForwardRegion_SimEv/SimulationHitCollection.h"
#include "ForwardRegion_EventTPCnv/SimulationHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

typedef SimulationHitCollection_p1  SimulationHitCollection_PERS;

class SimulationHitCollectionCnv : public T_AthenaPoolCustomCnv <SimulationHitCollection, SimulationHitCollection_PERS>
{
	friend class CnvFactory<SimulationHitCollectionCnv>;

protected:
	SimulationHitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <SimulationHitCollection, SimulationHitCollection_PERS >(svcloc) {}

	SimulationHitCollection_PERS* createPersistent (SimulationHitCollection *transCont);
	SimulationHitCollection* createTransient ();
 };

#endif //SimulationHitCollectionCnv_h
