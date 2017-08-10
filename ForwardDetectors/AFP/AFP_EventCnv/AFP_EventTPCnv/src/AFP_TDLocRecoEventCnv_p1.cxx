/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"
#undef private
#undef protected

#include "AFP_EventTPCnv/AFP_TDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEventCnv_p1.h"

void AFP_TDLocRecoEventCnv_p1::persToTrans(const AFP_TDLocRecoEvent_p1* persObj, AFP_TDLocRecoEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_TDLocRecoEventCnv_p1::persToTrans called " << endmsg;

	transObj->m_iAlgoNum	= persObj->m_iAlgoNum;
	transObj->m_nStationID	= persObj->m_nStationID;
	transObj->m_nDetectorID	= persObj->m_nDetectorID;
	transObj->m_nTrainID	= persObj->m_nTrainID;	
	transObj->m_fTrainTime	= persObj->m_fTrainTime;
	transObj->m_nTrainSize	= persObj->m_nTrainSize;
	transObj->m_nSaturation	= persObj->m_nSaturation;

}
 
 
void AFP_TDLocRecoEventCnv_p1::transToPers(const AFP_TDLocRecoEvent* transObj, AFP_TDLocRecoEvent_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_TDLocRecoEventCnv_p1::transToPers called " << endmsg;

	persObj->m_iAlgoNum	= transObj->m_iAlgoNum;
	persObj->m_nStationID	= transObj->m_nStationID;
	persObj->m_nDetectorID	= transObj->m_nDetectorID;
	persObj->m_nTrainID	= transObj->m_nTrainID;	
	persObj->m_fTrainTime	= transObj->m_fTrainTime;
	persObj->m_nTrainSize	= transObj->m_nTrainSize;
	persObj->m_nSaturation	= transObj->m_nSaturation;
}
