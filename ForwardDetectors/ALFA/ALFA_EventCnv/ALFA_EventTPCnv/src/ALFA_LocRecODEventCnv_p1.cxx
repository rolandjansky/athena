/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"
#undef private
#undef protected

#include "ALFA_EventTPCnv/ALFA_LocRecODEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEventCnv_p1.h"

void ALFA_LocRecODEventCnv_p1::persToTrans(const ALFA_LocRecODEvent_p1* persObj, ALFA_LocRecODEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecODEventCnv_p1::persToTrans called " << endreq;

	transObj->m_iAlgoNum  = persObj->m_iAlgoNum;
	transObj->m_pot_num   = persObj->m_pot_num;
	transObj->m_side      = persObj->m_side;
	transObj->m_y         = persObj->m_y;
	transObj->m_fOverY    = persObj->m_fOverY;
	transObj->m_iNumY     = persObj->m_iNumY;
	transObj->m_iFibSel   = persObj->m_iFibSel;
}
 
 
void ALFA_LocRecODEventCnv_p1::transToPers(const ALFA_LocRecODEvent* transObj, ALFA_LocRecODEvent_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecODEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->m_iAlgoNum;
	persObj->m_pot_num   = transObj->m_pot_num;
	persObj->m_side      = transObj->m_side;
	persObj->m_y         = transObj->m_y;
	persObj->m_fOverY    = transObj->m_fOverY;
	persObj->m_iNumY     = transObj->m_iNumY;
	persObj->m_iFibSel   = transObj->m_iFibSel;
}
