/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#undef private
#undef protected

#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEventCnv_p1.h"

void ALFA_LocRecEventCnv_p1::persToTrans(const ALFA_LocRecEvent_p1* persObj, ALFA_LocRecEvent* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_LocRecEventCnv_p1::persToTrans called " << endreq;
   
   transObj->m_iAlgoNum  = persObj->m_iAlgoNum;
   transObj->m_pot_num   = persObj->m_pot_num;
   transObj->m_x         = persObj->m_x;
   transObj->m_y         = persObj->m_y;
   transObj->m_fOverU    = persObj->m_fOverU;
   transObj->m_fOverV    = persObj->m_fOverV;
   transObj->m_iNumU     = persObj->m_iNumU;
   transObj->m_iNumV     = persObj->m_iNumV;
   transObj->m_iFibSel   = persObj->m_iFibSel;
}
 
 
void ALFA_LocRecEventCnv_p1::transToPers(const ALFA_LocRecEvent* transObj, 
                                       ALFA_LocRecEvent_p1* persObj, 
                                       MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->m_iAlgoNum;
	persObj->m_pot_num   = transObj->m_pot_num;
    persObj->m_x         = transObj->m_x;
    persObj->m_y         = transObj->m_y;
	persObj->m_fOverU    = transObj->m_fOverU;
	persObj->m_fOverV    = transObj->m_fOverV;
	persObj->m_iNumU     = transObj->m_iNumU;
	persObj->m_iNumV     = transObj->m_iNumV;
	persObj->m_iFibSel   = transObj->m_iFibSel;
 }
