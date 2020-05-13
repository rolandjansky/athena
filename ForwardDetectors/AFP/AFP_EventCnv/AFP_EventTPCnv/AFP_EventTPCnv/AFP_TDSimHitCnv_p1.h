/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHitCnv_p1_h
#define AFP_TDSimHitCnv_p1_h

#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_EventTPCnv/AFP_TDSimHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class AFP_TDSimHitCnv_p1 : public T_AthenaPoolTPCnvConstBase< AFP_TDSimHit, AFP_TDSimHit_p1> 
{
public:

        using base_class::transToPers;
        using base_class::persToTrans;



	AFP_TDSimHitCnv_p1() {}

	virtual void persToTrans( const AFP_TDSimHit_p1* persObj, AFP_TDSimHit* transObj, MsgStream &log) const override;
	virtual void transToPers( const AFP_TDSimHit* transObj, AFP_TDSimHit_p1* persObj, MsgStream &log) const override;
};
 
 
#endif
