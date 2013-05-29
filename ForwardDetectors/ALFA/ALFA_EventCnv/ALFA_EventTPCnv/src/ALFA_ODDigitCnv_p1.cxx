/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_RawEv/ALFA_ODDigit.h"
#undef private
#undef protected


#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCnv_p1.h"

void ALFA_ODDigitCnv_p1::persToTrans(const ALFA_ODDigit_p1* persObj, ALFA_ODDigit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_ODDigitCnv_p1::persToTrans called " << endreq;
   
   transObj->m_plate       = persObj->m_plate;
   transObj->m_side        = persObj->m_side;   
   transObj->m_fiber       = persObj->m_fiber;
   transObj->m_station     = persObj->m_station;      
}
 
 
void ALFA_ODDigitCnv_p1::transToPers(const ALFA_ODDigit* transObj, 
                                       ALFA_ODDigit_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_ODDigitCnv_p1::transToPers called " << endreq;
    persObj->m_plate       = transObj->m_plate;
    persObj->m_side        = transObj->m_side;    
    persObj->m_fiber       = transObj->m_fiber;
    persObj->m_station     = transObj->m_station;
        
 }
