/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
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

   double phi       = persObj->m_phi0        ;                 
   double eta       = persObj->m_eta0        ;                 
   double zed       = persObj->m_zed0        ;                 
   double phiPlus   = persObj->m_phi0 + persObj->m_phiHalfWidth;         
   double phiMinus  = persObj->m_phi0 - persObj->m_phiHalfWidth;         
   double etaPlus   = persObj->m_eta0 + persObj->m_etaHalfWidth;                  
   double etaMinus  = persObj->m_eta0 - persObj->m_etaHalfWidth;              
   double zedPlus   = persObj->m_zed0 + persObj->m_zedHalfWidth;         
   double zedMinus  = persObj->m_zed0 - persObj->m_zedHalfWidth;          

   *transObj = TrigRoiDescriptor (persObj->m_roiWord,
                                  persObj->m_l1Id,
                                  persObj->m_roiId,
                                  eta, etaMinus, etaPlus,
                                  phi, phiMinus, phiPlus,
                                  zed, zedMinus, zedPlus );
   transObj->version (1);
}


void TrigRoiDescriptorCnv_p1::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p1::transToPers called " << endreq;
   //   log << MSG::FATAL << "TrigRoiDescriptorCnv_p1::transToPers called - do not use this converter, use more recent converter" << endreq;

   persObj->m_phi0         = transObj->phi()        ;                 
   persObj->m_eta0         = transObj->eta()        ;                 
   persObj->m_zed0         = transObj->zed()        ;               

   if ( transObj->phiPlus() > transObj->phiMinus() )
     persObj->m_phiHalfWidth = 0.5*( transObj->phiPlus()-transObj->phiMinus());
   else
     persObj->m_phiHalfWidth = 0.5*( transObj->phiPlus()-transObj->phiMinus()) + M_PI;

   persObj->m_etaHalfWidth = 0.5*std::fabs( transObj->etaPlus()-transObj->etaMinus()) ;
   persObj->m_zedHalfWidth = 0.5*std::fabs( transObj->zedPlus()-transObj->zedMinus()) ;
   persObj->m_etaPlus      = transObj->etaPlus()     ;
   persObj->m_etaMinus     = transObj->etaMinus()    ;
         
   persObj->m_l1Id         = transObj->l1Id()        ;          
   persObj->m_roiId        = transObj->roiId()       ;         
   persObj->m_roiWord      = transObj->roiWord()     ;       
   //persObj->m_serialized   = transObj->m_serialized  ;
}
