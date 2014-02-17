/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHitCnv_p1_h
#define SimulationHitCnv_p1_h

#include "ForwardRegion_SimEv/SimulationHit.h"
#include "ForwardRegion_EventTPCnv/SimulationHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class SimulationHitCnv_p1 : public T_AthenaPoolTPCnvBase< SimulationHit, SimulationHit_p1> 
{
public:
	SimulationHitCnv_p1() {}

	virtual void persToTrans( const SimulationHit_p1* persObj, SimulationHit* transObj, MsgStream &log);
	virtual void transToPers( const SimulationHit* transObj, SimulationHit_p1* persObj, MsgStream &log);
};
 
 
#endif
