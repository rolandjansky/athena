/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigiCnv_p1_h
#define AFP_TDDigiCnv_p1_h

#include "AFP_DigiEv/AFP_TDDigi.h"
#include "AFP_EventTPCnv/AFP_TDDigi_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class AFP_TDDigiCnv_p1 : public T_AthenaPoolTPCnvBase< AFP_TDDigi, AFP_TDDigi_p1> 
{
public:
	AFP_TDDigiCnv_p1() {}

	virtual void persToTrans( const AFP_TDDigi_p1* persObj, AFP_TDDigi* transObj, MsgStream &log);
	virtual void transToPers( const AFP_TDDigi* transObj, AFP_TDDigi_p1* persObj, MsgStream &log);
};
 
 
#endif
