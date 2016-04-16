/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include <iostream>
#include "TrigSteeringEventTPCnv/TrigOperationalInfo_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"


void TrigOperationalInfoCnv_p1::persToTrans(const TrigOperationalInfo_p1* persObj, 
				       TrigOperationalInfo* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigOperationalInfoCnv_p1::persToTrans called " << endreq;
   *transObj = TrigOperationalInfo (persObj->m_keys, persObj->m_values);
}


void TrigOperationalInfoCnv_p1::transToPers(const TrigOperationalInfo* transObj, 
				       TrigOperationalInfo_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigOperationalInfoCnv_p1::transToPers called " << endreq;
   auto infos = transObj->infos();
   persObj->m_keys         =  std::move(infos.first);
   persObj->m_values       =  std::move(infos.second);
}
