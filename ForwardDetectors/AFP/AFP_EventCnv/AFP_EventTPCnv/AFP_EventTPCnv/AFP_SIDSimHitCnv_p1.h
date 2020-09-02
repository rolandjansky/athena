/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDSimHitCnv_p1_h
#define AFP_SIDSimHitCnv_p1_h

#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "AFP_EventTPCnv/AFP_SIDSimHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class AFP_SIDSimHitCnv_p1 : public T_AthenaPoolTPCnvConstBase< AFP_SIDSimHit, AFP_SIDSimHit_p1> 
{
public:

       using base_class::transToPers;
       using base_class::persToTrans;



	AFP_SIDSimHitCnv_p1() {}

	virtual void persToTrans( const AFP_SIDSimHit_p1* persObj, AFP_SIDSimHit* transObj, MsgStream &log) const override;
	virtual void transToPers( const AFP_SIDSimHit* transObj, AFP_SIDSimHit_p1* persObj, MsgStream &log) const override;
};
 
 
#endif
