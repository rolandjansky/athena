/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEventCnv_p1.h"

void ALFA_LocRecEventCnv_p1::persToTrans(const ALFA_LocRecEvent_p1* persObj, ALFA_LocRecEvent* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_LocRecEventCnv_p1::persToTrans called " << endreq;

   *transObj = ALFA_LocRecEvent (persObj->m_iAlgoNum,
                                 persObj->m_pot_num,
                                 persObj->m_x,
                                 persObj->m_y,
                                 persObj->m_fOverU,
                                 persObj->m_fOverV,
                                 persObj->m_iNumU,
                                 persObj->m_iNumV,
                                 persObj->m_iFibSel);
}
 
 
void ALFA_LocRecEventCnv_p1::transToPers(const ALFA_LocRecEvent* transObj, 
                                       ALFA_LocRecEvent_p1* persObj, 
                                       MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->getAlgoNum();
	persObj->m_pot_num   = transObj->getPotNum();
        persObj->m_x         = transObj->getXposition();
        persObj->m_y         = transObj->getYposition();
	persObj->m_fOverU    = transObj->getOverU();
	persObj->m_fOverV    = transObj->getOverV();
        persObj->m_iNumU     = transObj->getNumU();
	persObj->m_iNumV     = transObj->getNumV();
	persObj->m_iFibSel   = transObj->getFibSel();
 }
