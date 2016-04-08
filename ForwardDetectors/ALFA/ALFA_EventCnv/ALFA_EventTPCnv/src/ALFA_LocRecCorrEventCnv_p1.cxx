/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEventCnv_p1.h"

void ALFA_LocRecCorrEventCnv_p1::persToTrans(const ALFA_LocRecCorrEvent_p1* persObj, ALFA_LocRecCorrEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrEventCnv_p1::persToTrans called " << endreq;

        *transObj = ALFA_LocRecCorrEvent (persObj->m_iAlgoNum,
                                          persObj->m_pot_num,
                                          persObj->m_xLHC,
                                          persObj->m_yLHC,
                                          persObj->m_zLHC,
                                          persObj->m_xPot,
                                          persObj->m_yPot,
                                          persObj->m_xStat,
                                          persObj->m_yStat,
                                          persObj->m_xBeam,
                                          persObj->m_yBeam);
}
 
 
void ALFA_LocRecCorrEventCnv_p1::transToPers(const ALFA_LocRecCorrEvent* transObj, 
                                       ALFA_LocRecCorrEvent_p1* persObj, 
                                       MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->getAlgoNum();
	persObj->m_pot_num   = transObj->getPotNum();
	persObj->m_xLHC      = transObj->getXpositionLHC();
	persObj->m_yLHC      = transObj->getYpositionLHC();
	persObj->m_zLHC      = transObj->getZpositionLHC();
	persObj->m_xPot      = transObj->getXpositionPot();
	persObj->m_yPot      = transObj->getYpositionPot();
	persObj->m_xStat     = transObj->getXpositionStat();
	persObj->m_yStat     = transObj->getYpositionStat();
	persObj->m_xBeam     = transObj->getXpositionBeam();
	persObj->m_yBeam     = transObj->getYpositionBeam();
 }
