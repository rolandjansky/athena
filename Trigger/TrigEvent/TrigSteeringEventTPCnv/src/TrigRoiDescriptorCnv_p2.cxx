/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#undef private
#undef protected

#include <iostream>
#include <cmath>
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p2.h"
#include "TrigSteeringEvent/PhiHelper.h"


void TrigRoiDescriptorCnv_p2::persToTrans(const TrigRoiDescriptor_p2* persObj, 
				       TrigRoiDescriptor* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p2::persToTrans called " << endreq;

   double _phi       = persObj->geom[phi]        ;                 
   double _eta       = persObj->geom[eta]        ;                 
   double _zed       = persObj->geom[zed]        ;                 
   double _phiPlus   = persObj->geom[phi] + persObj->geom[wphi];         
   double _phiMinus  = persObj->geom[phi] - persObj->geom[wphi];         
   double _etaPlus   = persObj->geom[eta] + persObj->geom[weta];         
   double _etaMinus  = persObj->geom[eta] - persObj->geom[weta];         
   double _zedPlus   = persObj->geom[zed] + persObj->geom[wzed];         
   double _zedMinus  = persObj->geom[zed] - persObj->geom[wzed];         
   
   transObj->m_roiId        = persObj->ids[roiid]       ;         
   transObj->m_roiWord      = persObj->ids[roiword]     ;   
    
   transObj->m_fullscan = false;
   transObj->m_version  = 2;

   /// now set up the variables
   transObj->construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ); 

}


void TrigRoiDescriptorCnv_p2::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p2::transToPers called " << endreq;

   persObj->geom[phi]      = transObj->m_phi        ;                 
   persObj->geom[eta]      = transObj->m_eta        ;                 
   persObj->geom[zed]      = transObj->m_zed        ;                 
   persObj->geom[wphi]     = 0.5*std::fabs( HLT::wrapPhi(transObj->m_phiPlus - transObj->m_phiMinus) );         
   persObj->geom[weta]     = 0.5*std::fabs( transObj->m_etaPlus - transObj->m_etaMinus );         
   persObj->geom[wzed]     = 0.5*std::fabs( transObj->m_zedPlus - transObj->m_zedMinus );         
   persObj->geom[etaplus]  = transObj->m_etaPlus     ;
   persObj->geom[etaminus] = transObj->m_etaMinus    ;
         
   persObj->ids[roiid]     = transObj->m_roiId       ;         
   persObj->ids[roiword]   = transObj->m_roiWord     ;       
}

