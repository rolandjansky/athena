/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#undef private
#undef protected

#include <iostream>
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"


void TrigRoiDescriptorCnv_p1::persToTrans(const TrigRoiDescriptor_p1* persObj, 
				       TrigRoiDescriptor* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p1::persToTrans called " << endreq;

   transObj->m_phi0         = persObj->m_phi0        ;                 
   transObj->m_eta0         = persObj->m_eta0        ;                 
   transObj->m_zed0         = persObj->m_zed0        ;                 
   transObj->m_phiHalfWidth = persObj->m_phiHalfWidth;         
   transObj->m_etaHalfWidth = persObj->m_etaHalfWidth;         
   transObj->m_zedHalfWidth = persObj->m_zedHalfWidth;         
   transObj->m_etaPlus      = persObj->m_etaPlus     ;         
   transObj->m_etaMinus     = persObj->m_etaMinus    ;         
   
   transObj->m_l1Id         = persObj->m_l1Id        ;          
   transObj->m_roiId        = persObj->m_roiId       ;         
   transObj->m_roiWord      = persObj->m_roiWord     ;   
    
   transObj->m_serialized   = persObj->m_serialized  ;


}


void TrigRoiDescriptorCnv_p1::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p1::transToPers called " << endreq;

   persObj->m_phi0         = transObj->m_phi0        ;                 
   persObj->m_eta0         = transObj->m_eta0        ;                 
   persObj->m_zed0         = transObj->m_zed0        ;                 
   persObj->m_phiHalfWidth = transObj->m_phiHalfWidth;         
   persObj->m_etaHalfWidth = transObj->m_etaHalfWidth;
   persObj->m_zedHalfWidth = transObj->m_zedHalfWidth;
   persObj->m_etaPlus      = transObj->m_etaPlus     ;
   persObj->m_etaMinus     = transObj->m_etaMinus    ;
         
   persObj->m_l1Id         = transObj->m_l1Id        ;          
   persObj->m_roiId        = transObj->m_roiId       ;         
   persObj->m_roiWord      = transObj->m_roiWord     ;       
   persObj->m_serialized   = transObj->m_serialized  ;

}
