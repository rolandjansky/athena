/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHitCnv_p1_h
#define AFP_TDSimHitCnv_p1_h

#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_EventTPCnv/AFP_TDSimHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class AFP_TDSimHitCnv_p1 : public T_AthenaPoolTPCnvBase< AFP_TDSimHit, AFP_TDSimHit_p1> 
{
public:
	AFP_TDSimHitCnv_p1() {}

	virtual void persToTrans( const AFP_TDSimHit_p1* persObj, AFP_TDSimHit* transObj, MsgStream &log);
	virtual void transToPers( const AFP_TDSimHit* transObj, AFP_TDSimHit_p1* persObj, MsgStream &log);
};
 
 
#endif
