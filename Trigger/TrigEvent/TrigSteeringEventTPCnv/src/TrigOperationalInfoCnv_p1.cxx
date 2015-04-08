/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#undef private
#undef protected

#include <iostream>
#include "TrigSteeringEventTPCnv/TrigOperationalInfo_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"


void TrigOperationalInfoCnv_p1::persToTrans(const TrigOperationalInfo_p1* persObj, 
				       TrigOperationalInfo* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigOperationalInfoCnv_p1::persToTrans called " << endreq;
   transObj->m_infoName       = persObj->m_keys;
   transObj->m_infoValue      = persObj->m_values;

}


void TrigOperationalInfoCnv_p1::transToPers(const TrigOperationalInfo* transObj, 
				       TrigOperationalInfo_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigOperationalInfoCnv_p1::transToPers called " << endreq;
   persObj->m_keys         =  transObj->m_infoName;
   persObj->m_values       = transObj->m_infoValue;
}
