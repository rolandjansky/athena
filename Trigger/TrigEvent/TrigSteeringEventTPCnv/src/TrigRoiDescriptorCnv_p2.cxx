/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
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

   double phi       = persObj->geom[PHI]        ;                 
   double eta       = persObj->geom[ETA]        ;                 
   double zed       = persObj->geom[ZED]        ;                 
   double phiPlus   = persObj->geom[PHI] + persObj->geom[WPHI];         
   double phiMinus  = persObj->geom[PHI] - persObj->geom[WPHI];         
   double etaPlus   = persObj->geom[ETA] + persObj->geom[WETA];         
   double etaMinus  = persObj->geom[ETA] - persObj->geom[WETA];         
   double zedPlus   = persObj->geom[ZED] + persObj->geom[WZED];         
   double zedMinus  = persObj->geom[ZED] - persObj->geom[WZED];         
   
   *transObj = TrigRoiDescriptor (persObj->ids[ROIWORD],
                                  0,
                                  persObj->ids[ROIID],
                                  eta, etaMinus, etaPlus,
                                  phi, phiMinus, phiPlus,
                                  zed, zedMinus, zedPlus );

   transObj->version (2);
}


void TrigRoiDescriptorCnv_p2::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p2::transToPers called " << endreq;
   //   log << MSG::FATAL << "TrigRoiDescriptorCnv_p1::transToPers called - do not use this converter, use more recent converter" << endreq;  

   persObj->geom[PHI]      = transObj->phi()        ;                 
   persObj->geom[ETA]      = transObj->eta()        ;                 
   persObj->geom[ZED]      = transObj->zed()        ;  
               
   if ( transObj->phiPlus() > transObj->phiMinus() )
     persObj->geom[WPHI] = 0.5*(transObj->phiPlus() - transObj->phiMinus());         
   else
     persObj->geom[WPHI] = 0.5*(transObj->phiPlus() - transObj->phiMinus()) + M_PI;

   persObj->geom[WETA]     = 0.5*std::fabs( transObj->etaPlus() - transObj->etaMinus() );         
   persObj->geom[WZED]     = 0.5*std::fabs( transObj->zedPlus() - transObj->zedMinus() );         
   persObj->geom[ETAPLUS]  = transObj->etaPlus()     ;
   persObj->geom[ETAMINUS] = transObj->etaMinus()    ;
         
   persObj->ids[ROIID]     = transObj->roiId()       ;         
   persObj->ids[ROIWORD]   = transObj->roiWord()     ;       
}

