/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEventCnv_p1.h"

void ALFA_LocRecODEventCnv_p1::persToTrans(const ALFA_LocRecODEvent_p1* persObj, ALFA_LocRecODEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecODEventCnv_p1::persToTrans called " << endreq;

        *transObj = ALFA_LocRecODEvent (persObj->m_iAlgoNum,
                                        persObj->m_pot_num,
                                        persObj->m_side,
                                        persObj->m_y,
                                        persObj->m_fOverY,
                                        persObj->m_iNumY,
                                        persObj->m_iFibSel);
}
 
 
void ALFA_LocRecODEventCnv_p1::transToPers(const ALFA_LocRecODEvent* transObj, ALFA_LocRecODEvent_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecODEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->getAlgoNum();
	persObj->m_pot_num   = transObj->getPotNum();
	persObj->m_side      = transObj->getSide();
	persObj->m_y         = transObj->getYposition();
	persObj->m_fOverY    = transObj->getOverY();
	persObj->m_iNumY     = transObj->getNumY();
	persObj->m_iFibSel   = transObj->getFibSel();
}
