/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_Digit.h"
#include "ALFA_EventTPCnv/ALFA_Digit_p1.h"
#include "ALFA_EventTPCnv/ALFA_DigitCnv_p1.h"

void ALFA_DigitCnv_p1::persToTrans(const ALFA_Digit_p1* persObj, ALFA_Digit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_DigitCnv_p1::persToTrans called " << endreq;

   *transObj = ALFA_Digit (persObj->m_station,
                           persObj->m_plate,
                           persObj->m_fiber);
}
 
 
void ALFA_DigitCnv_p1::transToPers(const ALFA_Digit* transObj, 
                                       ALFA_Digit_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_DigitCnv_p1::transToPers called " << endreq;
    persObj->m_plate       = transObj->getPlate();
    persObj->m_fiber       = transObj->getFiber();
    persObj->m_station     = transObj->getStation();
        
 }
