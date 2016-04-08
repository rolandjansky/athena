/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"
#include "ALFA_EventTPCnv/ALFA_GloRecEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_GloRecEventCnv_p1.h"



void ALFA_GloRecEventCnv_p1::persToTrans(const ALFA_GloRecEvent_p1* persObj, ALFA_GloRecEvent* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_GloRecEventCnv_p1::persToTrans called - version with pointers to LocRecCorr " << endreq;
   *transObj = ALFA_GloRecEvent (persObj->m_arm,
                                 persObj->m_x,
                                 persObj->m_y,
                                 persObj->m_x_slope,
                                 persObj->m_y_slope,
                                 nullptr,
                                 nullptr);
//   transObj->m_near_LocRecCorr = persObj->m_near_LocRecCorr;
   //transObj->m_far_LocRecCorr = persObj->m_far_LocRecCorr;

}
 
 
void ALFA_GloRecEventCnv_p1::transToPers(const ALFA_GloRecEvent* transObj, 
                                       ALFA_GloRecEvent_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_GloRecEventCnv_p1::transToPers called - version with pointers to LocRecCorr " << endreq;
   persObj->m_arm       = transObj->getArmNum();
   persObj->m_x         = transObj->getXposition();
   persObj->m_y         = transObj->getYposition();
   persObj->m_x_slope   = transObj->getXslope();
   persObj->m_y_slope   = transObj->getYslope();
//    persObj->m_near_LocRecCorr = transObj->m_near_LocRecCorr;
//    persObj->m_far_LocRecCorr = transObj->m_far_LocRecCorr;
        
}


