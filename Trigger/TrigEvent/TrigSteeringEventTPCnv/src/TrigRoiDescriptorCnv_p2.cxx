/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#undef private
#undef protected

#include <iostream>
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p2.h"


void TrigRoiDescriptorCnv_p2::persToTrans(const TrigRoiDescriptor_p2* persObj, 
				       TrigRoiDescriptor* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p2::persToTrans called " << endreq;

   transObj->m_phi0         = persObj->geom[phi]        ;                 
   transObj->m_eta0         = persObj->geom[eta]        ;                 
   transObj->m_zed0         = persObj->geom[zed]        ;                 
   transObj->m_phiHalfWidth = persObj->geom[wphi];         
   transObj->m_etaHalfWidth = persObj->geom[weta];         
   transObj->m_zedHalfWidth = persObj->geom[wzed];         
   transObj->m_etaPlus      = persObj->geom[etaplus]     ;         
   transObj->m_etaMinus     = persObj->geom[etaminus]    ;         
   
   transObj->m_roiId        = persObj->ids[roiid]       ;         
   transObj->m_roiWord      = persObj->ids[roiword]     ;   
    
}


void TrigRoiDescriptorCnv_p2::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p2::transToPers called " << endreq;

   persObj->geom[phi]         = transObj->m_phi0        ;                 
   persObj->geom[eta]         = transObj->m_eta0        ;                 
   persObj->geom[zed]         = transObj->m_zed0        ;                 
   persObj->geom[wphi] = transObj->m_phiHalfWidth;         
   persObj->geom[weta] = transObj->m_etaHalfWidth;
   persObj->geom[wzed] = transObj->m_zedHalfWidth;
   persObj->geom[etaplus]      = transObj->m_etaPlus     ;
   persObj->geom[etaminus]     = transObj->m_etaMinus    ;
         
   persObj->ids[roiid]        = transObj->m_roiId       ;         
   persObj->ids[roiword]      = transObj->m_roiWord     ;       
}
