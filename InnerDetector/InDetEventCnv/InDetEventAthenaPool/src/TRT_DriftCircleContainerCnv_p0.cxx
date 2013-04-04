/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DriftCircleContainerCnv_p0.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetIdentifier/TRT_ID.h"
#include "MsgUtil.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <string>
#include <iostream>
#include <sstream>

//================================================================


StatusCode TRT_DriftCircleContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   // Get the messaging service, print where you are
   log << MSG::INFO << "TRT_DriftCircleContainerCnv::initialize()" << endreq;

   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } else {
      MSG_DEBUG(log,"Found DetectorStore.");
   }

   // Get the trt helper from the detector store
   sc = detStore->retrieve(m_trtId, "TRT_ID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get TRT_ID helper !" << endreq;
      return StatusCode::FAILURE;
   } else {
      MSG_DEBUG(log,"Found the TRT_ID helper.");
   }

   sc = detStore->retrieve(m_trtMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get TRT_DetectorDescription" << endreq;
      return sc;
   }

   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::TRT_DriftCircleContainer* TRT_DriftCircleContainerCnv_p0::createTransient(const TRT_DriftCircleContainer_p0* persObj, MsgStream& log) {

  std::auto_ptr<InDet::TRT_DriftCircleContainer> trans(new InDet::TRT_DriftCircleContainer(m_trtId->straw_layer_hash_max()) );
  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());
  
  TRT_DriftCircleContainer_p0::const_iterator it   = persObj->begin();
  TRT_DriftCircleContainer_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
      const InDet::TRT_DriftCircleCollection* dcColl = *it;
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::TRT_BaseElement * de = m_trtMgr->getElement(collHash);
      MSG_DEBUG(log,"Set TRT_DriftCircle detector element to "<< de);

      InDet::TRT_DriftCircleCollection::const_iterator itColl   = dcColl->begin();
      InDet::TRT_DriftCircleCollection::const_iterator lastColl = dcColl->end();
      for (int num = 0; itColl != lastColl; ++itColl, ++num) {
	 MSG_DEBUG(log,"PRD " << num);
         (*itColl)->m_detEl = de;
      }

      StatusCode sc= trans->addCollection(dcColl, collHash);
      if (sc.isSuccess()){
	 MSG_VERBOSE(log,"TRT_DriftCircleContainer successfully added to Container !");
      } else {
         log << MSG::ERROR << "Failed to add TRT_DriftCircleContainer to container" << endreq;
         return 0;
      }
   }
  return(trans.release());
}

