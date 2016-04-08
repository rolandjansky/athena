/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_ODDigit.h"
#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCnv_p1.h"

void ALFA_ODDigitCnv_p1::persToTrans(const ALFA_ODDigit_p1* persObj, ALFA_ODDigit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_ODDigitCnv_p1::persToTrans called " << endreq;

   *transObj = ALFA_ODDigit (persObj->m_station,
                             persObj->m_side,
                             persObj->m_plate,
                             persObj->m_fiber);
}
 
 
void ALFA_ODDigitCnv_p1::transToPers(const ALFA_ODDigit* transObj, 
                                       ALFA_ODDigit_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_ODDigitCnv_p1::transToPers called " << endreq;
   persObj->m_plate       = transObj->getPlate();
   persObj->m_side        = transObj->getSide();    
   persObj->m_fiber       = transObj->getFiber();
   persObj->m_station     = transObj->getStation();
        
 }
