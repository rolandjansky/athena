/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <iostream>
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"


void TrigRoiDescriptorCnv_p3::persToTrans(const TrigRoiDescriptor_p3* persObj, 
					  TrigRoiDescriptor* transObj, 
					  MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p3::persToTrans called " << endreq;

   double phi       = persObj->geom[PHI]        ;                 
   double eta       = persObj->geom[ETA]        ;                 
   double zed       = persObj->geom[ZED]        ;                 
   double phiPlus   = persObj->geom[PHIPLUS]    ;
   double phiMinus  = persObj->geom[PHIMINUS]   ;
   double etaPlus   = persObj->geom[ETAPLUS]    ;
   double etaMinus  = persObj->geom[ETAMINUS]   ;
   double zedPlus   = persObj->geom[ZEDPLUS]    ;
   double zedMinus  = persObj->geom[ZEDMINUS]   ;
   
   *transObj = TrigRoiDescriptor (persObj->ids[ROIWORD],
                                  0,
                                  persObj->ids[ROIID],
                                  eta, etaMinus, etaPlus,
                                  phi, phiMinus, phiPlus,
                                  zed, zedMinus, zedPlus );

   transObj->m_fullscan  = persObj->fullscan   ;

   if ( persObj->rois.size()>0 ) {
     
     transObj->manageConstituents (true);

     for (const std::vector<float>& roi : persObj->rois) {
       transObj->push_back( new TrigRoiDescriptor( roi[ETA], roi[ETAMINUS], roi[ETAPLUS],
						   roi[PHI], roi[PHIMINUS], roi[PHIPLUS],
						   roi[ZED], roi[ZEDMINUS], roi[ZEDPLUS] ) );
     } 
   } 

}


void TrigRoiDescriptorCnv_p3::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p3::transToPers called " << endreq;

   persObj->geom[PHI]      = transObj->phi()        ;
   persObj->geom[ETA]      = transObj->eta()        ;
   persObj->geom[ZED]      = transObj->zed()        ;
   persObj->geom[PHIPLUS]  = transObj->phiPlus()    ;
   persObj->geom[PHIMINUS] = transObj->phiMinus()   ;
   persObj->geom[ETAPLUS]  = transObj->etaPlus()    ;
   persObj->geom[ETAMINUS] = transObj->etaMinus()   ;
   persObj->geom[ZEDPLUS]  = transObj->zedPlus()    ;
   persObj->geom[ZEDMINUS] = transObj->zedMinus()   ;
         
   persObj->ids[ROIID]     = transObj->roiId()      ;
   persObj->ids[ROIWORD]   = transObj->roiWord()    ;

   persObj->fullscan       = transObj->isFullscan() ;
   
   

   if ( transObj->size()>0 ) {

     persObj->rois.reserve(transObj->size());

     for ( unsigned i=0 ; i<transObj->size() ; i++ ) {
 
       std::vector<float> roi(9);

       const IRoiDescriptor* iroi = transObj->at(i);
 
       roi[PHI] = iroi->phi();
       roi[ETA] = iroi->eta();
       roi[ZED] = iroi->zed();

       roi[PHIMINUS] = iroi->phiMinus();
       roi[PHIPLUS]  = iroi->phiPlus();

       roi[ETAMINUS] = iroi->etaMinus();
       roi[ETAPLUS]  = iroi->etaPlus();

       roi[ZEDMINUS] = iroi->zedMinus();
       roi[ZEDPLUS]  = iroi->zedPlus();

       persObj->rois.push_back( roi );
     } 
   } 

}

