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
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"
#include "TrigSteeringEvent/PhiHelper.h"


void TrigRoiDescriptorCnv_p1::persToTrans(const TrigRoiDescriptor_p1* persObj, 
				       TrigRoiDescriptor* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p1::persToTrans called " << endreq;

#if 0
   transObj->m_phi      = persObj->m_phi0        ;                 
   transObj->m_eta      = persObj->m_eta0        ;                 
   transObj->m_zed      = persObj->m_zed0        ;                 
   transObj->m_phiMinus = persObj->m_phi0 - persObj->m_phiHalfWidth;         
   transObj->m_phiPlus  = persObj->m_phi0 + persObj->m_phiHalfWidth;         
   transObj->m_etaMinus = persObj->m_eta0 - persObj->m_etaHalfWidth;         
   transObj->m_etaPlus  = persObj->m_eta0 + persObj->m_etaHalfWidth;         
   transObj->m_zedMinus = persObj->m_zed0 - persObj->m_zedHalfWidth;         
   transObj->m_zedPlus  = persObj->m_zed0 + persObj->m_zedHalfWidth;         
#endif
   
   double _phi       = persObj->m_phi0        ;                 
   double _eta       = persObj->m_eta0        ;                 
   double _zed       = persObj->m_zed0        ;                 
   double _phiPlus   = persObj->m_phi0 + persObj->m_phiHalfWidth;         
   double _phiMinus  = persObj->m_phi0 - persObj->m_phiHalfWidth;         
   double _etaPlus   = persObj->m_eta0 + persObj->m_etaHalfWidth;                  
   double _etaMinus  = persObj->m_eta0 - persObj->m_etaHalfWidth;              
   double _zedPlus   = persObj->m_zed0 + persObj->m_zedHalfWidth;         
   double _zedMinus  = persObj->m_zed0 - persObj->m_zedHalfWidth;          

   transObj->m_l1Id         = persObj->m_l1Id        ;          
   transObj->m_roiId        = persObj->m_roiId       ;         
   transObj->m_roiWord      = persObj->m_roiWord     ;   
    
   transObj->m_serialized   = persObj->m_serialized  ;

   transObj->m_version  = 1;
   transObj->m_fullscan = false;

   /// now set up the variables
   transObj->construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ); 
}


void TrigRoiDescriptorCnv_p1::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p1::transToPers called " << endreq;

   persObj->m_phi0         = transObj->m_phi        ;                 
   persObj->m_eta0         = transObj->m_eta        ;                 
   persObj->m_zed0         = transObj->m_zed        ;                 
   persObj->m_phiHalfWidth = 0.5*std::fabs( HLT::wrapPhi(transObj->m_phiPlus-transObj->m_phiMinus) );
   persObj->m_etaHalfWidth = 0.5*std::fabs( transObj->m_etaPlus-transObj->m_etaMinus) ;
   persObj->m_zedHalfWidth = 0.5*std::fabs( transObj->m_zedPlus-transObj->m_zedMinus) ;
   persObj->m_etaPlus      = transObj->m_etaPlus     ;
   persObj->m_etaMinus     = transObj->m_etaMinus    ;
         
   persObj->m_l1Id         = transObj->m_l1Id        ;          
   persObj->m_roiId        = transObj->m_roiId       ;         
   persObj->m_roiWord      = transObj->m_roiWord     ;       
   persObj->m_serialized   = transObj->m_serialized  ;

}
