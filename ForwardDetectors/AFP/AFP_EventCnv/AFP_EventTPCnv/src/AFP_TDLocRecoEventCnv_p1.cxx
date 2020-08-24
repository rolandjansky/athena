/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"

#include "AFP_EventTPCnv/AFP_TDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEventCnv_p1.h"

void AFP_TDLocRecoEventCnv_p1::persToTrans(const AFP_TDLocRecoEvent_p1* persObj, AFP_TDLocRecoEvent* transObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_TDLocRecoEventCnv_p1::persToTrans called " << endmsg;

        *transObj = AFP_TDLocRecoEvent (persObj->m_iAlgoNum,
                                        persObj->m_nStationID,
                                        persObj->m_nDetectorID,
                                        persObj->m_nTrainID,
                                        persObj->m_fTrainTime,
                                        persObj->m_nTrainSize,
                                        persObj->m_nSaturation);


}
 
 
void AFP_TDLocRecoEventCnv_p1::transToPers(const AFP_TDLocRecoEvent* transObj, AFP_TDLocRecoEvent_p1* persObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_TDLocRecoEventCnv_p1::transToPers called " << endmsg;

        persObj->m_iAlgoNum	= transObj->getAlgoNum();
	persObj->m_nStationID	= transObj->getStationID();
	persObj->m_nDetectorID	= transObj->getDetectorID();
	persObj->m_nTrainID	= transObj->getTrainID();	
	persObj->m_fTrainTime	= transObj->getTrainTime();
	persObj->m_nTrainSize	= transObj->getTrainSize();
	persObj->m_nSaturation	= transObj->getSaturation();




}
