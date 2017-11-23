/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "AFP_LocRecoEv/AFP_SIDLocRecoEvent.h"
#undef private
#undef protected

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"

void AFP_SIDLocRecoEventCnv_p1::persToTrans(const AFP_SIDLocRecoEvent_p1* persObj, AFP_SIDLocRecoEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SIDLocRecoEventCnv_p1::persToTrans called " << endmsg;

	transObj->m_iAlgoNum	= persObj->m_iAlgoNum;
	transObj->m_nStationID	= persObj->m_nStationID;
	
	transObj->m_x		= persObj->m_x;
	transObj->m_y		= persObj->m_y;
	transObj->m_z		= persObj->m_z;
	transObj->m_x_slope	= persObj->m_x_slope;
	transObj->m_y_slope	= persObj->m_y_slope;
	transObj->m_z_slope	= persObj->m_z_slope;	
	
	transObj->m_nHits	= persObj->m_nHits;
	transObj->m_nHoles	= persObj->m_nHoles;
	transObj->m_fChi2	= persObj->m_fChi2;

}
 
 
void AFP_SIDLocRecoEventCnv_p1::transToPers(const AFP_SIDLocRecoEvent* transObj, AFP_SIDLocRecoEvent_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SIDLocRecoEventCnv_p1::transToPers called " << endmsg;

	persObj->m_iAlgoNum	= transObj->m_iAlgoNum;
	persObj->m_nStationID	= transObj->m_nStationID;
	
	persObj->m_x		= transObj->m_x;
	persObj->m_y		= transObj->m_y;
	persObj->m_z		= transObj->m_z;
	persObj->m_x_slope	= transObj->m_x_slope;
	persObj->m_y_slope	= transObj->m_y_slope;
	persObj->m_z_slope	= transObj->m_z_slope;	
	
	persObj->m_nHits	= transObj->m_nHits;
	persObj->m_nHoles	= transObj->m_nHoles;
	persObj->m_fChi2	= transObj->m_fChi2;
	
}
