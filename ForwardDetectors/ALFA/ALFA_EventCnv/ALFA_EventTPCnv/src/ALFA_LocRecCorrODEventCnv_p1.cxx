/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvent.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEventCnv_p1.h"

void ALFA_LocRecCorrODEventCnv_p1::persToTrans(const ALFA_LocRecCorrODEvent_p1* persObj, ALFA_LocRecCorrODEvent* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrODEventCnv_p1::persToTrans called " << endreq;

        *transObj = ALFA_LocRecCorrODEvent(persObj->m_iAlgoNum,
                                           persObj->m_pot_num,
                                           persObj->m_side,
                                           persObj->m_yLHC,
                                           persObj->m_zLHC,
                                           persObj->m_yPot,
                                           persObj->m_yStat,
                                           persObj->m_yBeam);
}


void ALFA_LocRecCorrODEventCnv_p1::transToPers(const ALFA_LocRecCorrODEvent* transObj, ALFA_LocRecCorrODEvent_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "ALFA_LocRecCorrODEventCnv_p1::transToPers called " << endreq;

	persObj->m_iAlgoNum  = transObj->getAlgoNum();
	persObj->m_pot_num   = transObj->getPotNum();
	persObj->m_side      = transObj->getSide();
	persObj->m_yLHC      = transObj->getYpositionLHC();
	persObj->m_zLHC      = transObj->getZpositionLHC();
	persObj->m_yPot      = transObj->getYpositionPot();
	persObj->m_yStat     = transObj->getYpositionStat();
	persObj->m_yBeam     = transObj->getYpositionBeam();
 }
