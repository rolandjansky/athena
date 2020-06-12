/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SiDigiCnv_p1_h
#define AFP_SiDigiCnv_p1_h

#include "AFP_DigiEv/AFP_SiDigi.h"
#include "AFP_EventTPCnv/AFP_SiDigi_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class AFP_SiDigiCnv_p1 : public T_AthenaPoolTPCnvConstBase< AFP_SiDigi, AFP_SiDigi_p1> 
{
public:

       using base_class::transToPers;
       using base_class::persToTrans;



	AFP_SiDigiCnv_p1() {}

	virtual void persToTrans( const AFP_SiDigi_p1* persObj, AFP_SiDigi* transObj, MsgStream &log) const override;
	virtual void transToPers( const AFP_SiDigi* transObj, AFP_SiDigi_p1* persObj, MsgStream &log) const override;
};
 
 
#endif
