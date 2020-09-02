/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocRecoEv/AFP_SIDLocRecoEvent.h"

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"

void AFP_SIDLocRecoEventCnv_p1::persToTrans(const AFP_SIDLocRecoEvent_p1* persObj, AFP_SIDLocRecoEvent* transObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_SIDLocRecoEventCnv_p1::persToTrans called " << endmsg;

        *transObj = AFP_SIDLocRecoEvent (persObj->m_iAlgoNum,
                                         persObj->m_nStationID,
                                         persObj->m_x,
                                         persObj->m_y,
                                         persObj->m_z,
                                         persObj->m_x_slope,
                                         persObj->m_y_slope,
                                         persObj->m_z_slope,	
                                         persObj->m_nHits,
                                         persObj->m_nHoles,
                                         persObj->m_fChi2);




}
 
 
void AFP_SIDLocRecoEventCnv_p1::transToPers(const AFP_SIDLocRecoEvent* transObj, AFP_SIDLocRecoEvent_p1* persObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_SIDLocRecoEventCnv_p1::transToPers called " << endmsg;

        persObj->m_iAlgoNum	= transObj->getAlgoNum();
	persObj->m_nStationID	= transObj->getStationID();
	
	persObj->m_x		= transObj->getXposition();
	persObj->m_y		= transObj->getYposition();
	persObj->m_z		= transObj->getZposition();
	persObj->m_x_slope	= transObj->getXslope();
	persObj->m_y_slope	= transObj->getYslope();
	persObj->m_z_slope	= transObj->getZslope();
	
	persObj->m_nHits	= transObj->getNHits();
	persObj->m_nHoles	= transObj->getNHoles();
	persObj->m_fChi2	= transObj->getChi2();

	
}
