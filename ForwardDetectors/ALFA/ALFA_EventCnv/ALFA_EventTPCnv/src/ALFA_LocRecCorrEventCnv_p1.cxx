/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"
#undef private
#undef protected

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEventCnv_p1.h"

void ALFA_LocRecCorrEventCnv_p1::persToTrans(const ALFA_LocRecCorrEvent_p1* persObj, ALFA_LocRecCorrEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrEventCnv_p1::persToTrans called " << endreq;

	transObj->m_iAlgoNum  = persObj->m_iAlgoNum;
	transObj->m_pot_num   = persObj->m_pot_num;
	transObj->m_xLHC      = persObj->m_xLHC;
	transObj->m_yLHC      = persObj->m_yLHC;
	transObj->m_zLHC      = persObj->m_zLHC;
	transObj->m_xPot      = persObj->m_xPot;
	transObj->m_yPot      = persObj->m_yPot;
	transObj->m_xStat     = persObj->m_xStat;
	transObj->m_yStat     = persObj->m_yStat;
	transObj->m_xBeam     = persObj->m_xBeam;
	transObj->m_yBeam     = persObj->m_yBeam;
}
 
 
void ALFA_LocRecCorrEventCnv_p1::transToPers(const ALFA_LocRecCorrEvent* transObj, 
                                       ALFA_LocRecCorrEvent_p1* persObj, 
                                       MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->m_iAlgoNum;
	persObj->m_pot_num   = transObj->m_pot_num;
	persObj->m_xLHC      = transObj->m_xLHC;
	persObj->m_yLHC      = transObj->m_yLHC;
	persObj->m_zLHC      = transObj->m_zLHC;
	persObj->m_xPot      = transObj->m_xPot;
	persObj->m_yPot      = transObj->m_yPot;
	persObj->m_xStat     = transObj->m_xStat;
	persObj->m_yStat     = transObj->m_yStat;
	persObj->m_xBeam     = transObj->m_xBeam;
	persObj->m_yBeam     = transObj->m_yBeam;
 }
